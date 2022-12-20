#pragma once

#include "abit.hpp"

namespace abit {

ABIT_API auto
Version() -> const char*;

ABIT_API auto
VersionWide() -> const wchar_t*;

}
