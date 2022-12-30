#pragma once

#include "fmt/format.h"

#include "yarnbox/ue/FString.hpp"
#include "yarnbox/ue/UClass.hpp"
#include "yarnbox/ue/UObject.hpp"

namespace ue {

struct UObjectFmt
{
	const UObject* object;
};

}

/// Adds {fmt} formatting support for `UObject` pointers.
///
/// Supported specifiers:
/// - `I` - enable index printing
/// - `p` - enable pointer printing
template<>
struct fmt::formatter<ue::UObjectFmt>
{
	bool showIndex = false;
	bool showPointer = false;

	constexpr auto parse(format_parse_context& context) -> decltype(context.begin())
	{
		auto it = context.begin();
		auto end = context.end();

		if (it != end && *it == 'i') {
			showIndex = true;
			++it;
		}
		if (it != end && *it == 'p') {
			showPointer = true;
			++it;
		}

		if (it != end && *it != '}') {
			throw format_error("invalid format");
		}

		return it;
	}

	template<typename Context>
	auto format(const ue::UObjectFmt& fmt, Context& context) -> decltype(context.out()) const
	{
		const ue::UObject* object = fmt.object;
		if (object != nullptr) {
			if (showIndex) {
				fmt::format_to(context.out(), "{}:", object->objectIndex);
			}
			ue::FString className = object->objectClass->GetName();
			ue::FString name = object->GetName();
			fmt::format_to(
				context.out(), L"{}'{}'", className.ToWstringView(), name.ToWstringView()
			);
			if (showPointer) {
				fmt::format_to(context.out(), " ({})", reinterpret_cast<const void*>(object));
			}
		} else {
			fmt::format_to(context.out(), "None");
		}
		return context.out();
	}
};
