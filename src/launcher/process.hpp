#pragma once

#include <string_view>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace abit {

struct Process
{
	Process(std::wstring_view executablePath, std::wstring_view workingDirectory);
	~Process();

	auto WaitForExit() -> void;

	HANDLE handle = nullptr;
	HANDLE threadHandle = nullptr;
};

}
