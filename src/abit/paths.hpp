#pragma once

#include <filesystem>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace abit {

std::filesystem::path
GetExecutablePath(HINSTANCE instance = nullptr);

std::filesystem::path
GetConfigPath(HINSTANCE instance = nullptr);

}
