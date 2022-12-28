#pragma once

#include <string>

#include "abit/abit.hpp"

namespace abit {

struct ABIT_API Error
{
	std::string what;

	Error(std::string what);

	/// Convert a system error into an `abit::Error`. Uses `GetLastError()` together with
	/// `std::system_category` to create a nicer error message.
	static Error System(std::string_view what);
};

}
