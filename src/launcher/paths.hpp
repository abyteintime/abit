#pragma once

#include <string>

namespace abit {

auto
GetGameExecutablePath(std::string_view installDirectory) -> std::wstring;

auto
GetThisExecutablePath() -> std::wstring;

auto
GetLoaderDllPath() -> std::wstring;

}
