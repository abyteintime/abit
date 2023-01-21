// An example mod which changes the in-game speedrun timer to always show 4:20 (and also changes the
// act timer to 21:37 because why not.)

#include "abit/dll_macros.hpp"

#include "abit/loader/logging.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/90.hpp"
#include "abit/procs/e0.hpp"

static float (*O_GetGameTime)();
static float
GetGameTime()
{
	return 4.f * 60.f + 20.f;
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
	abit::Patch(abit::procs::P_e09fc84f7eff2eef5e637fdf1ca84153_0, GetGameTime, O_GetGameTime);
	abit::Patch(
		abit::procs::P_901b4e3ea33ba7f82dc72aaf6aebd1c5_0,
		GetTotalTimePieceCount,
		O_GetTotalTimePieceCount
	);
	spdlog::info("Your speedrun timer now shows 4:20.");
}
