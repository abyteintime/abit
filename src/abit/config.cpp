#include "config.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "lib/ini.h"

#include "abit/error.hpp"
#include "abit/io.hpp"
#include "abit/paths.hpp"

using namespace abit;

static const char* configFileName = "ByteinTime.ini";

Config
Config::Load(const std::filesystem::path& path)
{
	Config config;

	std::vector<char> configFile = ReadFile(path);
	if (ini_parse_string(&configFile[0], &Config::IniKeyHandler, &config) < 0) {
		throw Error{ std::string{ "failed to load config file " } + configFileName };
	}

	return config;
}

void
Config::SaveDefault(const std::filesystem::path& path)
{
	std::ofstream outputStream{ path };
	outputStream << defaultConfigIni;
}

Config
Config::LoadOrSaveDefault(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path)) {
		SaveDefault(path);
	}
	return Load(path);
}

bool
Config::ParseBool(std::string_view string)
{
	if (string == "true" || string == "yes" || string == "1") {
		return true;
	} else {
		return false;
	}
}

int
Config::IniKeyHandler(void* user, const char* psection, const char* pkey, const char* pvalue)
{
	std::string_view section{ psection };
	std::string_view key{ pkey };
	std::string_view value{ pvalue };
	Config* config = reinterpret_cast<Config*>(user);

	const auto match = [section, key](const char* againstSection, const char* againstKey) {
		return section == againstSection && key == againstKey;
	};

	if (match("Game", "Executable")) {
		config->game.executable = value;
	} else if (match("Game", "WorkingDirectory")) {
		config->game.workingDirectory = value;
	} else if (match("Mods", "+Disable")) {
		config->mods.disable.insert(std::string{ value });
	} else if (match("Log", "FileLevel")) {
		config->log.fileLevel = value;
	} else if (match("Log", "ConsoleLevel")) {
		config->log.consoleLevel = value;
	} else if (match("Debug", "WaitForDebugger")) {
		config->debug.waitForDebugger = ParseBool(value);
	} else {
		return false;
	}

	return true;
}
