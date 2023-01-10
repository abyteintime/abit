#pragma once

#include "abit/ue/common.hpp"

#include "abit/ue/FArchive.hpp"
#include "abit/ue/TArray.hpp"
#include "abit/ue/UObject.hpp"

namespace ue {

struct [[gnu::packed]] UStruct : public UObject
{
private:
	uint8_t __unknownPadding1__[24];

public:
	UStruct* parentType;

private:
	uint8_t __unknownPadding2__[12];

public:
	// Oddity: this field seems to be at offset 140 despite `TArray<T>` having an alignment
	// requirement of 8, and 140 is not divisible by 8.
	ViewIntoTArray<uint8_t> bytecode;

	static struct UClass* StaticClass();

	bool Is(UStruct* other) const;

	template<typename T>
	bool Is() const
	{
		return Is(T::StaticClass());
	}
};

}
