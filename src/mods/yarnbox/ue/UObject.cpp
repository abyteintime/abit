#include "yarnbox/ue/UObject.hpp"

#include "abit/procs/UObject.hpp"

using namespace ue;

void
UObject::GetName(FString& outString) const
{
	abit::procs::UObject::GetName_140094c10.Call<abit::ThisCall, void>(this, &outString);
}

FString
UObject::GetName() const
{
	FString name;
	GetName(name);
	return name;
}

void
UObject::__thereIsAVTableInMyHouse__()
{
}
