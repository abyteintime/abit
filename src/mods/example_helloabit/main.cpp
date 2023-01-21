#include "abit/dll_macros.hpp"

#include "abit/loader/logging.hpp"

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	spdlog::info("Hello ABiT!");
}
