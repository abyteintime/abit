#pragma once

#include "yarnbox/bytecode/opcode.hpp"

#include <vector>

namespace yarn {

struct BytecodeTree
{
	using NodeIndex = uint32_t;
	using DataIndex = uint64_t;

	struct Node
	{
		const size_t ip;
		Opcode opcode;
		union
		{
			struct
			{
				uint32_t a, b;
			} u32pair;
			uint64_t u64 = 0;
		} data;

		inline Node WithU32Pair(uint32_t a, uint32_t b)
		{
			data.u32pair.a = a;
			data.u32pair.b = b;
			return *this;
		}

		inline Node WithU64(uint64_t x)
		{
			data.u64 = x;
			return *this;
		}
	};

	std::vector<Node> nodes;
	std::vector<uint64_t> data;
	std::vector<std::string> strings;

	NodeIndex AppendNode(Node node);
	DataIndex AppendData(size_t count);
	DataIndex AppendString(std::string&& string);

	inline uint64_t& Data(DataIndex index, size_t offset) { return data.at(index + offset); }
};

}
