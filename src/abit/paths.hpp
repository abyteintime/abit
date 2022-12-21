#pragma once

#include <filesystem>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "abit/abit.hpp"

namespace abit {

ABIT_API std::filesystem::path
GetExecutablePath(HINSTANCE instance = nullptr);

ABIT_API std::filesystem::path
GetConfigPath(HINSTANCE instance = nullptr);

}
