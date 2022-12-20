#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "console.hpp"
#include "error.hpp"
#include "patches.hpp"
#include "procs.hpp"
#include "string.hpp"
#include "version.hpp"

namespace abit {

using GuardedMainWrapperType = int __cdecl (*)(wchar_t*, HINSTANCE, HINSTANCE, int);
using GetGameTimeType = float __cdecl (*)();

namespace patches {

GuardedMainWrapperType O_GuardedMainWrapper = nullptr;
GetGameTimeType O_GetGameTime = nullptr;

float __cdecl GetGameTime()
{
	return 4.f * 60.f + 20.f;
}

int __cdecl GuardedMainWrapper(wchar_t* a, HINSTANCE b, HINSTANCE c, int d)
{
	PrintLine(L"GuardedMainWrapper was patched successfully! \\o/");
	PrintLine(L"We're now ready to load native mods.");

	Patch(procs::UHat_GlobalTimer::GetGameTime, &GetGameTime, O_GetGameTime);

	return O_GuardedMainWrapper(a, b, c, d);
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
	Patch(procs::GuardedMainWrapper, &patches::GuardedMainWrapper, patches::O_GuardedMainWrapper);

	PrintLine(L"LoaderInit done.");
}

}

extern "C" bool APIENTRY
DllMain(HINSTANCE dll, DWORD callReason, LPVOID)
{
	if (callReason == DLL_PROCESS_ATTACH) {
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
