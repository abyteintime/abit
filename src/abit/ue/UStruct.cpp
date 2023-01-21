#include "abit/ue/UStruct.hpp"

#include "abit/procs/d8.hpp"

using namespace ue;

struct UClass*
UStruct::StaticClass()
{
	return abit::procs::P_d8699dbef475aab31a813e5ddc80c6bb_0.Call<struct UClass*>();
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
