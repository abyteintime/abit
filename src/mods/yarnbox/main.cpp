#include "abit/dll_macros.hpp"

#include "abit/loader/logging.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/FArchive.hpp"
#include "abit/procs/UObject.hpp"
#include "abit/procs/UStruct.hpp"

#include "yarnbox/ue/FArchive.hpp"
#include "yarnbox/ue/FString.hpp"
#include "yarnbox/ue/UObject.hpp"
#include "yarnbox/ue/UStruct.hpp"
#include "yarnbox/vm/opcode.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using EExprToken = yarn::Opcode;

static void __thiscall Serialize_Override(ue::FArchive* ar, void* data, int64_t size);

struct VTable : public ue::FArchive::VTable
{
	int unused_somePaddingJustInCase[16];
	int callCounter = 0;
	void(__thiscall* O_Serialize)(ue::FArchive* ar, void* data, int64_t size);

	VTable(const ue::FArchive::VTable& base)
	  : ue::FArchive::VTable(base)
	{
		O_Serialize = base.Serialize;
		Serialize = Serialize_Override;
	}
};

static void __thiscall Serialize_Override(ue::FArchive* ar, void* data, int64_t size)
{
	VTable* goodVtable = (VTable*)ar->vtable;
	goodVtable->callCounter += 1;
	if (goodVtable->O_Serialize == &Serialize_Override) {
		spdlog::error("O_Serialize was set to Serialize_Override and would call itself recursively"
		);
		return;
	}
	(goodVtable->O_Serialize)(ar, data, size);
}

static void(__thiscall* O_UObject_Serialize)(ue::UObject*, ue::FArchive*);
static void __thiscall UObject_Serialize(ue::UObject* self, ue::FArchive* ar)
{
	ue::FString name;
	ue::UObject::GetName(self, &name);
	spdlog::info(L"UObject: {}\tDataOnly={}", name.to_wstring_view(), ar->dataOnly);

	ue::FArchive::VTable* oldVtable = ar->vtable;
	VTable sus(*oldVtable);
	ar->vtable = &sus;

	O_UObject_Serialize(self, ar);

	ar->vtable = oldVtable;
	spdlog::debug("    {} calls to Serialize were made", sus.callCounter);
}

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::Patch(abit::procs::UObject::Serialize, &UObject_Serialize, O_UObject_Serialize);
}
