#pragma once

#include "yarnbox/bytecode/opcode.hpp"

#include <optional>
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
	std::vector<std::wstring> wideStrings;
	std::optional<size_t> firstError = std::nullopt;

	NodeIndex AppendNode(Node node);
	DataIndex AppendData(size_t count);
	DataIndex AppendDataFromVector(const std::vector<uint64_t>& source);
	DataIndex AppendString(std::string&& string);
	DataIndex AppendWideString(std::wstring&& wideString);

	void SetFirstErrorIfNull(size_t ip);
	bool IsBytecodeBogusAt(size_t ip) const;

	inline uint64_t& Data(DataIndex index, size_t offset) { return data.at(index + offset); }
	inline uint64_t Data(DataIndex index, size_t offset) const { return data.at(index + offset); }
};

}
