#include "abit/dll_macros.hpp"

#include <memory>
#include <unordered_map>

#include "fmt/ranges.h"

#include "abit/loader/logging.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/AGameMod.hpp"
#include "abit/procs/FArchive.hpp"
#include "abit/procs/FEngineLoop.hpp"
#include "abit/procs/UObject.hpp"
#include "abit/procs/UStruct.hpp"

#include "yarnbox/config.hpp"

#include "yarnbox/bytecode/disassembler.hpp"
#include "yarnbox/bytecode/opcode.hpp"

#include "yarnbox/ue/AGameMod.hpp"
#include "yarnbox/ue/FArchive.hpp"
#include "yarnbox/ue/FString.hpp"
#include "yarnbox/ue/UClass.hpp"
#include "yarnbox/ue/UFunction.hpp"
#include "yarnbox/ue/UObject.hpp"
#include "yarnbox/ue/UObject/fmt.hpp"
#include "yarnbox/ue/UStruct.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace yarn;
using namespace ue;

static size_t attemptedDisassemblies = 0;
static size_t functionsSuccessfullyDisassembled = 0;

static void (*O_UStruct_Serialize)(UStruct*, FArchive*);
static void
UStruct_Serialize(UStruct* self, FArchive* ar)
{

	O_UStruct_Serialize(self, ar);

	if (self->objectClass == UFunction::StaticClass()) {
		FString name = self->GetName();

		spdlog::debug(L"UFunction: {}", name.ToWstringView());
		spdlog::debug("- objectFlags: {:b}", int32_t(self->objectFlags));
		spdlog::debug("- objectIndex: {}", self->objectIndex);
		spdlog::debug("- objectClass: {:ip}", UObjectFmt{ self->objectClass });
		spdlog::debug("- outer: {:ip}", UObjectFmt{ self->outer });
		spdlog::debug("- bytecode: {}", self->bytecode.Range());

		BytecodeTree tree;
		Disassembler disassembler{ self->bytecode.dataPtr, size_t(self->bytecode.length), tree };
		bool success = true;
		while (!disassembler.AtEnd()) {
			auto nodeIndex = disassembler.Disassemble();
			if (Disassembler::StopDisassemblingAfterOpcode(tree.nodes[nodeIndex].opcode)) {
				success = false;
				break;
			}
		}
		attemptedDisassemblies += 1;
		functionsSuccessfullyDisassembled += success;
		spdlog::debug("Disassembled bytecode into {} nodes", tree.nodes.size());
	}
}

static void
PostInit()
{
	spdlog::info(
		"Disassembly stats: attempted {}, successful {}. Success rate is {:.2}%",
		attemptedDisassemblies,
		functionsSuccessfullyDisassembled,
		float(functionsSuccessfullyDisassembled) / float(attemptedDisassemblies) * 100.f
	);

	spdlog::info("Yarnbox PostInit: we'll now try to load and apply patch lists from mods.");
	TArray<FString> localModPackages;
	TArray<FString> localModPaths;
	AGameMod::GetLocalModPackagesAndPaths(localModPackages, localModPaths);
	for (size_t i = 0; i < localModPackages.length; ++i) {
		const FString& modPackage = localModPackages[i];
		const FString& modPath = localModPaths[i];
		if (auto configPath = ModConfig::PathInModDirectory(modPath.ToWstringView())) {
			spdlog::info(L"Loading Yarnbox.Patches.json for {}", modPackage.ToWstringView());
			ModConfig config = ModConfig::Load(*configPath);
		}
	}
}

static void (*O_FEngineLoop_Init)(class FEngineLoop*);
static void
FEngineLoop_Init(class FEngineLoop* self)
{
	O_FEngineLoop_Init(self);

	PostInit();
}

static uint32_t (*O_AGameMod_GetGameModInfoFromFile
)(FString&, FString&, struct FGameModInfo*, uint32_t);
static uint32_t
AGameMod_GetGameModInfoFromFile(
	FString& path,
	FString& folderName,
	struct FGameModInfo* c,
	uint32_t d
)
{
	spdlog::info(
		L"GetGameModInfoFromFile path={} folderName={}",
		path.ToWstringView(),
		folderName.ToWstringView()
	);
	return O_AGameMod_GetGameModInfoFromFile(path, folderName, c, d);
}

static uint32_t (*O_AGameMod_UpdateLocalModInstalls)(uint32_t a);
static uint32_t
AGameMod_UpdateLocalModInstalls(uint32_t a)
{
	uint32_t result = O_AGameMod_UpdateLocalModInstalls(a);
	spdlog::info("Hat updated its list of mod installs");
	return result;
}

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::Patch(abit::procs::UStruct::Serialize, &UStruct_Serialize, O_UStruct_Serialize);
	abit::Patch(abit::procs::FEngineLoop::Init, &FEngineLoop_Init, O_FEngineLoop_Init);
	abit::Patch(
		abit::procs::AGameMod::GetGameModInfoFromFile,
		&AGameMod_GetGameModInfoFromFile,
		O_AGameMod_GetGameModInfoFromFile
	);
	abit::Patch(
		abit::procs::AGameMod::UpdateLocalModInstalls,
		&AGameMod_UpdateLocalModInstalls,
		O_AGameMod_UpdateLocalModInstalls
	);
}
