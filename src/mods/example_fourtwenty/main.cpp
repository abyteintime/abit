// An example mod which changes the in-game speedrun timer to always show 4:20 (and also changes the
// act timer to 21:37 because why not.)

#include "abit/dll_macros.hpp"

#include "abit/loader/logging.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/UHat_ChapterInfo.hpp"
#include "abit/procs/UHat_GlobalTimer.hpp"

float (*O_GetGameTime)();
static float
GetGameTime()
{
	return 4.f * 60.f + 20.f;
}

float (*O_GetActTime)();
static float
GetActTime()
{
	return 21.f * 60.f + 37.f;
}

static int32_t (*O_GetTotalTimePieceCount)();
static int32_t
GetTotalTimePieceCount()
{
	return std::numeric_limits<int32_t>::max();
}

extern "C" ABIT_DLL_EXPORT void
ABiT_ModInit()
{
	abit::Patch(abit::procs::UHat_GlobalTimer::GetGameTime, GetGameTime, O_GetGameTime);
	abit::Patch(abit::procs::UHat_GlobalTimer::GetActTime, GetActTime, O_GetActTime);
	abit::Patch(
		abit::procs::UHat_ChapterInfo::GetTotalTimePieceCount,
		GetTotalTimePieceCount,
		O_GetTotalTimePieceCount
	);
	spdlog::info("Your speedrun timer now shows 4:20.");
}
