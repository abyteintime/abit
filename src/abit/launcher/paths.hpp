#pragma once

#include <string>

namespace abit {

std::wstring
GetGameExecutablePath(std::string_view installDirectory);

std::wstring
GetLoaderDllPath();

}
