#include "abit/paths.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::wstring
abit::GetExecutablePath(HINSTANCE instance)
{
	std::wstring buffer{ 256, L'\0' };
	size_t length = 0;
	while (true) {
		length = static_cast<size_t>(GetModuleFileNameW(instance, buffer.data(), buffer.size()));
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			buffer = std::wstring(buffer.size() * 2, L'\0');
		} else {
			break;
		}
	}
	buffer.resize(length);
	return buffer;
}