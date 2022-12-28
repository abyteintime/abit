#pragma once

#include "yarnbox/ue/FString.hpp"

namespace ue {

enum class EObjectFlags
{
	None = 0x0,
};

struct UObject
{
	EObjectFlags flags;
	int32_t internalIndex;
	class UClass* objectClass;

	void GetName(FString* outString);

	template<typename VTable>
	VTable*& GetVTable()
	{
		return *reinterpret_cast<VTable**>(this);
	}

private:
	/// Used to tell the compiler that this type has a vtable pointer in its layout.
	/// Do not use.
	virtual void __thereIsAVTableInMyHouse__();
};

}
