#pragma once

#include <string>
#include <string_view>

namespace abit {

std::wstring
Widen(std::string_view string);

std::string
Narrow(std::wstring_view string);

char
CharToLowerAscii(char c);
char
CharToUpperAscii(char c);

std::string
StringToLowerAscii(std::string_view string);
std::string
StringToUpperAscii(std::string_view string);

bool
StartsWithCaseInsensitive(std::wstring_view string, std::wstring_view prefix);

}
