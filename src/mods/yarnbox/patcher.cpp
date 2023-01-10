#include "yarnbox/patcher.hpp"

#include "abit/loader/ensure.hpp"
#include "abit/loader/logging.hpp"

#include "yarnbox/ue/UObject/fmt.hpp"
#include "yarnbox/ue/cast.hpp"

using namespace yarn;
using namespace ue;

static void
ApplyReplacement(const Patch::Replacement& replacement, const Registry& registry)
{
	const Chunk* replacementChunk = registry.GetChunkByName(replacement.chunk);
	ABIT_ENSURE(replacementChunk != nullptr, "chunk '{}' does not exist", replacement.chunk);
	ABIT_ENSURE(
		replacementChunk->ustruct->bytecode.length > 0, "chunk '{}' does not contain any bytecode"
	);
	UStruct* replacementSourceClass = Cast<UStruct>(replacementChunk->ustruct->outer);

	UStruct* parentType = replacementSourceClass->parentType;
	ABIT_ENSURE(
		parentType->objectClass == UClass::StaticClass(),
		"{}, the parent type of {}, is not a UClass",
		UObjectFmt{ parentType },
		UObjectFmt{ replacementChunk->ustruct }
	);

	const Chunk* parentClass = registry.GetChunkByPointer(parentType);
	ABIT_ENSURE(
		parentClass->functions.count(replacementChunk->name) > 0,
		"function '{}' does not exist within the parent class '{}' of replacement class '{}'.\n"
		"Available functions are:{}",
		replacementChunk->name,
		parentClass->name,
		replacementSourceClass->GetName().ToString(),
		[parentClass] {
			std::string text;
			for (const auto& [key, value] : parentClass->functions) {
				text += fmt::format("\n  - {}", key);
			}
			return text;
		}()
	);

	auto* functionToReplace =
		Cast<UFunction>(parentClass->functions.at(replacementChunk->name)->ustruct);
	auto* replacementFunction = Cast<UFunction>(replacementChunk->ustruct);
	ABIT_ENSURE(
		replacementFunction != nullptr,
		"chunk {} is not a UFunction",
		UObjectFmt{ replacementChunk->ustruct }
	);

	functionToReplace->bytecode.Clear();
	functionToReplace->bytecode.ExtendByCopying(replacementFunction->bytecode);

	spdlog::debug("Replacement patch '{}' was applied successfully", replacement.chunk);
}

void
yarn::ApplyPatch(const Patch& patch, const Registry& registry)
{
	spdlog::debug("Applying patch '{}'", patch.comment);

	try {
		if (const auto* replacement = std::get_if<Patch::Replacement>(&patch.data)) {
			ApplyReplacement(*replacement, registry);
		} else if (const auto* injection = std::get_if<Patch::Injection>(&patch.data)) {
			spdlog::error("Cannot apply injection patch: injections are not yet implemented");
		}
	} catch (abit::Error e) {
		spdlog::error("Cannot apply patch '{}': {}", patch.comment, e.what);
	}
}
