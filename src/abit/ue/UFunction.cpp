#include "abit/ue/UFunction.hpp"

#include "abit/procs/63.hpp"
#include "abit/procs/8d.hpp"
#include "abit/procs/bb.hpp"

using namespace ue;

struct UClass*
UFunction::StaticClass()
{
	// UFunction::StaticClass seems to be optimized out by LTO, so we need to implement it
	// ourselves.

	struct UClass*& privateStaticClass
		= *abit::procs::G_bb9dc293b83c02e1c2d6f0fd99db2c9a_0.As<struct UClass*>();

	if (privateStaticClass == nullptr) {
		privateStaticClass
			= abit::procs::P_8d7caf437c7810a6ee21b7b9e95352e7_0.Call<struct UClass*>(L"Core");
		abit::procs::P_633ef314955afb4ed5c35d008ce2e074_0.Call<void>();
	}

	return privateStaticClass;
}
