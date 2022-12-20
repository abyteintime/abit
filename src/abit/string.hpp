#pragma once

#include <string>
#include <string_view>

#include "abit.hpp"

namespace abit {

ABIT_API std::wstring
Widen(std::string_view string);

}
