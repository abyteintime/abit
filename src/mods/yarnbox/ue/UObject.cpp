#include "yarnbox/ue/UObject.hpp"

#include "abit/procs/UObject.hpp"

void
ue::UObject::GetName(FString* outString)
{
	abit::procs::UObject::GetName_140094c10.Call<abit::ThisCall, void>(this, outString);
}

void
ue::UObject::__thereIsAVTableInMyHouse__()
{
}
