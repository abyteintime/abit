#include "abit/ue/FName.hpp"

#include "abit/procs/47.hpp"
#include "abit/procs/7d.hpp"

using namespace ue;

void
FName::ToString(FString& outString) const
{
	if (IsValid()) {
		abit::procs::P_4793b37f6195aeea7fc3924ab846b60a_0.Call<void>(this, &outString);
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
	return abit::procs::G_7d8c3e124d62956120b42bd37431b3b5_0.As<ViewIntoTArray<FNameEntry*>>();
}
