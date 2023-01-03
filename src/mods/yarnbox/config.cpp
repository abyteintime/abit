#include "yarnbox/config.hpp"

#include <fstream>

#include "fmt/format.h"

#include "nlohmann/json.hpp"

#include "abit/error.hpp"
#include "abit/loader/logging.hpp"

using namespace yarn;

using Json = nlohmann::json;

static constexpr uint32_t currentConfigVersion = 1;

namespace yarn {

NLOHMANN_JSON_SERIALIZE_ENUM(
	PatchType,
	{
		{ PatchType::Replacement, "Replacement" },
		{ PatchType::Injection, "Injection" },
	}
);

}

static Patch
PatchFromJson(const Json& json, uint32_t version)
{
	Patch patch;
	patch.comment = json.value("comment", "");

	PatchType type = json.at("type");
	switch (type) {
		case PatchType::Replacement: {
			Patch::Replacement replacement;
			json.at("class").get_to(replacement.className);
			json.at("function").get_to(replacement.function);
			patch.data = replacement;
			break;
		}
		case PatchType::Injection:
			spdlog::error("Injection-type patches are yet to be specified and implemented");
			break;
	}

	return patch;
}

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

	for (const Json& element : json.at("patches")) {
		config.patches.push_back(PatchFromJson(element, config.version));
	}

	spdlog::debug("Total {} patches defined by this mod", config.patches.size());

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
		throw abit::Error{ fmt::format("Error while reading Yarnbox.Patches.json: {}", e.what()) };
	} catch (abit::Error e) {
		throw abit::Error{ fmt::format("Error while reading Yarnbox.Patches.json: {}", e.what) };
	}
}
