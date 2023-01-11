#pragma once

#include <string>
#include <variant>
#include <vector>

#include "yarnbox/bytecode/opcode.hpp"

namespace yarn {

enum class PatchType
{
	Replacement,
	Injection,
};

struct Injection
{
	struct HeadQuery
	{};

	struct OpcodeQuery
	{
		struct AllOccurrences
		{};

		enum SearchFrom
		{
			Start,
			End,
		};

		Opcode opcode;
		std::variant<AllOccurrences, std::vector<uint32_t>> which;
		SearchFrom searchFrom;
	};

	using Query = std::variant<HeadQuery, OpcodeQuery>;

	enum class Action
	{
		Prepend,
		Append,
		Replace,
		Insert,
	};

	struct StaticFinalFunctionCall
	{
		std::string function;
	};

	using CodeGen = std::variant<StaticFinalFunctionCall>;

	std::string into;
	std::vector<Query> select;
	Action action;
	CodeGen with;
};

struct Patch
{
	struct Replacement
	{
		std::string chunk;
	};

	struct Injection
	{
		std::vector<yarn::Injection> inject;
	};

	std::string comment;
	std::variant<Replacement, Injection> data;

	inline PatchType GetType() const { return static_cast<PatchType>(data.index()); }
};

}
