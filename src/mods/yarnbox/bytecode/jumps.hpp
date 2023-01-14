#pragma once

#include <cstdint>

#include "yarnbox/bytecode/tree.hpp"

namespace yarn {

struct Jumps
{
	struct Offset
	{
		BytecodeTree::DataIndex dataIndex;
	};

	std::vector<Offset> offsets;

	void Analyze(const BytecodeTree& tree);
};

}
