#include "yarnbox/ue/FName.hpp"

#include "abit/procs/FName.hpp"

using namespace ue;

void
FName::ToString(FString& outString) const
{
	if (IsValid()) {
		abit::procs::FName::ToString_52c84dce.Call<void>(this, &outString);
	} else {
		outString.dataPtr = nullptr;
		outString.length = 0;
		outString.capacity = 0;
	}
}

FString
FName::ToString() const
{
	FString string;
	ToString(string);
	return string;
}

bool
FName::IsValid() const
{
	return nameIndex < Names()->length && (*Names())[nameIndex] != nullptr;
}

const ViewIntoTArray<FNameEntry*>*
FName::Names()
{
	return abit::procs::FName::Names.As<ViewIntoTArray<FNameEntry*>>();
}
