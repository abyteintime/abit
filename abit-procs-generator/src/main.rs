mod codegen;
mod filter;
mod pdbfile;

use std::{collections::HashMap, fs::File, path::PathBuf};

use anyhow::Context;
use clap::Parser;
use log::{error, info, LevelFilter};

use crate::{
    codegen::{generate_definitions, generate_header},
    filter::{is_supported_namespace_name, is_supported_symbol_name},
};

#[derive(Parser)]
struct Args {
    #[clap(long)]
    pdb: PathBuf,

    #[clap(long)]
    output_source: PathBuf,

    #[clap(long)]
    output_headers: PathBuf,

    #[clap(long)]
    dry_run: bool,
}

fn fallible_main() -> anyhow::Result<()> {
    let args = Args::parse();

    let pdb_file = File::open(args.pdb).context("cannot open PDB file")?;
    let mut namespaces =
        pdbfile::read_pdb(pdb_file).context("cannot read symbols from PDB file")?;
    let symbol_count = namespaces
        .values()
        .map(|ns| ns.symbols.len())
        .sum::<usize>();
    info!(
        "{} namespaces with {symbol_count} symbols total",
        namespaces.len()
    );

    namespaces.retain(|name, ns| {
        ns.symbols
            .retain(|name, _symbol| is_supported_symbol_name(name));
        !ns.symbols.is_empty() && is_supported_namespace_name(name)
    });
    let symbol_count = namespaces
        .values()
        .map(|ns| ns.symbols.len())
        .sum::<usize>();
    info!(
        "{} namespaces with {symbol_count} symbols after filtering",
        namespaces.len()
    );

    info!("Generating headers");
    let mut headers = HashMap::new();
    for (ns_name, ns) in &namespaces {
        let header = generate_header(ns_name, ns).context("cannot generate header file")?;
        headers.insert(ns_name, header);
    }

    if !args.dry_run {
        info!("Saving headers");
        std::fs::create_dir_all(&args.output_headers)
            .context("cannot create header output directory")?;
        for (name, header) in &headers {
            std::fs::write(args.output_headers.join(format!("{name}.hpp")), header)
                .context("cannot write header file")?;
        }
    }

    info!("Generating definitions");
    let definitions =
        generate_definitions(&namespaces).context("cannot generate definitions file")?;

    if !args.dry_run {
        info!("Saving definitions");
        std::fs::write(&args.output_source, definitions)
            .context("cannot write definitions file")?;
    }

    Ok(())
}

fn main() {
    env_logger::builder()
        .filter_module("abit_procs_generator", LevelFilter::Info)
        .format_timestamp(None)
        .init();

    match fallible_main() {
        Ok(()) => (),
        Err(err) => error!("{err}"),
    }
}
