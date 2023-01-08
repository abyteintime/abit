#include "yarnbox/bytecode/opcode.hpp"

namespace yarn::detail {

struct OpcodeNames
{
	std::string_view opcodes[opcodeCount];
	std::string_view primitiveCasts[primitiveCastCount];

	constexpr OpcodeNames()
	{
#define YARN__EXPAND_OPCODE_AS_NAME_ASSIGNMENT(name, index) \
	opcodes[index] = std::string_view(#name);
#define YARN__EXPAND_CAST_AS_OPCODE_NAME_ASSIGNMENT(name, index) \
	opcodes[size_t(PrimitiveCastToOpcode(PrimitiveCast(index)))] = \
		std::string_view("PrimitiveCast." #name);
#define YARN__EXPAND_CAST_AS_NAME_ASSIGNMENT(name, index) \
	primitiveCasts[index] = std::string_view(#name);

		YARN_X_OPCODES(YARN__EXPAND_OPCODE_AS_NAME_ASSIGNMENT);

		YARN_X_PRIMITIVE_CASTS(YARN__EXPAND_CAST_AS_OPCODE_NAME_ASSIGNMENT);
		YARN_X_PRIMITIVE_CASTS(YARN__EXPAND_CAST_AS_NAME_ASSIGNMENT);

#undef YARN__EXPAND_OPCODE_AS_NAME_ASSIGNMENT
#undef YARN__EXPAND_CAST_AS_NAME_ASSIGNMENT
	}
};

}

static constexpr yarn::detail::OpcodeNames names;

std::string_view
yarn::OpcodeToString(Opcode opcode)
{
	return names.opcodes[static_cast<size_t>(opcode)];
}

std::string_view
yarn::PrimitiveCastToString(PrimitiveCast cast)
{
	return names.primitiveCasts[static_cast<size_t>(cast)];
}
