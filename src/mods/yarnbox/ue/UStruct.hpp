#pragma once

#include "yarnbox/ue/common.hpp"

#include "yarnbox/ue/FArchive.hpp"
#include "yarnbox/ue/UObject.hpp"

namespace ue {

struct UStruct : UObject
{
	const struct VTable
	{
		yarn::UnknownFunction unknown[0x50];
		void(__thiscall* SerializeExpr)(UStruct*, int*, FArchive*);
	}* vtable;
};

}
