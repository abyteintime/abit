#pragma once

#include "fmt/format.h"

#include "abit/error.hpp"

namespace abit {

template<typename F>
void
Ensure(bool condition, F&& errorMessageCallback)
{
	if (!condition) {
		throw abit::Error{ errorMessageCallback() };
	}
}

}

#define ABIT_ENSURE(Condition, Format, ...) \
	::abit::Ensure((Condition), [&] { return ::fmt::format(Format, ##__VA_ARGS__); })
