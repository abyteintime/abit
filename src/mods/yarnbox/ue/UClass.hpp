#pragma once

#include "yarnbox/ue/UState.hpp"

namespace ue {

struct UClass : UState
{
	static UClass* StaticClass();
};

}