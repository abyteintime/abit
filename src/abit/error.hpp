#pragma once

#include <exception>
#include <string>

#include "abit/abit.hpp"

namespace abit {

struct ABIT_API Error : std::exception
{
	std::string message;

	Error(std::string what);

	/// Convert a system error into an `abit::Error`. Uses `GetLastError()` together with
	/// `std::system_category` to create a nicer error message.
	static Error System(std::string_view what);

	virtual const char* what() const noexcept;
};

}
