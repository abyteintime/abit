#pragma once

#include <string>

#include "abit.hpp"

namespace abit {

struct ABIT_API Error
{
	std::string what;

	static auto System(std::string_view what) -> Error;
};

}
