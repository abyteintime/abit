#include <atomic>
#include <filesystem>
#include <utility>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "abit/error.hpp"
#include "abit/paths.hpp"
#include "abit/string.hpp"
#include "abit/version.hpp"

#include "abit/loader/console.hpp"
#include "abit/loader/mod.hpp"
#include "abit/loader/patches.hpp"
#include "abit/loader/procs.hpp"

namespace abit {

HINSTANCE loaderDllHandle;

std::vector<Mod> loadedMods;

void
ModsInit()
{
	std::wstring loaderDllPathString = abit::GetExecutablePath(loaderDllHandle);
	std::filesystem::path loaderDllPath{ loaderDllPathString };
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
			std::wstring modName = path.stem().wstring();
			PrintLine(std::wstring(L"Loading ") + modName);
			std::wstring absoluteDllPath = std::filesystem::absolute(path).wstring();
			try {
				Mod mod{ absoluteDllPath };
				mod.CallModInit();
				loadedMods.push_back(std::move(mod));
			} catch (abit::Error error) {
				PrintLine(
					std::wstring(L"ERROR: Mod ") + modName + L" failed to load: " +
					abit::Widen(error.what)
				);
			}
		}
	}
}

namespace patches {

using GuardedMainWrapperType = int __cdecl (*)(wchar_t*, HINSTANCE, HINSTANCE, int);
GuardedMainWrapperType originalGuardedMainWrapper = nullptr;
int __cdecl GuardedMainWrapper(wchar_t* a, HINSTANCE b, HINSTANCE c, int d)
{
	PrintLine(L"GuardedMainWrapper was patched successfully! \\o/");
	PrintLine(L"We're now ready to load native mods.");

	try {
		ModsInit();
	} catch (abit::Error error) {
		PrintLine(std::wstring(L"ERROR (in abit::ModsInit): ") + abit::Widen(error.what));
		PrintLine(L"NOTE: Due to this error, native mod functionality may be partially or "
				  L"completely unavailable.");
	}

	return originalGuardedMainWrapper(a, b, c, d);
}

}

void
LoaderInit()
{
	InitializeConsole();
	PrintLine(std::wstring(L"A Byte in Time Loader version ") + versionWide);

	PrintLine(L"Initializing patching library");
	InitializePatches();

	PrintLine(L"Patching GuardedMainWrapper");
	Patch(
		procs::GuardedMainWrapper, &patches::GuardedMainWrapper, patches::originalGuardedMainWrapper
	);

	PrintLine(L"LoaderInit done.");
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
			std::wstring message =
				std::wstring(L"ERROR (during abit::LoaderInit): ") + abit::Widen(error.what);
			abit::PrintLine(message);
			return false;
		}
	}
	return true;
}
