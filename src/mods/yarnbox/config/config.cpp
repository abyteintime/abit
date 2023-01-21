#include "yarnbox/config/config.hpp"

#include <fstream>

#include "fmt/format.h"
#include "sol/sol.hpp"

#include "abit/error.hpp"
#include "abit/io.hpp"

#include "abit/loader/logging.hpp"

#include "abit/ue/UStruct.hpp"
#include "abit/ue/cast.hpp"

#include "yarnbox/config/common.hpp"
#include "yarnbox/config/opcode.hpp"

using namespace yarn;

namespace yarn::config {
extern const char* bootstrapLua;
}

static sol::table
LoadYarnboxIntoLua(sol::state& lua, Registry& registry)
{
	sol::table yarnbox = lua.create_table();

	// Base patch API

	auto patchType = yarnbox.new_usertype<Patch>("Patch", sol::default_constructor);
	patchType["comment"] = &Patch::comment;
	patchType["MakeIntoInjection"] = [](Patch& p, sol::table injectionsTable) {
		std::vector<Injection> injections;
		config::CopyTableToVector(injectionsTable, injections);
		p.data = Patch::Injection{ injections };
	};

	auto injectionPatchType
		= yarnbox.new_usertype<Injection>("Injection", sol::default_constructor);
	injectionPatchType["into"] = &Injection::into;
	injectionPatchType["select"] = &Injection::select;
	injectionPatchType["generate"] = &Injection::generate;

	// Enums

	config::LoadOpcodeIntoLua(yarnbox);

	// Injection code generators

	auto staticFinalFunctionCallGeneratorType
		= yarnbox.new_usertype<codegen::StaticFinalFunctionCallGenerator>(
			"StaticFinalFunctionCallGenerator"
		);
	staticFinalFunctionCallGeneratorType["captureSelf"]
		= &codegen::StaticFinalFunctionCallGenerator::captureSelf;
	staticFinalFunctionCallGeneratorType["Downcast"]
		= [](const std::shared_ptr<codegen::StaticFinalFunctionCallGenerator>& self
		  ) -> std::shared_ptr<codegen::CodeGenerator> { return self; };

	yarnbox["StaticFinalFunctionCall"] = [](Chunk* function) {
		auto generator = std::make_shared<codegen::StaticFinalFunctionCallGenerator>();
		generator->function = ue::Cast<ue::UFunction>(function->ustruct);
		return generator;
	};

	// Object references

	yarnbox["GetFunction"] = [&registry](const std::string& name) -> Chunk* {
		if (Chunk* chunk = registry.GetChunkByName(name)) {
			if (chunk->ustruct->InstanceOf<ue::UFunction>()) {
				return chunk;
			} else {
				throw abit::Error::Format(
					"chunk '{}' is not a function (it is a {})",
					name,
					chunk->ustruct->objectClass->GetName().ToString()
				);
			}
		} else {
			throw abit::Error::Format("function '{}' does not exist", name);
		}
	};

	// Console

	yarnbox["LogInfo"] = [](const std::string& text) { spdlog::info("(Lua) {}", text); };

	lua["_yarnbox"] = yarnbox;
	return yarnbox;
}

static ModConfig
LoadModConfig(const std::string& source, Registry& registry)
{
	sol::state lua;
	lua.open_libraries(
		sol::lib::base,
		sol::lib::coroutine,
		sol::lib::debug,
		sol::lib::math,
		sol::lib::string,
		sol::lib::table,
		sol::lib::utf8
	);
	sol::table lib = LoadYarnboxIntoLua(lua, registry);

	try {
		lua.script(config::bootstrapLua, sol::script_throw_on_error, "@Yarnbox/bootstrap.lua");
		lua.script(source, sol::script_throw_on_error, "@Yarnbox.Patches.lua");
	} catch (sol::error e) {
		throw abit::Error{ fmt::format("Lua error: {}", e.what()) };
	}

	ModConfig config;
	sol::table patches = lib["outPatches"];
	config::CopyTableToVector(patches, config.patches);
	return config;
}

std::optional<std::filesystem::path>
ModConfig::PathInModDirectory(const std::filesystem::path& modDirectory)
{
	std::filesystem::path configPath = modDirectory / "Yarnbox.Patches.lua";
	if (std::filesystem::exists(configPath)) {
		return configPath;
	} else {
		return std::nullopt;
	}
}

ModConfig
ModConfig::Load(const std::filesystem::path& path, Registry& registry)
{
	std::string source = abit::ReadFileToString(path);

	try {
		return LoadModConfig(source, registry);
	} catch (abit::Error e) {
		throw abit::Error{ fmt::format("Error while reading Yarnbox.Patches.lua: {}", e.message) };
	}
}
