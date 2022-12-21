#pragma once

#include <filesystem>
#include <string>

namespace abit {

std::filesystem::path
GetGameExecutablePath(std::string_view installDirectory);

std::filesystem::path
GetLoaderDllPath();

}
