#pragma once

#include "yarnbox/bytecode/opcode.hpp"

#include <optional>
#include <vector>

namespace yarn {

struct BytecodeTree
{
	using NodeIndex = uint32_t;
	using DataIndex = uint64_t;

	/// Half-inclusive range of instruction pointers [start; end).
	struct Span
	{
		uint16_t start;
		uint16_t end;

		static inline Span Invalid() { return { 0xFFFF, 0xFFFF }; }
	};

	struct Node
	{
		uint64_t data = 0;
		Opcode opcode;
		Span span;

		Node(Span span, Opcode opcode, uint64_t data = 0)
			: opcode(opcode)
			, span(span)
			, data(data)
		{
		}
	};

	std::vector<Node> nodes;
	std::vector<uint64_t> data;
	std::vector<Span> dataSpans;
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
	inline Span DataSpan(DataIndex index, size_t offset) const
	{
		return index + offset < dataSpans.size() ? dataSpans.at(index + offset) : Span::Invalid();
	}

	void SetDataSpan(DataIndex index, Span span);
};

}
