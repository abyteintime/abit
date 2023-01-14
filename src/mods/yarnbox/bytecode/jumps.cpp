#include "yarnbox/bytecode/jumps.hpp"

#include "yarnbox/bytecode/encoding.hpp"

using namespace yarn;

using NodeIndex = BytecodeTree::NodeIndex;
using DataIndex = BytecodeTree::DataIndex;

void
Jumps::Analyze(const BytecodeTree& tree)
{
	using namespace yarn::primitive;

	offsets.clear();

	for (NodeIndex nodeIndex = 0; nodeIndex < tree.nodes.size(); ++nodeIndex) {
		BytecodeTree::Node node = tree.nodes[nodeIndex];
		const Rule& rule = encoding.Rule(node.opcode);
		DataIndex data = node.data;
		for (size_t i = 0; i < rule.primsCount; ++i) {
			Primitive prim = rule.prims[i];
			if (prim.type == PU16
				&& (prim.arg == KOffsetAbs || IsOffsetRel(static_cast<IntKind>(prim.arg)))) {
				offsets.push_back({ data + i });
			}
		}
	}
}
