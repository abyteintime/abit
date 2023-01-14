#include "yarnbox/patcher.hpp"

#include "abit/loader/ensure.hpp"
#include "abit/loader/logging.hpp"

#include "abit/ue/UObject/fmt.hpp"
#include "abit/ue/cast.hpp"

using namespace yarn;
using namespace ue;

using Node = BytecodeTree::Node;
using NodeIndex = BytecodeTree::NodeIndex;
using DataIndex = BytecodeTree::DataIndex;

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

	auto* functionToReplace
		= Cast<UFunction>(parentClass->functions.at(replacementChunk->name)->ustruct);
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

namespace yarn {

struct ChunkWorkingCopy
{
	TArray<uint8_t> bytecode;
	Chunk::Disassembly disassembly;
	Chunk::Analysis analysis;
};

struct InjectionState
{
	std::unordered_map<const Chunk*, ChunkWorkingCopy> workingCopiesOfChunks;

	ChunkWorkingCopy* GetWorkingCopyOfChunk(
		const Chunk* chunk,
		const Chunk::Disassembly& disasm,
		const Chunk::Analysis& analysis
	)
	{
		if (workingCopiesOfChunks.count(chunk) == 0) {
			ChunkWorkingCopy copy;
			new (&copy.bytecode) TArray<uint8_t>(chunk->ustruct->bytecode);
			new (&copy.disassembly) auto(disasm);
			new (&copy.analysis) auto(analysis);
			workingCopiesOfChunks.emplace(std::make_pair(chunk, std::move(copy)));
		}
		return &workingCopiesOfChunks[chunk];
	}
};

}

static uint32_t
PerformOpcodeQuery(
	const Injection::OpcodeQuery& query,
	const BytecodeTree& tree,
	const Index& index,
	std::vector<BytecodeSpan>& outResults
)
{
	uint32_t count = 0;

	if (const auto* specificOccurrences = std::get_if<std::vector<int32_t>>(&query.which)) {
		for (int32_t relativeIndex : *specificOccurrences) {
			if (std::optional<NodeIndex> nodeIndex = index.FindNode(query.opcode, relativeIndex)) {
				Node node = tree.nodes[*nodeIndex];
				outResults.push_back(node.span);
				count += 1;
			}
		}
	} else if (const auto* _ = std::get_if<Injection::OpcodeQuery::AllOccurrences>(&query.which)) {
		uint32_t occurrenceCount = index.Count(query.opcode);
		for (uint32_t i = 0; i < occurrenceCount; ++i) {
			std::optional<NodeIndex> nodeIndex = index.GetNode(query.opcode, i);
			Node node = tree.nodes[*nodeIndex];
			outResults.push_back(node.span);
			count += 1;
		}
	}

	return count;
}

static void
PrepareInjection(InjectionState& state, const Injection& injection, Registry& registry)
{
	Chunk* chunk = registry.GetChunkByName(injection.into);
	UFunction* function = Cast<UFunction>(chunk->ustruct);
	ABIT_ENSURE(
		function != nullptr,
		"target chunk '{}' is not a UFunction; it is a {}",
		injection.into,
		chunk->ustruct->objectClass->GetName().ToString()
	);

	auto [disasm, analysis] = chunk->GetOrPerformAnalysis();
	ABIT_ENSURE(
		disasm != nullptr,
		"disassembly of chunk '{}' failed. Please report this on ABiT's GitHub",
		injection.into
	);
	ABIT_ENSURE(analysis != nullptr, "analysis of chunk '{}' failed", injection.into);

	ChunkWorkingCopy* copy = state.GetWorkingCopyOfChunk(chunk, *disasm, *analysis);

	ABIT_ENSURE(
		injection.select.size() > 0,
		"no queries specified in the `select` array, which makes the injection useless"
	);

	std::vector<BytecodeSpan> spans;
	for (size_t i = 0; i < injection.select.size(); ++i) {
		const Injection::Query& query = injection.select[i];
		if (const auto* opcodeQuery = std::get_if<Injection::OpcodeQuery>(&query)) {
			uint32_t found = PerformOpcodeQuery(
				*opcodeQuery, copy->disassembly.tree, copy->analysis.index, spans
			);
			ABIT_ENSURE(
				found > 0, "query #{} did not find any spans, which makes the query useless", i + 1
			);
		}
	}

	std::vector<uint8_t> injectedBytecode;
	if (const auto* call = std::get_if<Injection::StaticFinalFunctionCall>(&injection.place)) {
	}
}

static void
ApplyInjection(const Patch& patch, const Patch::Injection& injections, Registry& registry)
{
	InjectionState state;
	bool allSucceeded = true;
	for (size_t i = 0; i < injections.inject.size(); ++i) {
		const Injection& injection = injections.inject[i];
		try {
			PrepareInjection(state, injection, registry);
		} catch (abit::Error e) {
			spdlog::error("Patch '{}': error in injection #{}: {}", patch.comment, i + 1, e.what);
			allSucceeded = false;
		}
	}
	if (!allSucceeded) {
		throw abit::Error{ "failed to apply some injections. See errors above for context" };
	}
}

void
yarn::ApplyPatch(const Patch& patch, Registry& registry)
{
	spdlog::debug("Applying patch '{}'", patch.comment);

	try {
		if (const auto* replacement = std::get_if<Patch::Replacement>(&patch.data)) {
			ApplyReplacement(*replacement, registry);
		} else if (const auto* injection = std::get_if<Patch::Injection>(&patch.data)) {
			ApplyInjection(patch, *injection, registry);
		}
	} catch (abit::Error e) {
		spdlog::error("Cannot apply patch '{}': {}", patch.comment, e.what);
	}
}
