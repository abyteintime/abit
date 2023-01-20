#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace abit {

std::vector<char>
ReadFile(const std::filesystem::path& path);

std::string
ReadFileToString(const std::filesystem::path& path);

}
