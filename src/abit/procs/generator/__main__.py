import argparse
from pathlib import Path

import cpp_code
import ghidra_output

parser = argparse.ArgumentParser(prog="procs_generator")
parser.add_argument("--input", help="Ghidra symbol table that should be used for generation", required=True)
parser.add_argument("--output-source", help="Where to output the .cpp file containing all definitions", required=True)
parser.add_argument("--output-headers", help="Where to output .hpp files with definitions", required=True)

args = parser.parse_args()

namespaces = None
with open(args.input, "r") as input_file:
    namespaces = ghidra_output.load_symbols(input_file)
symbol_count = sum(map(lambda namespace: len(namespace), namespaces))
print(f"{len(namespaces)} namespaces loaded total with {symbol_count} symbols")

ghidra_output.filter_namespaces(namespaces)
for namespace in namespaces.values():
    ghidra_output.filter_symbols(namespace)
symbol_count = sum(map(lambda namespace: len(namespace), namespaces))
print(f"{len(namespaces)} namespaces and {symbol_count} symbols after filtering")

print("Generating headers")
headers = []
for namespace_name, namespace in namespaces.items():
    headers.append(cpp_code.generate_header(namespace_name, namespace))

print("Saving headers")
header_output_path = Path(args.output_headers)
header_output_path.mkdir(parents=True, exist_ok=True)
for filename, header in headers:
    path = header_output_path.joinpath(Path(filename))
    with open(path, "w") as output_file:
        output_file.write(header.source_code)

print("Generating source")
source = cpp_code.generate_cpp(namespaces, headers)

print("Saving source")
with open(args.output_source, "w") as output_file:
    output_file.write(source)

