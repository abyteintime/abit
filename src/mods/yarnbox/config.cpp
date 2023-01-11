#include "yarnbox/config.hpp"

#include <fstream>

#include "fmt/format.h"

#include "nlohmann/json.hpp"

#include "abit/error.hpp"
#include "abit/loader/logging.hpp"

#include "yarnbox/bytecode/opcode.hpp"

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

NLOHMANN_JSON_SERIALIZE_ENUM(
	Injection::Action,
	{
		{ Injection::Action::Prepend, "Prepend" },
		{ Injection::Action::Append, "Append" },
		{ Injection::Action::Replace, "Replace" },
		{ Injection::Action::Insert, "Insert" },
	}
);

NLOHMANN_JSON_SERIALIZE_ENUM(
	Injection::OpcodeQuery::SearchFrom,
	{
		{ Injection::OpcodeQuery::SearchFrom::Start, "Start" },
		{ Injection::OpcodeQuery::SearchFrom::End, "End" },
	}
);

#define YARN__EXPAND_OPCODE_AS_JSON_SERIALIZE_ENUM_PAIRS(Name, Index) { Opcode::Name, #Name },

NLOHMANN_JSON_SERIALIZE_ENUM(
	Opcode,
	// This might be a bit slow... does C++ have an efficient way of generating a trie of these, or
	// something? Maybe this should be a hash map?
	{ YARN_X_OPCODES(YARN__EXPAND_OPCODE_AS_JSON_SERIALIZE_ENUM_PAIRS) }
)

void
from_json(const Json& json, Injection::OpcodeQuery& query)
{
	json.at("opcode").get_to(query.opcode);
	if (json.at("which") == "all") {
		query.which = Injection::OpcodeQuery::AllOccurrences{};
	} else {
		std::vector<uint32_t> indices;
		json.at("which").get_to(indices);
		query.which = std::move(indices);
	}
	json.at("searchFrom").get_to(query.searchFrom);
}

void
from_json(const Json& json, Injection::Query& query)
{
	if (json == "head") {
		query = Injection::HeadQuery{};
	} else {
		Injection::OpcodeQuery opcodeQuery;
		json.get_to(opcodeQuery.opcode);
		query = std::move(opcodeQuery);
	}
}

void
from_json(const Json& json, Injection::CodeGen& codegen)
{
	std::string type;
	json.at("type").get_to(type);

	if (type == "StaticFinalFunctionCall") {
		Injection::StaticFinalFunctionCall cg;
		json.at("function").get_to(cg.function);
		codegen = std::move(cg);
	}
}

void
from_json(const Json& json, Injection& injection)
{
	json.at("into").get_to(injection.into);
	json.at("select").get_to(injection.select);
	json.at("do").get_to(injection.action);
	json.at("with").get_to(injection.with);
}

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
			json.at("chunk").get_to(replacement.chunk);
			patch.data = std::move(replacement);
			break;
		}
		case PatchType::Injection:
			Patch::Injection injection;
			json.at("inject").get_to(injection.inject);
			patch.data = std::move(injection);
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
