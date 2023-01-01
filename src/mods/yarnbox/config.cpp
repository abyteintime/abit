#include "yarnbox/config.hpp"

#include <fstream>

#include "fmt/format.h"

#include "nlohmann/json.hpp"

#include "abit/error.hpp"

#include "abit/loader/logging.hpp"

using namespace yarn;

using Json = nlohmann::json;

static constexpr uint32_t currentConfigVersion = 1;

static ModConfig
ModConfigFromJson(const Json& json)
{
	ModConfig config;

	json.at("version").get_to(config.version);
	if (config.version != currentConfigVersion) {
		spdlog::warn(
			"Mod has incompatible version of Yarnbox.Patches.json (this version of Yarnbox expects "
			"{}, but the json file declares {}.) Expect loading errors or subtle incompatibilities",
			currentConfigVersion,
			config.version
		);
	}

	return config;
}

std::optional<std::filesystem::path>
ModConfig::PathInModDirectory(const std::filesystem::path& modDirectory)
{
	std::filesystem::path configPath = modDirectory / "Yarnbox.Patches.json";
	if (std::filesystem::exists(configPath)) {
		return configPath;
	} else {
		return std::nullopt;
	}
}

ModConfig
ModConfig::Load(const std::filesystem::path& path)
{
	std::ifstream inputFile{ path };
	if (!inputFile.good()) {
		throw abit::Error{ "cannot open config file for loading" };
	}

	try {
		Json json = Json::parse(inputFile);
		return ModConfigFromJson(json);
	} catch (nlohmann::json::exception e) {
		throw abit::Error{ fmt::format("error while reading Yarnbox.Patches.json: {}", e.what()) };
	} catch (abit::Error e) {
		throw abit::Error{ fmt::format("error while reading Yarnbox.Patches.json: {}", e.what) };
	}
}
