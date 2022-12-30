#pragma once

#include "yarnbox/ue/UState.hpp"

namespace ue {

struct UClass : public UState
{
	static UClass* StaticClass();
};

}