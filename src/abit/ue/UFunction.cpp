#include "abit/ue/UFunction.hpp"

#include "abit/procs/UFunction.hpp"

using namespace ue;

struct UClass*
UFunction::StaticClass()
{
	// UFunction::StaticClass seems to be optimized out by LTO, so we need to implement it
	// ourselves.

	struct UClass*& privateStaticClass =
		*abit::procs::UFunction::PrivateStaticClass.As<struct UClass*>();

	if (privateStaticClass == nullptr) {
		privateStaticClass =
			abit::procs::UFunction::GetPrivateStaticClassUFunction.Call<struct UClass*>(L"Core");
		abit::procs::UFunction::InitializePrivateStaticClassUFunction.Call<void>();
	}

	return privateStaticClass;
}
