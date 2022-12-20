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

	static auto Load() -> Config;
	static auto SaveDefault() -> void;
	static auto LoadOrSaveDefault() -> Config;

	static const std::string_view defaultConfigIni;

private:
	static auto IniKeyHandler(void* user, const char* section, const char* key, const char* value)
		-> int;
};

}
