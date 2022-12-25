#pragma once

#include <cstdint>

namespace ue {

struct FString
{
	wchar_t* dataPtr = nullptr;
	int32_t length = 0;
	int32_t capacity = 0;

	FString();
	FString(const FString&) = delete;

	FString(FString&&);
	~FString();
};

}
