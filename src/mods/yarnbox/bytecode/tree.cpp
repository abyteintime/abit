#include "yarnbox/bytecode/tree.hpp"

using namespace yarn;

BytecodeTree::NodeIndex
BytecodeTree::AppendNode(Node node)
{
	NodeIndex index = nodes.size();
	nodes.push_back(node);
	return index;
}
