#include "yarnbox/config/config.hpp"

#include <fstream>

#include "fmt/format.h"
#include "sol/sol.hpp"

#include "abit/error.hpp"
#include "abit/io.hpp"
#include "abit/loader/logging.hpp"

#include "yarnbox/config/opcode.hpp"

using namespace yarn;

namespace yarn::config {
extern const char* bootstrapLua;
}

static void
LoadYarnboxIntoLua(sol::state& lua)
{
	sol::table yarnbox;

	config::LoadOpcodeIntoLua(yarnbox);

	auto injectionPatchType
		= yarnbox.new_usertype<Injection>("Injection", sol::default_constructor);
	injectionPatchType["into"] = &Injection::into;
	injectionPatchType["select"] = &Injection::select;
	injectionPatchType["generate"] = &Injection::generate;

	auto patchType = yarnbox.new_usertype<Patch>("Patch", sol::default_constructor);
	patchType["comment"] = &Patch::comment;
	patchType["MakeIntoInjection"] = [](Patch& p, const std::vector<Injection>& injections) {
		p.data = Patch::Injection{ injections };
	};

	lua["Yarnbox"] = std::move(yarnbox);
}

static ModConfig
LoadModConfig(const std::string& source)
{
	sol::state lua;
	lua.open_libraries(
		sol::lib::base,
		sol::lib::coroutine,
		sol::lib::math,
		sol::lib::string,
		sol::lib::table,
		sol::lib::utf8
	);
	LoadYarnboxIntoLua(lua);

	try {
		lua.safe_script(config::bootstrapLua, sol::script_throw_on_error, "@Yarnbox/bootstrap.lua");
		lua.safe_script(source, sol::script_throw_on_error, "@Yarnbox.Patches.lua");
	} catch (sol::error e) {
		throw abit::Error{ fmt::format("Lua error: {}", e.what()) };
	}

	ModConfig config;
	std::vector<Patch> patches = lua["Yarnbox"]["outPatches"];
	config.patches = std::move(patches);
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
ModConfig::Load(const std::filesystem::path& path)
{
	std::string source = abit::ReadFileToString(path);

	try {
		return LoadModConfig(source);
	} catch (abit::Error e) {
		throw abit::Error{ fmt::format("Error while reading Yarnbox.Patches.lua: {}", e.message) };
	}
}
