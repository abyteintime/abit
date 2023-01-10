#include "abit/ue/UStruct.hpp"

#include "abit/procs/UStruct.hpp"

using namespace ue;

struct UClass*
UStruct::StaticClass()
{
	return abit::procs::UStruct::StaticClass.Call<struct UClass*>();
}

bool
UStruct::Is(UStruct* other) const
{
	const UStruct* p = this;
	do {
		if (p == other) {
			return true;
		}
		p = p->parentType;
	} while (p != nullptr);
	return false;
}
