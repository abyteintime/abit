// Templates are filtered out during pdb file loading.
pub fn is_template(name: &str) -> bool {
    name.contains('<') && name.contains('>')
}

fn is_generated(name: &str) -> bool {
    name.starts_with('\u{7f}')
        || name.starts_with('`')
        || name.contains('@')
        || name.starts_with("__IMPORT_DESCRIPTOR_")
}

pub fn is_supported_namespace_name(name: &str) -> bool {
    let is_nested_namespace = name.contains("::");
    !(is_generated(name) || is_nested_namespace)
}

pub fn is_supported_symbol_name(name: &str) -> bool {
    !is_generated(name)
        && !name.starts_with("??__C")
        && !name.starts_with('$')
        && !name.contains('.')
}
