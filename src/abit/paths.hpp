#pragma once

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "abit/abit.hpp"

namespace abit {

ABIT_API std::wstring
GetExecutablePath(HINSTANCE instance = nullptr);

}
