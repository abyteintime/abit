#include "yarnbox/patcher.hpp"

#include "abit/loader/ensure.hpp"
#include "abit/loader/logging.hpp"

#include "abit/ue/UObject/fmt.hpp"
#include "abit/ue/cast.hpp"

#include "yarnbox/bytecode/codegen.hpp"
#include "yarnbox/bytecode/disassembler.hpp"
#include "yarnbox/bytecode/dumper.hpp"

using namespace yarn;
using namespace ue;

using Node = BytecodeTree::Node;
using NodeIndex = BytecodeTree::NodeIndex;
using DataIndex = BytecodeTree::DataIndex;

namespace yarn {

struct ChunkWorkingCopy
{
	std::vector<uint8_t> bytecode;
	Chunk::Disassembly disassembly;
	Chunk::Analysis analysis;
};

struct InjectionState
{
	std::unordered_map<Chunk*, ChunkWorkingCopy> workingCopiesOfChunks;

	ChunkWorkingCopy* GetWorkingCopyOfChunk(
		Chunk* chunk,
		const Chunk::Disassembly& disasm,
		const Chunk::Analysis& analysis
	)
	{
		if (workingCopiesOfChunks.count(chunk) == 0) {
			ChunkWorkingCopy copy;
			new (&copy.bytecode) std::vector<uint8_t>(chunk->ustruct->bytecode);
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
	auto PushResult = [&query, &outResults, &count](BytecodeSpan span) {
		switch (query.pick) {
			using Pick = Injection::OpcodeQuery::Pick;
			case Pick::Span:
				// Don't alter the span in any way.
				break;
			case Pick::Start:
				span.end = span.start;
				break;
			case Pick::End:
				span.start = span.end;
				break;
		}
		outResults.push_back(span);
		count += 1;
	};

	if (const auto* specificOccurrences = std::get_if<std::vector<int32_t>>(&query.which)) {
		for (int32_t relativeIndex : *specificOccurrences) {
			if (std::optional<NodeIndex> nodeIndex = index.FindNode(query.opcode, relativeIndex)) {
				Node node = tree.nodes[*nodeIndex];
				PushResult(node.span);
			}
		}
	} else if (const auto* _ = std::get_if<Injection::OpcodeQuery::AllOccurrences>(&query.which)) {
		uint32_t occurrenceCount = index.Count(query.opcode);
		for (uint32_t i = 0; i < occurrenceCount; ++i) {
			std::optional<NodeIndex> nodeIndex = index.GetNode(query.opcode, i);
			Node node = tree.nodes[*nodeIndex];
			PushResult(node.span);
		}
	}

	return count;
}

static void
InvalidateNodeData(BytecodeTree& tree, NodeIndex nodeIndex)
{
	using namespace yarn::primitive;

	if (nodeIndex == BytecodeTree::invalidNodeIndex) {
		return;
	}

	Node& node = tree.nodes[nodeIndex];
	DataIndex data = node.data;
	const Rule& rule = encoding.Rule(node.opcode);
	for (size_t i = 0; i < rule.primsCount; ++i) {
		Primitive prim = rule.prims[i];
		switch (prim.type) {
			case PInsn: {
				tree.Data(data, i) = BytecodeTree::invalidNodeIndex;
				break;
			}
			case PInsns: {
				DataIndex insnVector = tree.Data(data, i);
				size_t count = tree.Data(insnVector, 0);
				for (size_t i = 0; i < count; ++i) {
					tree.Data(insnVector, 1 + i) = BytecodeTree::invalidNodeIndex;
				}
				break;
			}
			default:
				tree.Data(data, i) = 0;
		}
	}
}

static void
PrepareInjection(InjectionState& state, const Injection& injection, Registry& registry)
{
	Chunk* chunk = registry.GetChunkByPointer(injection.into);
	ABIT_ENSURE(chunk != nullptr, "target chunk {} is not patchable", UObjectFmt{ injection.into });
	UFunction* function = Cast<UFunction>(chunk->ustruct);
	ABIT_ENSURE(
		function != nullptr,
		"target chunk {} is not a Function; it is a {}",
		UObjectFmt{ injection.into },
		chunk->ustruct->objectClass->GetName().ToString()
	);

	auto [disasm, analysis] = chunk->GetOrPerformAnalysis();
	ABIT_ENSURE(
		disasm != nullptr,
		"disassembly of chunk {} failed. Please report this on ABiT's GitHub",
		UObjectFmt{ injection.into }
	);
	ABIT_ENSURE(analysis != nullptr, "analysis of chunk '{}' failed", UObjectFmt{ injection.into });

	ChunkWorkingCopy* copy = state.GetWorkingCopyOfChunk(chunk, *disasm, *analysis);

	ABIT_ENSURE(
		injection.select.size() > 0,
		"no queries specified in the `select` array, which makes the injection useless"
	);

	std::vector<BytecodeSpan> spans;
	for (size_t i = 0; i < injection.select.size(); ++i) {
		const Injection::OpcodeQuery& query = injection.select[i];
		uint32_t found
			= PerformOpcodeQuery(query, copy->disassembly.tree, copy->analysis.index, spans);
		ABIT_ENSURE(
			found > 0, "query #{} did not find any spans, which makes the query useless", i + 1
		);
	}

	std::vector<uint8_t> injectedBytecode;
	injection.generate->GenerateCode(injectedBytecode);

	for (size_t i = 0; i < spans.size(); ++i) {
		BytecodeSpan splice = spans[i];
		spdlog::trace("Splicing bytecode into {:03x}..{:03x}", splice.start, splice.end);
		// TODO: Implement checking for invalidated nodes. Splicing into invalidated nodes is not
		//  a very bright idea, and will in fact cause crashes or misbehavior. However in the
		//  current configuration it can happen if one mod replaces an instruction entirely, and
		//  then another tries to replace an operand of that instruction.
		if (splice.start != splice.end) {
			copy->bytecode.erase(
				copy->bytecode.begin() + splice.start, copy->bytecode.begin() + splice.end
			);
		}
		copy->bytecode.insert(
			copy->bytecode.begin() + splice.start, injectedBytecode.begin(), injectedBytecode.end()
		);

		// After the injection is applied we need to do exactly three things:
		// - Fix up collected spans in the `spans` vector
		// - Fix up the data spans in the bytecode tree
		// - Fix up jumps
		uint16_t removed = splice.end - splice.start;
		uint16_t added = injectedBytecode.size();

		// There's no point in fixing up _all_ spans since the spans we've already applied are...
		// well, already applied, so fixing them up will only incur unnecessary additional work.
		for (size_t j = i + 1; j < spans.size(); ++j) {
			BytecodeSpan& otherSpan = spans[j];
			// We have to perform this check since queries may not necessarily produce spans in
			// the order they appear in the bytecode.
			// Consider two queries: one looking for the last Return, and one looking for the
			// first Return. In a function with two Returns, if the queries are arranged exactly in
			// this order, we'll get the 2nd return before the 1st one in `spans`.
			// Maybe sorting would help with this, but I don't think there's really a big need to
			// do that since most injections are gonna inject maybe one or two opcodes.
			if (otherSpan.start > splice.start) {
				otherSpan.start = otherSpan.start - removed + added;
				otherSpan.end = otherSpan.end - removed + added;
			}
		}

		// Once our injection is made we also need to fix up the spans inside the tree.
		// Unfortunately there aren't any tricks we can do here to skip directly to spans after
		// the current one (at least not yet.) Most functions don't have thousands of nodes, and
		// even if they had, this is just some simple math we're doing here. It's plenty fast.
		for (size_t nodeIndex = 0; nodeIndex < copy->disassembly.tree.nodes.size(); ++nodeIndex) {
			Node& node = copy->disassembly.tree.nodes[nodeIndex];
			if (node.span.start == splice.start && node.span.end == splice.end) {
				// This case is a little bit special: We want to replace the underlying span with
				// the replacement's span, and replace all the data with invalid stuff, since all of
				// it no longer exists after we replaced it.
				InvalidateNodeData(copy->disassembly.tree, nodeIndex);
				node.span.end = splice.start + added;
			} else if (node.span.start > splice.start && node.span.end > splice.end) {
				node.span.start = node.span.start - removed + added;
				node.span.end = node.span.end - removed + added;
			}
		}
		for (BytecodeSpan& otherSpan : copy->disassembly.tree.dataSpans) {
			if (otherSpan.start > splice.start && otherSpan.end > splice.end) {
				otherSpan.start = otherSpan.start - removed + added;
				otherSpan.end = otherSpan.end - removed + added;
			}
		}

		// Last but not least, now that our nodes point to the correct spans, fix up jumps.
		for (auto [nodeIndex, dataOffset] : copy->analysis.jumps.offsets) {
			Node node = copy->disassembly.tree.nodes[nodeIndex];
			const Rule& rule = encoding.Rule(node.opcode);
			for (size_t i = 0; i < rule.primsCount; ++i) {
				using namespace yarn::primitive;
				BytecodeSpan jumpSpan = copy->disassembly.tree.DataSpan(node.data, i);
				uint64_t& jumpInData = copy->disassembly.tree.Data(node.data, i);
				uint16_t& jumpInBytecode
					= reinterpret_cast<uint16_t&>(copy->bytecode[jumpSpan.start]);
				Primitive prim = rule.prims[i];
				if (prim == POAbs && jumpInBytecode > splice.start) {
					jumpInData = jumpInData - removed + added;
					jumpInBytecode = jumpInBytecode - removed + added;
				}
				if (IsOffsetRel(static_cast<IntKind>(prim.arg))) {
					IntKind orel = static_cast<IntKind>(prim.arg);
					uint8_t jumpReference = orel - IntKind::KOffsetRel0;
					if (splice.start >= jumpSpan.start + jumpReference
						&& splice.start < jumpSpan.start + jumpReference + jumpInBytecode) {
						jumpInData = jumpInData - removed + added;
						jumpInBytecode = jumpInBytecode - removed + added;
					}
				}
			}
		}
	}

	ABIT_ENSURE(
		!Disassembler::IsBytecodeTooLarge(copy->bytecode.size()),
		"generated bytecode is too large ({} bytes exceeds the maximum size of a chunk {})",
		copy->bytecode.size(),
		std::numeric_limits<uint16_t>::max()
	);

	// TODO: This check doesn't catch nearly as many instances of bad bytecode since in a lot of
	//  cases the parser can recover and get back on track. As such, we should also perform checks
	//  for bogus pointers here.
	BytecodeTree validationTree;
	NodeIndex rootNode;
	bool treeIsValid = Disassemble(
		&copy->bytecode[0], static_cast<uint16_t>(copy->bytecode.size()), validationTree, rootNode
	);
	ABIT_ENSURE(
		treeIsValid,
		"bytecode is invalid structurally after the injection. Make sure you're injecting into the "
		"correct spots. Disassembly:\n{}",
		[&validationTree, rootNode] {
			std::string disassembly;
			DumpNode(validationTree, rootNode, disassembly);
			return disassembly;
		}()
	);

	if (spdlog::should_log(spdlog::level::trace)) {
		std::string disassembly;
		DumpNode(copy->disassembly.tree, copy->disassembly.rootNode, disassembly);
		spdlog::trace("Frankensteined bytecode tree after applying injection:\n{}", disassembly);

		disassembly.clear();
		DumpNode(validationTree, rootNode, disassembly);
		spdlog::trace("Disassembly after applying injection:\n{}", disassembly);
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
			spdlog::trace("Preparing injection #{} into {}", i, UObjectFmt{ injection.into });
			PrepareInjection(state, injection, registry);
		} catch (abit::Error e) {
			spdlog::error(
				"Patch '{}': error in injection #{}: {}", patch.comment, i + 1, e.message
			);
			allSucceeded = false;
		}
	}
	if (!allSucceeded) {
		throw abit::Error{ "failed to apply some injections. See errors above for context" };
	}

	spdlog::trace("All injections succeeded. Applying them to chunks");
	for (auto& [chunk, copy] : state.workingCopiesOfChunks) {
		spdlog::trace("  - applying modified chunk '{}'", chunk->pathName);
		chunk->ustruct->bytecode.Clear();
		chunk->ustruct->bytecode.ExtendByCopying(copy.bytecode);
		chunk->disassembly = std::move(copy.disassembly);
		chunk->analysis = std::move(copy.analysis);
	}
}

void
yarn::ApplyPatch(const Patch& patch, Registry& registry)
{
	spdlog::debug("Applying patch '{}'", patch.comment);

	try {
		if (const auto* injection = std::get_if<Patch::Injection>(&patch.data)) {
			ApplyInjection(patch, *injection, registry);
		}
	} catch (abit::Error e) {
		spdlog::error("Cannot apply patch '{}': {}", patch.comment, e.message);
	}
}
