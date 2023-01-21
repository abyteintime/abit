#include "abit/ue/AGameMod.hpp"

#include "abit/procs/eb.hpp"

using namespace ue;

void
AGameMod::GetInstalledModPackagesAndPaths(
	TArray<FString>& outLocalModPackages,
	TArray<FString>& outLocalModPaths
)
{
	abit::procs::P_ebbb59f52740877e7e3065f14258a2e7_0.Call<void>(
		&outLocalModPackages, &outLocalModPaths, FString{}, 0, FString{}
	);
}
