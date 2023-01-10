#include "yarnbox/ue/UState.hpp"

#include "abit/procs/UState.hpp"

#include "yarnbox/ue/UClass.hpp"

using namespace ue;

UClass*
UState::StaticClass()
{
	return abit::procs::UState::StaticClass.Call<UClass*>();
}
