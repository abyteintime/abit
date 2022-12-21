#pragma once

#include <filesystem>
#include <string_view>
#include <vector>

namespace abit {

std::vector<char>
ReadFile(const std::filesystem::path& path);

}
