#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "abit/ue/UFunction.hpp"

#include "yarnbox/bytecode/codegen.hpp"
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

	ue::UFunction* into;
	std::vector<OpcodeQuery> select;
	std::shared_ptr<codegen::CodeGenerator> generate = nullptr;
};

enum class PatchType
{
	Injection,
};

struct Patch
{
	struct Injection
	{
		std::vector<yarn::Injection> inject;
	};

	std::string comment;
	std::variant<Injection> data;

	inline PatchType GetType() const { return static_cast<PatchType>(data.index()); }
};

}
