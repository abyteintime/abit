#include "config.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "lib/ini.h"

#include "abit/error.hpp"

using namespace abit;

static const char* configFileName = "ByteinTime.ini";

Config
Config::Load()
{
	Config config;

	if (ini_parse(configFileName, &Config::IniKeyHandler, &config) < 0) {
		throw Error{ std::string{ "failed to load config file " } + configFileName };
	}

	return config;
}

void
Config::SaveDefault()
{
	std::ofstream outputStream{ configFileName };
	outputStream << defaultConfigIni;
}

Config
Config::LoadOrSaveDefault()
{
	if (!std::filesystem::exists(configFileName)) {
		SaveDefault();
	}
	return Load();
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
		config->game.executable = std::string{ pvalue };
	} else if (match("Game", "WorkingDirectory")) {
		config->game.workingDirectory = std::string{ pvalue };
	} else {
		return false;
	}

	return true;
}
