#pragma once

#include <string_view>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace abit {

struct Process
{
	Process(
		std::wstring_view executablePath,
		std::wstring_view workingDirectory,
		std::wstring environment
	);
	~Process();

	void WaitForExit();

	HANDLE handle = nullptr;
	HANDLE threadHandle = nullptr;
};

}
