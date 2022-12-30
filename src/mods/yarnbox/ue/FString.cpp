#include "yarnbox/ue/FString.hpp"

#include "abit/string.hpp"

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
	abit::procs::FString::_destructor.Call<void>(this);
}

std::wstring_view
ue::FString::ToWstringView() const
{
	return std::wstring_view{ dataPtr, size_t(length - 1) };
}

std::string
ue::FString::ToString() const
{
	return abit::Narrow(ToWstringView());
}
