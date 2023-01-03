#include "abit/dll_macros.hpp"

#include <memory>
#include <unordered_map>

#include "fmt/ranges.h"

#include "abit/error.hpp"
#include "abit/loader/logging.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/FEngineLoop.hpp"
#include "abit/procs/UFunction.hpp"
#include "abit/procs/UObject.hpp"

#include "yarnbox/config.hpp"
#include "yarnbox/patcher.hpp"
#include "yarnbox/registry.hpp"

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

using namespace yarn;
using namespace ue;

static Registry registry;

static size_t attemptedDisassemblies = 0;
static size_t functionsSuccessfullyDisassembled = 0;

static void (*O_UFunction_Serialize)(UFunction*, FArchive*);
static void
UFunction_Serialize(UFunction* self, FArchive* ar)
{
	O_UFunction_Serialize(self, ar);

	FString name = self->GetName();

	spdlog::debug(L"Function: {}", name.ToWstringView());
	spdlog::debug("  - outer: {:ip}", UObjectFmt{ self->outer });
	spdlog::debug("  - bytecode: {}", self->bytecode.Range());

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

	Function function;
	function.unreal = reinterpret_cast<UFunction*>(self);
	registry.RegisterFunction(std::move(function));
}

static void
ApplyPatches(const std::vector<Patch>& patches)
{
	for (const Patch& patch : patches) {
		ApplyPatch(patch, registry);
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
			spdlog::info(L"Applying patches for {}", modPackage.ToWstringView());
			try {
				ModConfig config = ModConfig::Load(*configPath);
				ApplyPatches(config.patches);
			} catch (abit::Error e) {
				spdlog::error("Applying patches failed: {}", e.what);
			}
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

static thread_local size_t callDepth = 0;

static void (*O_UObject_CallFunction)(UObject*, struct FFrame&, void* const, UFunction*);
static void
UObject_CallFunction(
	UObject* self,
	struct FFrame& frame,
	void* const returnPointer,
	UFunction* function
)
{
	callDepth += 1;
	if (callDepth > 100) {
		spdlog::warn(
			"Very deep recursion in UObject::CallFunction, while calling {} on {}",
			UObjectFmt{ function },
			UObjectFmt{ self }
		);
	}
	O_UObject_CallFunction(self, frame, returnPointer, function);
	callDepth -= 1;
}

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::Patch(abit::procs::UFunction::Serialize, &UFunction_Serialize, O_UFunction_Serialize);
	abit::Patch(abit::procs::FEngineLoop::Init, &FEngineLoop_Init, O_FEngineLoop_Init);
	abit::Patch(abit::procs::UObject::CallFunction, &UObject_CallFunction, O_UObject_CallFunction);
}
