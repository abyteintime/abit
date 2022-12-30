#pragma once

#include "yarnbox/ue/UStruct.hpp"

namespace ue {

struct UFunction : public UStruct
{
	static struct UClass* StaticClass();
};

}
