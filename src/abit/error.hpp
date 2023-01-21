#pragma once

#include <exception>
#include <string>

#include "fmt/format.h"

namespace abit {

struct Error : std::exception
{
	std::string message;

	Error(std::string what);

	/// Convert a system error into an `abit::Error`. Uses `GetLastError()` together with
	/// `std::system_category` to create a nicer error message.
	static Error System(std::string_view what);

	template<typename... FmtArgs>
	static Error Format(FmtArgs&&... args)
	{
		return Error{ fmt::format(std::forward<FmtArgs>(args)...) };
	}

	virtual const char* what() const noexcept;
};

}
