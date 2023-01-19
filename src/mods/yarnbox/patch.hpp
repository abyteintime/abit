#pragma once

#include <string>
#include <variant>
#include <vector>

#include "yarnbox/bytecode/opcode.hpp"

namespace yarn {

struct Injection
{
	struct OpcodeQuery
	{
		struct AllOccurrences
		{};

		enum class Pick
		{
			Span,
			Start,
			End,
		};

		Opcode opcode;
		std::variant<AllOccurrences, std::vector<int32_t>> which;
		Pick pick;
	};

	using Query = std::variant<OpcodeQuery>;

	struct StaticFinalFunctionCall
	{
		std::string function;
		bool captureSelf = false;
	};

	using CodeGen = std::variant<StaticFinalFunctionCall>;

	std::string into;
	std::vector<Query> select;
	CodeGen place;
};

enum class PatchType
{
	Replacement,
	Injection,
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
