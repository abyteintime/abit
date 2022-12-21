#include "abit/launcher/paths.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "abit/paths.hpp"
#include "abit/string.hpp"

#include <filesystem>
#include <string_view>

std::filesystem::path
abit::GetGameExecutablePath(std::string_view installDirectory)
{
	return Widen(installDirectory) + L"\\Binaries\\Win64\\HatinTimeGame.exe";
}

std::filesystem::path
abit::GetLoaderDllPath()
{
	return GetExecutablePath().parent_path().append(L"AByteInTime.Loader.dll");
}
