#pragma once

#include "yarnbox/patch.hpp"
#include "yarnbox/registry.hpp"

namespace yarn {

void
ApplyPatch(const Patch& patch, const Registry& registry);

}
