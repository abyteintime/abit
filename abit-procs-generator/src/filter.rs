pub fn is_internal_symbol(demangled_name: &str) -> bool {
    (demangled_name.contains('`') && !demangled_name.contains("`vftable'"))
        || demangled_name.contains('$')
        || (demangled_name.contains('@') && !demangled_name.contains("GRegisterNative"))
        || demangled_name.starts_with("__imp_")
}
