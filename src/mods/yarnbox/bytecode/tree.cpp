#include "yarnbox/bytecode/tree.hpp"

using namespace yarn;

BytecodeTree::BytecodeTree(const uint8_t* bytecode, size_t length) {}

BytecodeTree::NodeIndex
BytecodeTree::AppendNode(Node node)
{
	NodeIndex index = nodes.size();
	nodes.push_back(node);
	return index;
}
