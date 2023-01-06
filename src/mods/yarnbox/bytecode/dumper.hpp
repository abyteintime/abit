#pragma once

#include <string>

#include "yarnbox/bytecode/tree.hpp"

namespace yarn {

void
DumpNode(const BytecodeTree& tree, BytecodeTree::NodeIndex node, std::string& outString);

}
