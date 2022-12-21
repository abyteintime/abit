import collections
import csv

Symbol = collections.namedtuple("Row", "location")

def load_symbols(file):
    namespaces = {}

    reader = csv.reader(file, dialect="unix", escapechar="\\")
    next(reader)  # discard header row
    for row in reader:
        [namespace, name, location, *_] = row
        namespace_dict = namespaces.get(namespace, {})
        namespace_dict[name] = Symbol(location)
        namespaces[namespace] = namespace_dict

    return namespaces

def filter_namespaces(namespaces: dict):
    def is_unsupported_namespace(name: str):
        is_inner_namespace = "::" in name  # for now these are not supported
        is_template = '<' in name and '>' in name
        is_generated = name[0] == '`' or name[0] == '_'
        return is_inner_namespace or is_template or is_generated

    to_remove = [name for name in namespaces.keys() if is_unsupported_namespace(name)]
    for name in to_remove:
        del namespaces[name]

def filter_symbols(namespace: dict):
    def is_unsupported_symbol(name: str):
        is_template = '<' in name and '>' in name
        is_generated = '`' in name
        is_exception_handling = name.startswith("Unwind@") or name.startswith("Catch@") or name.startswith("Catch_All@")
        is_mangled = name.startswith("?")
        return is_template or is_generated or is_exception_handling or is_mangled

    to_remove = [name for name in namespace.keys() if is_unsupported_symbol(name)]
    for name in to_remove:
        del namespace[name]

BASE_ADDRESS = 0x140000000
