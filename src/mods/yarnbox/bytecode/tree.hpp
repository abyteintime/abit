#pragma once

#include "yarnbox/bytecode/opcode.hpp"

#include <vector>

namespace yarn {

struct BytecodeTree
{
	using NodeIndex = uint32_t;

	struct Node
	{
		const size_t ip;
		Opcode opcode;
	};

	std::vector<Node> nodes;

	BytecodeTree(const uint8_t* bytecode, size_t length);

	NodeIndex AppendNode(Node node);
};

}
