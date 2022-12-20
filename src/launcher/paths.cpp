#include "paths.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "string.hpp"

#include <filesystem>
#include <string>

auto
abit::GetGameExecutablePath(std::string_view installDirectory) -> std::wstring
{
	return Widen(installDirectory) + L"\\Binaries\\Win64\\HatinTimeGame.exe";
}

auto
abit::GetThisExecutablePath() -> std::wstring
{
	std::wstring buffer{ 256, L'\0' };
	size_t length = 0;
	while (true) {
		length = static_cast<size_t>(GetModuleFileNameW(nullptr, buffer.data(), buffer.size()));
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			buffer = std::wstring(buffer.size() * 2, L'\0');
		} else {
			break;
		}
	}
	buffer.resize(length);
	return buffer;
}

auto
abit::GetLoaderDllPath() -> std::wstring
{
	std::wstring thisExecutable = GetThisExecutablePath();
	std::filesystem::path thisExecutablePath{ thisExecutable };
	return thisExecutablePath.parent_path().append(L"AByteInTime.Loader.dll");
}
