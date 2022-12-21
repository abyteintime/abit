#pragma once

#include <string_view>

namespace abit {

void
InitializeConsole();

void
Print(std::wstring_view text);

void
PrintLine(std::wstring_view text);

}
