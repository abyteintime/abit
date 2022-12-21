#include "abit/dll_macros.hpp"

#include "abit/loader/console.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/UHat_GlobalTimer.hpp"

float (*O_GetGameTime)() = nullptr;
static float __cdecl GetGameTime()
{
	return 4.f * 60.f + 20.f;
}

float (*O_GetActTime)() = nullptr;
static float __cdecl GetActTime()
{
	return 21.f * 60.f + 37.f;
}

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::Patch(abit::procs::UHat_GlobalTimer::GetGameTime, GetGameTime, O_GetGameTime);
	abit::Patch(abit::procs::UHat_GlobalTimer::GetActTime, GetActTime, O_GetActTime);
	abit::PrintLine(L"Your speedrun timer now shows 4:20.");
}
