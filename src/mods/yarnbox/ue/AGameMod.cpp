#include "yarnbox/ue/AGameMod.hpp"

#include "abit/procs/AGameMod.hpp"

using namespace ue;

void
AGameMod::GetLocalModPackagesAndPaths(
	TArray<FString>& outLocalModPackages,
	TArray<FString>& outLocalModPaths
)
{
	abit::procs::AGameMod::GetLocalModPackagesAndPaths.Call<void>(
		&outLocalModPackages, &outLocalModPaths, FString{}, 0, FString{}
	);
}
