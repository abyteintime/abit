#pragma once

#include <string>
#include <variant>

namespace yarn {

enum class PatchType
{
	Replacement,
	Injection,
};

struct Patch
{
	struct Replacement
	{
		std::string className;
		std::string function;
	};

	struct Injection
	{
		// TBD
	};

	std::string comment;
	std::variant<Replacement, Injection> data;

	inline PatchType GetType() const { return static_cast<PatchType>(data.index()); }
};

}
