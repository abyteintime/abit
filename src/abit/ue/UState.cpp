#include "abit/ue/UState.hpp"

#include "abit/procs/UState.hpp"

#include "abit/ue/UClass.hpp"

using namespace ue;

UClass*
UState::StaticClass()
{
	return abit::procs::UState::StaticClass.Call<UClass*>();
}
