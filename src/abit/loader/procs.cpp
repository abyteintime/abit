#include "abit/loader/procs.hpp"

void* const abit::Proc::baseAddress = (void*)0x140000000;

// TODO: Auto-generate this from the executable's symbol table.
const abit::Proc abit::procs::GuardedMainWrapper{ "GuardedMainWrapper", (void*)0x140a74810 };
const abit::Proc abit::procs::UHat_GlobalTimer::GetGameTime{ "UHat_GlobalTimer::GetGameTime",
															 (void*)0x140aea2f0 };