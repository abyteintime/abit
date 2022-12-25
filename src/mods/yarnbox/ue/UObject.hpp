#pragma once

#include "yarnbox/ue/FString.hpp"

namespace ue {

struct UObject
{
	static void GetName(UObject* self, FString* outString);
};

}
