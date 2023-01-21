#pragma once

#include "abit/ue/FString.hpp"
#include "abit/ue/TArray.hpp"

namespace ue {

struct AGameMod
{
	static void GetInstalledModPackagesAndPaths(
		TArray<FString>& outLocalModPackages,
		TArray<FString>& outLocalModPaths
	);
};

}
