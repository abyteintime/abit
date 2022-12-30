#include "yarnbox/ue/UStruct.hpp"

#include "abit/procs/UStruct.hpp"

using namespace ue;

struct UClass*
UStruct::StaticClass()
{
	return abit::procs::UStruct::StaticClass.Call<struct UClass*>();
}
