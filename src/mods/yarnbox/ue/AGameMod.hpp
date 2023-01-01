#pragma once

#include "yarnbox/ue/FString.hpp"
#include "yarnbox/ue/TArray.hpp"

namespace ue {

struct AGameMod
{
	static void GetLocalModPackagesAndPaths(
		TArray<FString>& outLocalModPackages,
		TArray<FString>& outLocalModPaths
	);
};

}
