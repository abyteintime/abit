#pragma once

#include <vector>

#include "abit/ue/UFunction.hpp"

#include "yarnbox/bytecode/opcode.hpp"

namespace yarn::codegen {

void
BeginStaticFinalFunctionCall(ue::UFunction* function, std::vector<uint8_t>& outBytecode);

void
EndStaticFinalFunctionCall(std::vector<uint8_t>& outBytecode);

void
Self(std::vector<uint8_t>& outBytecode);

}
