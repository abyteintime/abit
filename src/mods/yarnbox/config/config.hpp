#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

#include "yarnbox/patch.hpp"

namespace yarn {

struct ModConfig
{
	std::vector<Patch> patches;

	static std::optional<std::filesystem::path> PathInModDirectory(
		const std::filesystem::path& modDirectory
	);
	static ModConfig Load(const std::filesystem::path& path);
};

}
