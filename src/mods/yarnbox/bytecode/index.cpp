#include "yarnbox/bytecode/index.hpp"

using namespace yarn;

void
Index::Analyze(const BytecodeTree& tree)
{
	nodeSearchMap.clear();
	counts.clear();

	for (BytecodeTree::NodeIndex nodeIndex = 0; nodeIndex < tree.nodes.size(); ++nodeIndex) {
		BytecodeTree::Node node = tree.nodes[nodeIndex];
		uint32_t occurrenceIndex = GetNextIndexForOpcode(node.opcode);
		nodeSearchMap[{ node.opcode, occurrenceIndex }] = nodeIndex;
	}
}

std::optional<BytecodeTree::NodeIndex>
Index::FindNode(Opcode opcode, int32_t relativeIndex) const
{
	if (counts.count(opcode) == 0) {
		return std::nullopt;
	}

	uint32_t front = static_cast<uint32_t>(relativeIndex);
	uint32_t back = counts.at(opcode) + relativeIndex;
	uint32_t occurrenceIndex = relativeIndex >= 0 ? front : back;

	return GetNode(opcode, occurrenceIndex);
}

std::optional<BytecodeTree::NodeIndex>
Index::GetNode(Opcode opcode, uint32_t occurrenceIndex) const
{
	if (nodeSearchMap.count({ opcode, occurrenceIndex }) > 0) {
		return nodeSearchMap.at({ opcode, occurrenceIndex });
	} else {
		return std::nullopt;
	}
}

uint32_t
Index::Count(Opcode opcode) const
{
	return counts.count(opcode) > 0 ? counts.at(opcode) : 0;
}

uint32_t
Index::GetNextIndexForOpcode(Opcode opcode)
{
	if (counts.count(opcode) == 0) {
		counts[opcode] = 0;
	}
	return counts[opcode]++;
}
