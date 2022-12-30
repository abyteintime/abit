#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_set>

namespace abit {

struct Config
{
	struct Game
	{
		std::string workingDirectory;
		std::string executable;
	} game;

	struct Mods
	{
		std::unordered_set<std::string> disable;
	} mods;

	struct Log
	{
		std::string fileLevel = "debug";
		std::string consoleLevel = "info";
	} log;

	struct Debug
	{
		bool waitForDebugger = false;
	} debug;

	static Config Load(const std::filesystem::path& path);
	static void SaveDefault(const std::filesystem::path& path);
	static Config LoadOrSaveDefault(const std::filesystem::path& path);

	static bool ParseBool(std::string_view string);

	static const std::string_view defaultConfigIni;

private:
	static int IniKeyHandler(void* user, const char* section, const char* key, const char* value);
};

}
