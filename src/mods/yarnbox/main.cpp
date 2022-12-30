#include "abit/dll_macros.hpp"

#include <unordered_map>

#include "fmt/ranges.h"

#include "abit/loader/logging.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/FArchive.hpp"
#include "abit/procs/UObject.hpp"
#include "abit/procs/UStruct.hpp"

#include "yarnbox/bytecode/disassembler.hpp"
#include "yarnbox/bytecode/opcode.hpp"

#include "yarnbox/ue/FArchive.hpp"
#include "yarnbox/ue/FString.hpp"
#include "yarnbox/ue/UClass.hpp"
#include "yarnbox/ue/UFunction.hpp"
#include "yarnbox/ue/UObject.hpp"
#include "yarnbox/ue/UObject/fmt.hpp"
#include "yarnbox/ue/UStruct.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct DisassembledFunction
{
	ue::UFunction* ufunction;
	yarn::BytecodeTree tree;
};

struct DisassembledUStruct
{
	ue::UStruct* ustruct;
	std::unordered_map<std::string, DisassembledFunction> functions;
};

static void (*O_UStruct_Serialize)(ue::UStruct*, ue::FArchive*);
static void
UStruct_Serialize(ue::UStruct* self, ue::FArchive* ar)
{
	ue::FString name;
	self->GetName(name);
	O_UStruct_Serialize(self, ar);
	if (self->objectClass == ue::UStruct::StaticClass() ||
		self->objectClass == ue::UClass::StaticClass()) {
		spdlog::info(L"UStruct: {}", name.ToWstringView(), ar->dataOnly);
		spdlog::debug("- objectFlags: {:b}", int32_t(self->objectFlags));
		spdlog::debug("- objectIndex: {}", self->objectIndex);
		spdlog::debug("- autoRegister: {}", self->autoRegister);
		spdlog::debug("- objectClass: {:ip}", ue::UObjectFmt{ self->objectClass });
		spdlog::debug("- outer: {:ip}", ue::UObjectFmt{ self->outer });
		spdlog::debug("- parentType: {:ip}", ue::UObjectFmt{ self->parentType });
		spdlog::debug("- bytecode: {}", self->bytecode.Range());
	}
	// spdlog::debug("- ptr1_U: {}", self->ptr1_U);
	// spdlog::debug("- ptr2_U: {}", self->ptr2_U);
	// spdlog::debug("- ptr3_U: {}", self->ptr3_U);
	// spdlog::debug("- ptr4_U: {}", self->ptr4_U);
	// spdlog::debug("- ptr6_U: {}", self->ptr6_U);
	// spdlog::debug("- ptr7_U: {}", self->ptr7_U);
	// spdlog::debug("- int_U: {}", self->int_U);
}

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::Patch(abit::procs::UStruct::Serialize, &UStruct_Serialize, O_UStruct_Serialize);
}
