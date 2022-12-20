#pragma once

#include <string>
#include <string_view>

namespace abit {

struct Config
{
	struct Game
	{
		std::string workingDirectory;
		std::string executable;
	} game;

	static Config Load();
	static void SaveDefault();
	static Config LoadOrSaveDefault();

	static const std::string_view defaultConfigIni;

private:
	static int IniKeyHandler(void* user, const char* section, const char* key, const char* value);
};

}
