#include "yarnbox/bytecode/tree.hpp"

using namespace yarn;

BytecodeTree::NodeIndex
BytecodeTree::AppendNode(Node node)
{
	NodeIndex index = nodes.size();
	nodes.push_back(node);
	return index;
}

BytecodeTree::DataIndex
BytecodeTree::AppendData(size_t count)
{
	DataIndex index = data.size();
	for (size_t i = 0; i < count; ++i) {
		data.push_back(0);
	}
	return index;
}

BytecodeTree::DataIndex
BytecodeTree::AppendString(std::string&& string)
{
	DataIndex index = strings.size();
	strings.push_back(std::move(string));
	return index;
}
