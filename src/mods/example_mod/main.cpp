#include "abit/dll_macros.hpp"

#include "abit/loader/console.hpp"

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::PrintLine(L"Hello from the Example Mod!");
}
