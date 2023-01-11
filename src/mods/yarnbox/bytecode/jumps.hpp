#pragma once

#include <cstdint>

#include "yarnbox/bytecode/tree.hpp"

namespace yarn {

struct JumpSpan
{
	enum class Type : uint8_t
	{
		Absolute,
		Relative,
	};

	Type type;
	/// Reference point from which a jump is taken. This is only relevant for relative jumps, since
	/// absolute jumps are always performed from `ip`.
	int8_t reference = 0;
	/// The position in the bytecode at which the jump offset is encoded.
	uint16_t ip;
};

struct Jumps
{
	std::vector<JumpSpan> spans;

	void Analyze(const BytecodeTree& tree, BytecodeTree::NodeIndex rootNode);
};

}
