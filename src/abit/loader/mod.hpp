#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <filesystem>

namespace abit {

struct Mod
{
	Mod(const std::filesystem::path& dllPath);
	~Mod();

	Mod(const Mod&) = delete;
	Mod(Mod&&);

	void CallModInit();

private:
	HINSTANCE dll;

	using ModInitProc = void (*)();
	ModInitProc modInit;
};

}
