#include "yarnbox/ue/UObject.hpp"

#include "abit/procs/UObject.hpp"

#include "yarnbox/ue/UClass.hpp"

using namespace ue;

struct UClass*
UObject::StaticClass()
{
	return abit::procs::UObject::StaticClass.Call<struct UClass*>();
}

void
UObject::GetName(FString& outString) const
{
	abit::procs::UObject::GetName_de4ee4a5.Call<void>(this, &outString);
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
