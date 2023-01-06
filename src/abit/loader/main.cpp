#include <atomic>
#include <filesystem>
#include <utility>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "abit/config.hpp"
#include "abit/error.hpp"
#include "abit/paths.hpp"
#include "abit/string.hpp"
#include "abit/version.hpp"

#include "abit/loader/logging.hpp"
#include "abit/loader/logging_init.hpp"
#include "abit/loader/mod.hpp"
#include "abit/loader/patches.hpp"

#include "abit/procs/global.hpp"
#include "abit/procs/init.hpp"

#include "spdlog/sinks/sink.h"

namespace abit {

HINSTANCE loaderDllHandle;

Config config;
std::vector<Mod> loadedMods;

void
ModsInit()
{
	std::filesystem::path loaderDllPath = abit::GetExecutablePath(loaderDllHandle);
	std::filesystem::path nativeModsDirectory = loaderDllPath.parent_path().append("NativeMods");

	if (!std::filesystem::is_directory(nativeModsDirectory)) {
		throw Error{ std::string("NativeMods directory does not exist, so there's nowhere to load "
								 "mods from (looked for path ") +
					 nativeModsDirectory.string() + ")" };
	}

	for (const std::filesystem::directory_entry directoryEntry :
		 std::filesystem::directory_iterator{ nativeModsDirectory }) {

		const std::filesystem::path& path = directoryEntry.path();
		if (path.extension() == L".dll") {
			std::string modName = abit::Narrow(path.stem().wstring());
			if (!config.mods.disable.count(modName)) {
				spdlog::info("Loading {}", modName);
				std::wstring absoluteDllPath = std::filesystem::absolute(path).wstring();
				try {
					Mod mod{ absoluteDllPath };
					mod.CallModInit();
					loadedMods.push_back(std::move(mod));
				} catch (abit::Error error) {
					spdlog::error("Mod {} failed to load: {}", modName, error.what);
				}
			} else {
				spdlog::info("Loading {} SKIPPED (mod is disabled)", modName);
			}
		}
	}
}

void
ModsDeinit()
{
	loadedMods.clear();
}

namespace patches {

using GuardedMainWrapperType = int(__cdecl*)(wchar_t*, HINSTANCE, HINSTANCE, int);
GuardedMainWrapperType O_GuardedMainWrapper = nullptr;
int __cdecl GuardedMainWrapper(wchar_t* a, HINSTANCE b, HINSTANCE c, int d)
{
	spdlog::debug("GuardedMainWrapper was patched successfully! \\o/");

	spdlog::debug("We're now ready to load native mods.");

	try {
		ModsInit();
	} catch (abit::Error error) {
		spdlog::error("in abit::ModsInit: {}", error.what);
		spdlog::error("NOTE: Due to this error, native mod functionality may be partially or "
					  "completely unavailable.");
	}

	int result = O_GuardedMainWrapper(a, b, c, d);

	try {
		ModsDeinit();
	} catch (abit::Error error) {
		spdlog::error("in abit::ModsDeinit: {}", error.what);
	}
	abit::DeinitializeLogging();

	return result;
}

}

void
LoaderInit()
{
	InitializeLogging();
	spdlog::info("A Byte in Time Loader version {}", version);

	std::filesystem::path configPath = GetConfigPath(loaderDllHandle);
	spdlog::info(L"Config file: {}", configPath.wstring());
	config = Config::Load(configPath);

	if (config.debug.waitForDebugger) {
		spdlog::info("Waiting for debugger to attach...");
		uint32_t ticksWaited = 0;
		while (!IsDebuggerPresent()) {
			Sleep(250);
			ticksWaited += 1;
			if (ticksWaited == 40) {
				// If the user takes more than 10 seconds, let them know that we're not frozen.
				spdlog::info("Still waiting...");
			} else if (ticksWaited == 60) {
				spdlog::info(
					"NOTE: If you're confused by this, check your ByteinTime.ini. "
					"You may have accidentally set the [Debug] WaitForDebugger key to true."
				);
			}
		}
	}

	spdlog::set_level(spdlog::level::from_str(config.log.level));
	GetFileSink()->set_level(spdlog::level::from_str(config.log.fileLevel));
	GetConsoleSink()->set_level(spdlog::level::from_str(config.log.consoleLevel));

	spdlog::debug("Initializing procs library");
	InitializeProcs();

	spdlog::debug("Initializing patching library");
	InitializePatches();

	spdlog::debug("Patching GuardedMainWrapper");
	Patch(
		procs::global::GuardedMainWrapper,
		&patches::GuardedMainWrapper,
		patches::O_GuardedMainWrapper
	);

	spdlog::debug("LoaderInit done.");
}

}

static std::atomic_bool initialized = false;

extern "C" bool APIENTRY
DllMain(HINSTANCE inDll, DWORD callReason, LPVOID)
{
	if (callReason == DLL_PROCESS_ATTACH && !initialized.exchange(true)) {
		abit::loaderDllHandle = inDll;
		try {
			abit::LoaderInit();
		} catch (abit::Error error) {
			spdlog::error("in abit::LoaderInit: {}", error.what);
			return false;
		}
	}
	return true;
}
