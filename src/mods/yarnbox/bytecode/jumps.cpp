#include "yarnbox/bytecode/jumps.hpp"

#include "yarnbox/bytecode/encoding.hpp"

using namespace yarn;

using NodeIndex = BytecodeTree::NodeIndex;
using DataIndex = BytecodeTree::DataIndex;

void
Jumps::Analyze(const BytecodeTree& tree, NodeIndex rootNode)
{
	using namespace yarn::primitive;

	spans.clear();

	for (BytecodeTree::Node node : tree.nodes) {
		const Rule& rule = encoding.Rule(node.opcode);
		DataIndex data = node.data;
		for (size_t i = 0; i < rule.primsCount; ++i) {
			Primitive prim = rule.prims[i];
			if (prim.type == PU16) {
				auto [ip, _] = tree.DataSpan(data, i);
				switch (prim.arg) {
					case KOffsetAbs:
						spans.push_back({ JumpSpan::Type::Absolute, 0, ip });
						break;
					case KOffsetRel:
						spans.push_back({ JumpSpan::Type::Relative, 0, ip });
						break;
					default:
						break;
				}
			}
		}
	}
}
