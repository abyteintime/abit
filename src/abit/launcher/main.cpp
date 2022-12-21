#include <cstdio>
#include <system_error>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "abit/defer.hpp"
#include "abit/error.hpp"
#include "abit/paths.hpp"
#include "abit/string.hpp"
#include "abit/version.hpp"

#include "abit/config.hpp"
#include "abit/launcher/paths.hpp"
#include "abit/launcher/process.hpp"

namespace abit {

void
LauncherMain()
{
	printf("Welcome to A Byte in Time, the Hat in Time hacking toolkit\n");
	printf("Launcher version %s\n", version);

	std::filesystem::path configPath = GetConfigPath();
	printf("Looking for config at %ls\n", configPath.wstring().c_str());
	Config config = Config::LoadOrSaveDefault(configPath);
	printf("Config loaded.\n");

	printf("Starting game...\n");

	std::wstring executablePath = Widen(config.game.executable);
	std::wstring processWorkingDirectory = Widen(config.game.workingDirectory);
	std::wstring loaderDllPath = GetLoaderDllPath();

	printf("Executable path: %ls\n", executablePath.c_str());
	printf("Working directory: %ls\n", processWorkingDirectory.c_str());
	printf("Loader DLL: %ls\n", loaderDllPath.c_str());

	Process gameProcess{ executablePath, processWorkingDirectory };
	printf("The game is now running. The ABiT launcher will stop once the game quits.\n");
	printf(
		"Game process handle: %p, thread handle: %p\n", gameProcess.handle, gameProcess.threadHandle
	);

	size_t loaderDllPathSize = loaderDllPath.size() * sizeof(wchar_t);
	void* loaderDllPathInGame = VirtualAllocEx(
		gameProcess.handle, nullptr, loaderDllPathSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
	);
	if (loaderDllPathInGame == nullptr) {
		throw Error::System("cannot allocate memory in the game process");
	}
	Defer freeLoaderDllPathInGame{ [loaderDllPathInGame, &gameProcess]() {
		VirtualFreeEx(gameProcess.handle, loaderDllPathInGame, 0, MEM_RELEASE);
	} };
	printf(
		"Allocated %zu bytes at %p for the loader DLL path\n",
		loaderDllPathSize,
		loaderDllPathInGame
	);

	if (!WriteProcessMemory(
			gameProcess.handle,
			loaderDllPathInGame,
			loaderDllPath.c_str(),
			loaderDllPathSize,
			nullptr
		)) {
		throw Error::System("failed to write loader DLL path to the game process's memory");
	}
	printf("The DLL path is now inside the process's memory.\n");

	HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
	if (kernel32 == nullptr) {
		throw Error::System("failed to get handle to the kernel32 DLL");
	}
	auto* loadLibraryW =
		reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(kernel32, "LoadLibraryW"));
	printf("LoadLibraryW inside kernel32.dll (%p) is at %p\n", kernel32, loadLibraryW);

	HANDLE remoteThread = CreateRemoteThread(
		gameProcess.handle, nullptr, 0, loadLibraryW, loaderDllPathInGame, 0, nullptr
	);
	if (remoteThread == nullptr) {
		throw Error::System("failed to create remote thread");
	}
	SetThreadDescription(remoteThread, L"A Byte in Time bootstrapping thread");
	Defer closeRemoteThread([remoteThread]() { CloseHandle(remoteThread); });
	printf("Successfully created remote thread (handle %p)\n", remoteThread);

	printf("Waiting for remote thread to exit...\n");
	WaitForSingleObject(remoteThread, INFINITE);

	printf("Letting the game run freely now.\n");
	ResumeThread(gameProcess.threadHandle);

	printf("A Byte in Time should now be fully loaded into the game.\n");

	gameProcess.WaitForExit();
}

}

int
main()
{
	try {
		abit::LauncherMain();
		return 0;
	} catch (abit::Error e) {
		printf("ERROR (during abit::LauncherMain): %s\n", e.what.c_str());
		return 1;
	}
}