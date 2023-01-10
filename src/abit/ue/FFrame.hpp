#pragma once

#include "abit/ue/UFunction.hpp"

namespace ue {

struct [[gnu::packed]] FFrame
{
private:
	uint8_t __unknown__[12];

public:
	UFunction* function;
	UObject* context;
	uint8_t* ip;

private:
	/// Used to tell the compiler that this type has a vtable pointer in its layout.
	/// Do not use.
	virtual void __thereIsAVTableInMyHouse__();
};

}
