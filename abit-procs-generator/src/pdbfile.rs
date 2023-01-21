use std::{
    collections::HashMap,
    fmt::Debug,
    hash::Hash,
    io::{Read, Seek},
};

use anyhow::Context;
use pdb::FallibleIterator;
use symbolic_common::{Language, Name, NameMangling};
use symbolic_demangle::{Demangle, DemangleOptions};
use twox_hash::{xxh3::HasherExt, Xxh3Hash128};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SymbolKind {
    Function,
    Data,
}

#[derive(Debug)]
pub struct Symbol {
    pub kind: SymbolKind,
    pub mangled_name: String,
    pub demangled_name: String,
    pub full_demangled_name: String,
    pub exposed_name: String,
    pub address: u32,
}

impl Symbol {
    fn get_exposed_name(mangled_name: &str) -> String {
        let mut hasher = Xxh3Hash128::default();
        mangled_name.hash(&mut hasher);
        let hash = hasher.finish_ext();
        format!("{hash:032x}")
    }
}

#[derive(Debug)]
pub struct SymbolGroup {
    pub overloads: Vec<Symbol>,
}

impl SymbolGroup {
    pub fn insert(group: Option<Self>, symbol: Symbol) -> Self {
        match group {
            None => Self {
                overloads: vec![symbol],
            },
            Some(mut group) => {
                group.overloads.push(symbol);
                group.overloads.dedup_by(|a, b| a.address == b.address);
                group
            }
        }
    }
}

#[derive(Debug, Default)]
pub struct Namespace {
    pub symbols: HashMap<String, SymbolGroup>,
}

pub fn read_pdb<R>(reader: R) -> anyhow::Result<HashMap<String, Namespace>>
where
    R: Debug + Read + Seek,
{
    let mut pdb = pdb::PDB::open(reader).context("cannot read PDB file")?;

    let symbol_table = pdb
        .global_symbols()
        .context("cannot obtain symbol table from PDB")?;
    let address_map = pdb
        .address_map()
        .context("cannot obtain address map from PDB")?;

    let mut namespaces = HashMap::new();

    let mut symbol_iterator = symbol_table.iter();
    while let Some(symbol) = symbol_iterator.next()? {
        let symbol = symbol.parse()?;
        let symbol = match symbol {
            pdb::SymbolData::Data(data) => {
                let name = data.name.to_string().to_string();
                let rva = data.offset.to_rva(&address_map).unwrap_or_default();
                Symbol {
                    kind: SymbolKind::Data,
                    mangled_name: name.clone(),
                    demangled_name: name.clone(),
                    exposed_name: Symbol::get_exposed_name(&name),
                    full_demangled_name: name,
                    address: rva.0,
                }
            }
            pdb::SymbolData::Public(data) => {
                let name = Name::new(data.name.to_string(), NameMangling::Unknown, Language::Cpp);
                let demangled = name.try_demangle(DemangleOptions::name_only());
                let demangled_full = name.try_demangle(DemangleOptions::complete());
                let rva = data.offset.to_rva(&address_map).unwrap_or_default();
                Symbol {
                    kind: if data.function {
                        SymbolKind::Function
                    } else {
                        SymbolKind::Data
                    },
                    // Double to_string needed because the first one returns a Cow<'a, String>.
                    mangled_name: data.name.to_string().to_string(),
                    demangled_name: demangled.to_string(),
                    full_demangled_name: demangled_full.to_string(),
                    exposed_name: Symbol::get_exposed_name(&data.name.to_string()),
                    address: rva.0,
                }
            }
            _ => continue,
        };

        let (namespace_name, name_in_namespace) =
            (symbol.exposed_name.split_at(2).0, &symbol.exposed_name);
        let namespace = namespaces
            .entry(namespace_name.to_string())
            .or_insert_with(Namespace::default);
        let group = namespace.symbols.remove(name_in_namespace);
        namespace.symbols.insert(
            name_in_namespace.to_string(),
            SymbolGroup::insert(group, symbol),
        );
    }

    Ok(namespaces)
}
