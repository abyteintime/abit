#include "abit/dll_macros.hpp"

#include <memory>
#include <unordered_map>
#include <utility>

#include "fmt/ranges.h"

#include "abit/error.hpp"
#include "abit/loader/logging.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/FEngineLoop.hpp"
#include "abit/procs/UObject.hpp"
#include "abit/procs/UStruct.hpp"

#include "yarnbox/config/config.hpp"
#include "yarnbox/patcher.hpp"
#include "yarnbox/registry.hpp"

#include "yarnbox/bytecode/disassembler.hpp"
#include "yarnbox/bytecode/dumper.hpp"
#include "yarnbox/bytecode/opcode.hpp"

#include "yarnbox/vm/tracing.hpp"

#include "abit/ue/AGameMod.hpp"
#include "abit/ue/FArchive.hpp"
#include "abit/ue/FFrame.hpp"
#include "abit/ue/FString.hpp"
#include "abit/ue/UClass.hpp"
#include "abit/ue/UFunction.hpp"
#include "abit/ue/UObject.hpp"
#include "abit/ue/UObject/fmt.hpp"
#include "abit/ue/UStruct.hpp"

#include "abit/procs/FMaterialResource.hpp"
#include "abit/procs/FName.hpp"
#include "abit/procs/global.hpp"

using namespace yarn;
using namespace ue;

static Registry registry;

static size_t attemptedDisassemblies = 0;
static size_t functionsSuccessfullyDisassembled = 0;
static Disassembler::Stats disassemblerStats;

static void (*O_UStruct_Serialize)(UStruct*, FArchive*);
static void
UStruct_Serialize(UStruct* self, FArchive* ar)
{
	O_UStruct_Serialize(self, ar);

	spdlog::trace("Chunk: {}", UObjectFmt{ self });
	spdlog::trace("  - outer: {:ip}", UObjectFmt{ self->outer });

	if (self->objectClass == UState::StaticClass()) {
		spdlog::debug(
			L"Ignoring state '{}' because modifying states is unsupported",
			self->GetName().ToWstringView()
		); // TODO?
		return;
	}

	auto chunk = std::make_shared<Chunk>(self);
	if (self->bytecode.length > 0) {
		spdlog::trace("  - bytecode: ({} bytes) {}", self->bytecode.length, self->bytecode.Range());

		if (Disassembler::IsBytecodeTooLarge(self->bytecode.length)) {
			spdlog::error(
				"Bytecode in chunk {} is too large (exceeds 16-bit unsigned integer limit; length "
				"is {})",
				UObjectFmt{ self },
				self->bytecode.length
			);
			return;
		}

		BytecodeTree tree;
		Disassembler disassembler{ self->bytecode.dataPtr,
								   static_cast<uint16_t>(self->bytecode.length),
								   tree };
		std::string stringDump;
		disassembler.EnableStatCollection(disassemblerStats);
		bool success = true;
		while (!disassembler.AtEnd()) {
			BytecodeTree::NodeIndex nodeIndex = disassembler.Disassemble();
			DumpNode(tree, nodeIndex, stringDump);
			if (disassembler.ShouldStopDisassembling()) {
				success = false;
				break;
			}
		}
		attemptedDisassemblies += 1;
		functionsSuccessfullyDisassembled += success;
		spdlog::trace("Disassembled bytecode into {} nodes", tree.nodes.size());
		spdlog::trace("Disassembly:\n{}", stringDump);
	}
	registry.RegisterChunk(std::move(chunk));
}

static void
PrintDisassemblyStats()
{
	spdlog::info(
		"Disassembly stats: attempted {}, successful {}. Success rate is {:.2f}%",
		attemptedDisassemblies,
		functionsSuccessfullyDisassembled,
		float(functionsSuccessfullyDisassembled) / float(attemptedDisassemblies) * 100.f
	);

	Disassembler::Stats::Summary summary = disassemblerStats.ComputeSummary();

	spdlog::debug("===== Failure summary =====");
	spdlog::debug("Out of bounds reads: {}", disassemblerStats.outOfBoundsReads);
	spdlog::debug(
		"Total occurrences of unknown opcodes: {}", summary.totalOccurrencesOfUnknownOpcodes
	);
	spdlog::debug("  Counts per opcode");
	for (auto [opcode, count] : summary.occurrencesOfUnknownOpcodes) {
		spdlog::debug(
			"  {:4} | {:30}  {:6} times ({:5.1f}%)",
			static_cast<uint32_t>(opcode),
			OpcodeToString(opcode),
			count,
			float(count) / float(summary.totalOccurrencesOfUnknownOpcodes) * 100.f
		);
	}
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
	PrintDisassemblyStats();

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
				ModConfig config = ModConfig::Load(*configPath, registry);
				ApplyPatches(config.patches);
			} catch (abit::Error e) {
				spdlog::error("Applying patches failed: {}", e.message);
			}
		}
	}
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
	EnableNativeTracingInThisScope enableTracing{ true };
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

static void (*O_FEngineLoop_Init)(class FEngineLoop*);
static void
FEngineLoop_Init(class FEngineLoop* self)
{
	SetupNativeTracing();
	// SetNativeTracingEnabled(true);
	O_FEngineLoop_Init(self);
	PostInit();
}

class UMaterial : UObject
{};

struct FMaterialResource
{
	uint8_t __padding__[196];
	uint32_t blendMode;
};

static void (*O_FMaterialResource_FMaterialResource)(FMaterialResource*, UMaterial*);
static void
FMaterialResource_FMaterialResource(FMaterialResource* self, UMaterial* mat)
{
	O_FMaterialResource_FMaterialResource(self, mat);
	if (self->blendMode == 1) {
		self->blendMode = 0;
	}
}

static void (*O_FMaterialResource_Serialize)(FMaterialResource*, FArchive*);
static void
FMaterialResource_Serialize(FMaterialResource* self, FArchive* ar)
{
	O_FMaterialResource_Serialize(self, ar);
	if (self->blendMode == 1) {
		self->blendMode = 0;
	}
}

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::Patch(abit::procs::UStruct::Serialize, &UStruct_Serialize, O_UStruct_Serialize);
	abit::Patch(abit::procs::FEngineLoop::Init, &FEngineLoop_Init, O_FEngineLoop_Init);
	abit::Patch(abit::procs::UObject::CallFunction, &UObject_CallFunction, O_UObject_CallFunction);

	abit::Patch(
		abit::procs::FMaterialResource::FMaterialResource,
		&FMaterialResource_FMaterialResource,
		O_FMaterialResource_FMaterialResource
	);
	abit::Patch(
		abit::procs::FMaterialResource::Serialize,
		&FMaterialResource_Serialize,
		O_FMaterialResource_Serialize
	);
}
