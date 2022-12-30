#pragma once

#include "yarnbox/ue/common.hpp"

#include "yarnbox/ue/FArchive.hpp"
#include "yarnbox/ue/TArray.hpp"
#include "yarnbox/ue/UObject.hpp"

namespace ue {

struct [[gnu::packed]] UStruct : public UObject
{
private:
	uint8_t __unknownPadding__[44];

public:
	// Oddity: this field seems to be at offset 140 despite `TArray<T>` having an alignment
	// requirement of 8, and 140 is not divisible by 8.
	ViewIntoTArray<uint8_t> bytecode;
};

}
