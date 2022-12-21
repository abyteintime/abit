#pragma once

#include <string_view>
#include <type_traits>

#include "abit/loader/proc.hpp"

namespace abit {

void
InitializePatches();

namespace impl {

void
TypeUnsafePatch(const Proc& proc, void* detour, void** original);

}

template<typename F>
void
Patch(const Proc& proc, F detour, F& original)
{
	static_assert(
		std::is_pointer_v<F> && std::is_function_v<std::remove_pointer_t<F>>,
		"F must be a function pointer"
	);

	impl::TypeUnsafePatch(
		proc, reinterpret_cast<void*>(detour), reinterpret_cast<void**>(&original)
	);
}

}
