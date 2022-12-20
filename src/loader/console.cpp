#include "console.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static HANDLE consoleStdout = nullptr;

void
abit::InitializeConsole()
{
	AllocConsole();
	consoleStdout = GetStdHandle(STD_OUTPUT_HANDLE);
}

void
abit::Print(std::wstring_view text)
{
	WriteConsoleW(consoleStdout, text.data(), text.size(), nullptr, nullptr);
}

void
abit::PrintLine(std::wstring_view text)
{
	Print(text);
	Print(L"\n");
}
