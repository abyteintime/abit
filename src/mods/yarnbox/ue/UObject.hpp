#pragma once

#include "yarnbox/ue/FName.hpp"
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
	uint64_t int2_U;
	int32_t objectIndex;
	int32_t int_U;
	UObject* outer;
	FName ptr6_U;
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

	bool InstanceOf(struct UClass* type) const;

	template<typename T>
	bool Is() const
	{
		return Is(T::StaticClass());
	}

private:
	/// Used to tell the compiler that this type has a vtable pointer in its layout.
	/// Do not use.
	virtual void __thereIsAVTableInMyHouse__();
};

}
