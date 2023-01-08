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
BytecodeTree::AppendDataFromVector(const std::vector<uint64_t>& source)
{
	DataIndex index = AppendData(1 + source.size());
	Data(index, 0) = static_cast<uint64_t>(source.size());
	for (size_t i = 0; i < source.size(); ++i) {
		Data(index, 1 + i) = source[i];
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

BytecodeTree::DataIndex
BytecodeTree::AppendWideString(std::wstring&& wideString)
{
	DataIndex index = wideStrings.size();
	wideStrings.push_back(std::move(wideString));
	return index;
}

void
BytecodeTree::SetFirstErrorIfNull(size_t ip)
{
	if (!firstError) {
		firstError = ip;
	}
}

bool
BytecodeTree::IsBytecodeBogusAt(size_t ip) const
{
	if (auto error = firstError) {
		return ip >= *error;
	} else {
		return false;
	}
}
