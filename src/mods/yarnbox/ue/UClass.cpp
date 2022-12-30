#include "yarnbox/ue/UClass.hpp"

#include "abit/procs/UClass.hpp"

ue::UClass*
ue::UClass::StaticClass()
{
	return abit::procs::UClass::StaticClass.Call<UClass*>();
}
