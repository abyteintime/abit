use std::{collections::HashMap, fmt::Write};

use chrono::Utc;

use crate::pdbfile::{Namespace, Symbol, SymbolKind};

fn symbol_declaration_type(symbol_kind: SymbolKind) -> &'static str {
    match symbol_kind {
        SymbolKind::Function => "abit::Proc",
        SymbolKind::Data => "abit::Global",
    }
}

pub fn symbol_name_identifier(symbol: &Symbol, overload: usize) -> String {
    match symbol.kind {
        SymbolKind::Function => format!("P_{}_{}", symbol.exposed_name, overload),
        SymbolKind::Data => format!("G_{}_{}", symbol.exposed_name, overload),
    }
}

pub fn generate_header(ns: &Namespace) -> anyhow::Result<String> {
    let mut header = String::new();

    writeln!(header, "// Header generated by abit-procs-generator")?;
    writeln!(header, "// at {}", Utc::now())?;
    writeln!(header)?;
    writeln!(header, r#"#include "abit/procs/base.hpp""#)?;
    writeln!(header)?;

    writeln!(header, "namespace abit::procs {{")?;

    for symbol_group in ns.symbols.values() {
        for (i, symbol) in symbol_group.overloads.iter().enumerate() {
            let decl_type = symbol_declaration_type(symbol.kind);
            let symbol_name = symbol_name_identifier(symbol, i);
            writeln!(header, "ABIT_DLL_IMPORT extern {decl_type} {symbol_name};")?;
        }
    }

    writeln!(header, "}}")?;

    Ok(header)
}

pub fn generate_definitions(namespaces: &HashMap<String, Namespace>) -> anyhow::Result<String> {
    let mut source = String::new();

    writeln!(source, "// Definitions generated by abit-procs-generator")?;
    writeln!(source, "// at {}", Utc::now())?;
    writeln!(source)?;
    writeln!(source, r#"#include "abit/procs/base.hpp""#)?;
    writeln!(source)?;

    for ns in namespaces.values() {
        writeln!(source, "namespace abit::procs {{")?;
        for (symbol_name, symbol_group) in &ns.symbols {
            for (i, symbol) in symbol_group.overloads.iter().enumerate() {
                let decl_type = symbol_declaration_type(symbol.kind);
                let symbol_name_identifier = symbol_name_identifier(symbol, i);
                let Symbol { ref address, .. } = symbol;
                writeln!(
					source,
					"ABIT_DLL_EXPORT {decl_type} {symbol_name_identifier}{{ {symbol_name:?}, (void*)0x{address:x} }};"
				)?;
            }
        }
        writeln!(source, "}}")?;
        writeln!(source)?;
    }

    Ok(source)
}

pub fn generate_mappings(namespaces: &HashMap<String, Namespace>) -> anyhow::Result<String> {
    let mut mappings = String::new();

    let mut entries = namespaces
        .values()
        .flat_map(|ns| ns.symbols.values())
        .flat_map(|symbol_group| symbol_group.overloads.iter().enumerate())
        .map(|(i, symbol)| (symbol_name_identifier(symbol, i), symbol))
        .collect::<Vec<_>>();
    entries.sort_by_key(|(_, symbol)| &symbol.demangled_name);
    for (hashed_name, symbol) in entries {
        writeln!(
            mappings,
            "{hashed_name}\t{:100}\t{}",
            symbol.demangled_name, symbol.full_demangled_name
        )?;
    }

    Ok(mappings)
}
