#include "yarnbox/bytecode/codegen.hpp"

#include <cassert>

#include "abit/loader/ensure.hpp"

#include "abit/ue/UClass.hpp"
#include "abit/ue/cast.hpp"

#include "yarnbox/bytecode/opcode.hpp"

using namespace yarn;
using namespace ue;

void
codegen::BeginStaticFinalFunctionCall(UFunction* function, std::vector<uint8_t>& outBytecode)
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
	// totalSize += 1; //     EndFunctionParms - added dynamically
	size_t startIndex = outBytecode.size();
	outBytecode.resize(startIndex + totalSize);

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

	{
		uint8_t* code = &outBytecode[startIndex];
		code[0] = static_cast<uint8_t>(Opcode::ClassContext);
		code[1] = static_cast<uint8_t>(Opcode::ObjectConst);
		*(UObject**)&code[2] = owningClass;
		// And the following (bytes 10..20) is the bogus part, which is never actually read by the
		// VM.
		code[10] = 0;
		code[11] = 0;
		*(UObject**)&code[12] = nullptr;
		code[20] = 0;
		code[21] = static_cast<uint8_t>(Opcode::FinalFunction);
		*(UObject**)&code[22] = function;
	}
}

void
codegen::EndStaticFinalFunctionCall(std::vector<uint8_t>& outBytecode)
{
	outBytecode.push_back(static_cast<uint8_t>(Opcode::EndFunctionParms));
}

void
codegen::Self(std::vector<uint8_t>& outBytecode)
{
	outBytecode.push_back(static_cast<uint8_t>(Opcode::Self));
}
