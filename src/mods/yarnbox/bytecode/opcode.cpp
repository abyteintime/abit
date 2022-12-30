#include "yarnbox/bytecode/opcode.hpp"

namespace yarn::detail {

struct OpcodeNames
{
	std::string_view names[opcodeCount];

	constexpr OpcodeNames()
	{
#define YARN__EXPAND_OPCODE_AS_NAME_ASSIGNMENT(name, index) names[index] = std::string_view(#name);
		YARN_X_OPCODES(YARN__EXPAND_OPCODE_AS_NAME_ASSIGNMENT);
#undef YARN__EXPAND_OPCODE_AS_NAME_ASSIGNMENT
	}
};

}

static constexpr yarn::detail::OpcodeNames opcodeNames;

std::string_view
yarn::OpcodeToString(Opcode opcode)
{
	return opcodeNames.names[static_cast<size_t>(opcode)];
}
