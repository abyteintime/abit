#include "abit/dll_macros.hpp"

#include "abit/loader/console.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/AHat_MusicTreePlayer.hpp"
#include "abit/procs/ULinkerLoad.hpp"
#include "abit/procs/UStruct.hpp"

#include "yarnbox/ue/FArchive.hpp"
#include "yarnbox/ue/FString.hpp"
#include "yarnbox/ue/UObject.hpp"
#include "yarnbox/ue/UStruct.hpp"
#include "yarnbox/vm/opcode.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct FFrame;
struct ULinkerLoad;
struct UObject;

using EExprToken = yarn::Opcode;

static EExprToken(__thiscall* O_SerializeExpr)(ue::UStruct*, int*, ue::FArchive*);
static EExprToken __thiscall SerializeExpr(ue::UStruct* self, int* ip, ue::FArchive* ar)
{
	return O_SerializeExpr(self, ip, ar);
}

static void(__thiscall* O_UStruct_Serialize)(ue::UStruct*, ue::FArchive*);
static void __thiscall UStruct_Serialize(ue::UStruct* self, ue::FArchive* ar)
{
	O_UStruct_Serialize(self, ar);
	ue::FString name;
	ue::UObject::GetName(self, &name);
	abit::PrintLine(std::wstring(L"Deserialized UStruct: ") + name.dataPtr);
}

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::Patch(abit::procs::UStruct::SerializeExpr, &SerializeExpr, O_SerializeExpr);
	abit::Patch(abit::procs::UStruct::Serialize, &UStruct_Serialize, O_UStruct_Serialize);
}
