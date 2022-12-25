#include "yarnbox/ue/UObject.hpp"

#include "abit/procs/UObject.hpp"

void
ue::UObject::GetName(UObject* self, FString* outString)
{
	abit::procs::UObject::GetName_140094c10.Call<abit::ThisCall, void>(self, outString);
}
