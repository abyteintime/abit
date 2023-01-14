#pragma once

#include <vector>

#include "yarnbox/bytecode/opcode.hpp"

namespace yarn::codegen {

void
StaticFinalFunctionCall(struct UFunction* function, std::vector<uint8_t>& outBytecode);

}
