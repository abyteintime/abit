mod codegen;
mod pdbfile;

use std::{collections::HashMap, fs::File, path::PathBuf};

use anyhow::Context;
use clap::{Parser, Subcommand};
use log::{error, info, LevelFilter};

use crate::codegen::{generate_definitions, generate_header, generate_mappings};

#[derive(Parser)]
struct Args {
    #[clap(long)]
    pdb: PathBuf,

    #[clap(long)]
    dry_run: bool,

    #[clap(subcommand)]
    command: Command,
}

#[derive(Subcommand)]
enum Command {
    Addresses {
        #[clap(long)]
        output_source: PathBuf,

        #[clap(long)]
        output_headers: PathBuf,
    },
    Mappings {
        output_mappings: PathBuf,
    },
}

fn fallible_main() -> anyhow::Result<()> {
    let args = Args::parse();

    let pdb_file = File::open(args.pdb).context("cannot open PDB file")?;
    let namespaces = pdbfile::read_pdb(pdb_file).context("cannot read symbols from PDB file")?;
    let symbol_count = namespaces
        .values()
        .map(|ns| ns.symbols.len())
        .sum::<usize>();
    info!(
        "{} namespaces with {symbol_count} symbols total",
        namespaces.len()
    );

    match &args.command {
        Command::Addresses {
            output_source,
            output_headers,
        } => {
            info!("Generating headers");
            let mut headers = HashMap::new();
            for (ns_name, ns) in &namespaces {
                let header = generate_header(ns).context("cannot generate header file")?;
                headers.insert(ns_name, header);
            }

            if !args.dry_run {
                info!("Saving headers");
                std::fs::create_dir_all(output_headers)
                    .context("cannot create header output directory")?;
                for (name, header) in &headers {
                    std::fs::write(output_headers.join(format!("{name}.hpp")), header)
                        .context("cannot write header file")?;
                }
            }

            info!("Generating definitions");
            let definitions =
                generate_definitions(&namespaces).context("cannot generate definitions file")?;

            if !args.dry_run {
                info!("Saving definitions");
                std::fs::write(output_source, definitions)
                    .context("cannot write definitions file")?;
            }
        }
        Command::Mappings { output_mappings } => {
            info!("Generating mappings");
            let mappings = generate_mappings(&namespaces).context("cannot generate mappings")?;

            if !args.dry_run {
                info!("Saving mappings");
                std::fs::write(output_mappings, mappings).context("cannot write mappings file")?;
            }
        }
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
