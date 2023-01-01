#include "yarnbox/ue/FString.hpp"

#include "abit/string.hpp"

#include "abit/procs/FString.hpp"

#include "yarnbox/ue/memory.hpp"

using namespace ue;

FString::FString() {}
FString::FString(const FString& other)
{
	length = other.length;
	capacity = other.length;
	if (length != 0 && other.dataPtr != nullptr) {
		dataPtr = reinterpret_cast<wchar_t*>(appMalloc(capacity * sizeof(wchar_t)));
		memcpy(dataPtr, other.dataPtr, other.length * sizeof(wchar_t));
	} else {
		dataPtr = nullptr;
	}
}

FString::FString(FString&& other)
{
	other.dataPtr = nullptr;
	other.length = 0;
	other.capacity = 0;
}

FString::~FString()
{
	abit::procs::FString::_destructor.Call<void>(this);
}

std::wstring_view
FString::ToWstringView() const
{
	return std::wstring_view{ dataPtr, size_t(length - 1) };
}

std::string
FString::ToString() const
{
	return abit::Narrow(ToWstringView());
}
