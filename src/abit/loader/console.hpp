#pragma once

#include <string_view>

#include "fmt/format.h"
#include "fmt/os.h"

namespace abit {

void
InitializeConsole();

void
Print(std::wstring_view text);

void
PrintLine(std::wstring_view text);

}
