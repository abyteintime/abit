#include "abit/dll_macros.hpp"

#include <memory>
#include <unordered_map>

#include "fmt/ranges.h"

#include "abit/loader/logging.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/FArchive.hpp"
#include "abit/procs/FEngineLoop.hpp"
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

static size_t attemptedDisassemblies = 0;
static size_t functionsSuccessfullyDisassembled = 0;

static void (*O_UStruct_Serialize)(ue::UStruct*, ue::FArchive*);
static void
UStruct_Serialize(ue::UStruct* self, ue::FArchive* ar)
{

	O_UStruct_Serialize(self, ar);

	if (self->objectClass == ue::UFunction::StaticClass()) {
		ue::FString name = self->GetName();

		spdlog::info(L"UFunction: {}", name.ToWstringView());
		spdlog::debug("- objectFlags: {:b}", int32_t(self->objectFlags));
		spdlog::debug("- objectIndex: {}", self->objectIndex);
		spdlog::debug("- objectClass: {:ip}", ue::UObjectFmt{ self->objectClass });
		spdlog::debug("- outer: {:ip}", ue::UObjectFmt{ self->outer });
		spdlog::debug("- bytecode: {}", self->bytecode.Range());

		yarn::BytecodeTree tree;
		yarn::Disassembler disassembler{ self->bytecode.dataPtr,
										 size_t(self->bytecode.length),
										 tree };
		bool success = true;
		while (!disassembler.AtEnd()) {
			auto nodeIndex = disassembler.Disassemble();
			if (yarn::Disassembler::StopDisassemblingAfterOpcode(tree.nodes[nodeIndex].opcode)) {
				success = false;
				break;
			}
		}
		attemptedDisassemblies += 1;
		functionsSuccessfullyDisassembled += success;
		spdlog::debug("Disassembled bytecode into {} nodes", tree.nodes.size());
	}
}

static void (*O_FEngineLoop_Init)(class FEngineLoop*);
static void
FEngineLoop_Init(class FEngineLoop* self)
{
	O_FEngineLoop_Init(self);
	spdlog::info(
		"Disassembly stats: attempted {}, successful {}. Success rate is {:.2}%",
		attemptedDisassemblies,
		functionsSuccessfullyDisassembled,
		float(functionsSuccessfullyDisassembled) / float(attemptedDisassemblies) * 100.f
	);
}

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::Patch(abit::procs::UStruct::Serialize, &UStruct_Serialize, O_UStruct_Serialize);
	abit::Patch(abit::procs::FEngineLoop::Init, &FEngineLoop_Init, O_FEngineLoop_Init);
}
