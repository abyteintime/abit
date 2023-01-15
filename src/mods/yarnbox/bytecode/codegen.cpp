#include "yarnbox/bytecode/codegen.hpp"

#include <cassert>

#include "abit/loader/ensure.hpp"

#include "abit/ue/UClass.hpp"
#include "abit/ue/cast.hpp"

#include "yarnbox/bytecode/opcode.hpp"

using namespace yarn;
using namespace ue;

void
codegen::StaticFinalFunctionCall(UFunction* function, std::vector<uint8_t>& outBytecode)
{
	assert(function != nullptr);

	// A static final function call (with no parameters) is 31 bytes in total:
	size_t totalSize = 0;
	totalSize += 1; // ClassContext
	totalSize += 1; //   ObjectConst
	totalSize += 8; //     obj
	totalSize += 2; //   orel+12
	totalSize += 8; //   obj
	totalSize += 1; //   u8
	totalSize += 1; //   FinalFunction
	totalSize += 8; //     obj
	totalSize += 1; //     EndFunctionParms
	outBytecode.resize(totalSize);

	UStruct* owningClass = Cast<UStruct>(function->outer);
	ABIT_ENSURE(owningClass != nullptr, "function does not have an owning class to call it on");

	// To call a static function, we need to use ClassContext with the class to call the function
	// on. This will switch the context to the class's default object, on which we can execute a
	// FinalFunction instruction.
	//
	// We do our best to generate legit bytecode here; I say, "do our best" because this bytecode is
	// actually hardly even correct. It would not work for _all_ cases, but it should work for ours.
	// Context will use its operands 2..4 to jump over the expression we want to evaluate on the
	// class in case it's null. However in our case it is never null, since ObjectConst always
	// produces a valid object. Or so we hope. We check this with the ABIT_ENSURE above to be sure.
	// Only a monster would disable exceptions.

	outBytecode[0] = static_cast<uint8_t>(Opcode::ClassContext);
	outBytecode[1] = static_cast<uint8_t>(Opcode::ObjectConst);
	*(UObject**)&outBytecode[2] = owningClass;
	// And the following (bytes 10..20) is the bogus part, which is never actually read by the VM.
	outBytecode[10] = 0;
	outBytecode[11] = 0;
	*(UObject**)&outBytecode[12] = nullptr;
	outBytecode[20] = 0;
	outBytecode[21] = static_cast<uint8_t>(Opcode::FinalFunction);
	*(UObject**)&outBytecode[22] = function;
	outBytecode[30] = static_cast<uint8_t>(Opcode::EndFunctionParms);
}
