#include "abit/ue/UObject.hpp"

#include "abit/procs/15.hpp"
#include "abit/procs/aa.hpp"

#include "abit/ue/UClass.hpp"

using namespace ue;

struct UClass*
UObject::StaticClass()
{
	return abit::procs::P_1585a0abbd53b2dea0804a2147ef4931_0.Call<struct UClass*>();
}

void
UObject::GetName(FString& outString) const
{
	abit::procs::P_aac20ce61058b549d5267317945f1985_0.Call<void>(this, &outString);
}

FString
UObject::GetName() const
{
	FString name;
	GetName(name);
	return name;
}

bool
UObject::InstanceOf(UClass* type) const
{
	return objectClass->Is(type);
}

void
UObject::__thereIsAVTableInMyHouse__()
{
}
