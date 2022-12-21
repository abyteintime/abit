#include "abit/launcher/paths.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "abit/paths.hpp"
#include "abit/string.hpp"

#include <filesystem>
#include <string>

std::wstring
abit::GetGameExecutablePath(std::string_view installDirectory)
{
	return Widen(installDirectory) + L"\\Binaries\\Win64\\HatinTimeGame.exe";
}

std::wstring
abit::GetLoaderDllPath()
{
	std::wstring thisExecutable = GetExecutablePath();
	std::filesystem::path thisExecutablePath{ thisExecutable };
	return thisExecutablePath.parent_path().append(L"AByteInTime.Loader.dll");
}
