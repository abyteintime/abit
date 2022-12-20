#pragma once

#include <string>

#include "abit.hpp"

namespace abit {

struct ABIT_API Error
{
	std::string what;

	static Error System(std::string_view what);
};

}
