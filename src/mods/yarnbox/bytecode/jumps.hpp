#pragma once

#include <cstdint>

#include "yarnbox/bytecode/tree.hpp"

namespace yarn {

struct Jumps
{
	struct Offset
	{
		BytecodeTree::NodeIndex nodeIndex;
		uint32_t dataOffset;
	};

	std::vector<Offset> offsets;

	void Analyze(const BytecodeTree& tree);
};

}
