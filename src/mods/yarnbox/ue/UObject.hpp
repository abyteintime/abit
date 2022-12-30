#pragma once

#include "yarnbox/ue/FString.hpp"

namespace ue {

enum class EObjectFlags
{
	None = 0x0,
};

struct UObject
{
	void* ptr1_U;
	EObjectFlags objectFlags;
	void* ptr2_U;
	void* ptr3_U;
	void* ptr4_U;
	void* autoRegister;
	int32_t objectIndex;
	int32_t int_U;
	UObject* outer;
	void* ptr6_U;
	struct UClass* objectClass;
	void* ptr7_U;

	static struct UClass* StaticClass();

	void GetName(FString& outString) const;
	FString GetName() const;

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
