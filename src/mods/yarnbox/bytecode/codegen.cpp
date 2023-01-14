#include "yarnbox/bytecode/codegen.hpp"

#include "yarnbox/bytecode/opcode.hpp"

using namespace yarn;

void
codegen::StaticFinalFunctionCall(struct UFunction* function, std::vector<uint8_t>& outBytecode)
{
	outBytecode.resize(10);

	outBytecode[0] = static_cast<uint8_t>(Opcode::FinalFunction);
	struct UFunction** functionPtr = reinterpret_cast<UFunction**>(&outBytecode[1]);
	*functionPtr = function;
	outBytecode[9] = static_cast<uint8_t>(Opcode::EndFunctionParms);
}
