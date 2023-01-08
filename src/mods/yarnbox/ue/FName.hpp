#pragma once

#include <cstdint>

#include "yarnbox/ue/FString.hpp"
#include "yarnbox/ue/TArray.hpp"

namespace ue {

struct [[gnu::packed]] FNameEntry
{
	enum Flags : uint32_t
	{
		FlagsNone = 0x0,
		FlagsIsValid = 0x1,
	};

	uint64_t int1_U;
	Flags flags;
	uint64_t int2_U;
	FString name;
};

struct FName
{
	uint32_t nameIndex;
	uint32_t instanceIndex;

	void ToString(FString& outString) const;
	FString ToString() const;

	bool IsValid() const;

	static const ViewIntoTArray<FNameEntry*>* Names();
};

}
