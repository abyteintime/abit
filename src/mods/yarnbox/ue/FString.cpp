#include "yarnbox/ue/FString.hpp"

#include "abit/procs/FString.hpp"

ue::FString::FString() {}

ue::FString::FString(FString&& moving)
{
	moving.dataPtr = nullptr;
	moving.length = 0;
	moving.capacity = 0;
}

ue::FString::~FString()
{
	abit::procs::FString::_destructor.Call<abit::ThisCall, void>(this);
}
