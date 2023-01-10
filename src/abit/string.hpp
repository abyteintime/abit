#pragma once

#include <string>
#include <string_view>

#include "abit/abit.hpp"

namespace abit {

ABIT_API std::wstring
Widen(std::string_view string);

ABIT_API std::string
Narrow(std::wstring_view string);

ABIT_API char
CharToLowerAscii(char c);
ABIT_API char
CharToUpperAscii(char c);

ABIT_API std::string
StringToLowerAscii(std::string_view string);
ABIT_API std::string
StringToUpperAscii(std::string_view string);

}
