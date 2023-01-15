#pragma once

#include <vector>

#include "abit/ue/UFunction.hpp"

#include "yarnbox/bytecode/opcode.hpp"

namespace yarn::codegen {

void
StaticFinalFunctionCall(ue::UFunction* function, std::vector<uint8_t>& outBytecode);

}
