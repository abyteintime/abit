#pragma once

#include <string_view>
#include <type_traits>

namespace abit {

auto
InitializeDetours() -> void;

namespace impl {

auto
TypeUnsafeDetour(std::string_view name, void* detour, void** original) -> void;

}

template<typename F>
auto
Detour(std::string_view name, F detour, F& original) -> void
{
	static_assert(
		std::is_pointer_v<F> && std::is_function_v<std::remove_pointer_t<F>>,
		"F must be a function pointer"
	);

	impl::TypeUnsafeDetour(
		name, reinterpret_cast<void*>(detour), reinterpret_cast<void**>(&original)
	);
}

}
