#pragma once

#include <optional>
#include <unordered_map>
#include <utility>

#include "abit/fnv.hpp"

#include "yarnbox/bytecode/opcode.hpp"
#include "yarnbox/bytecode/tree.hpp"

template<>
struct std::hash<std::pair<yarn::Opcode, uint32_t>>
{
	size_t operator()(const std::pair<yarn::Opcode, uint32_t> pair) const
	{
		abit::Fnv1a64 fnv;
		fnv.Mix(static_cast<uint64_t>(pair.first));
		fnv.Mix(static_cast<uint64_t>(pair.second));
		return fnv.hash;
	}
};

namespace yarn {

struct Index
{
	std::unordered_map<std::pair<Opcode, uint32_t>, BytecodeTree::NodeIndex> nodeSearchMap;
	std::unordered_map<Opcode, uint32_t> counts;

	void Analyze(const BytecodeTree& tree);

	std::optional<BytecodeTree::NodeIndex> FindNode(Opcode opcode, int32_t relativeIndex) const;
	std::optional<BytecodeTree::NodeIndex> GetNode(Opcode opcode, uint32_t occurrenceIndex) const;
	uint32_t Count(Opcode opcode) const;

private:
	uint32_t GetNextIndexForOpcode(Opcode opcode);
};

}
