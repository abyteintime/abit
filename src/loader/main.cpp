#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "console.hpp"
#include "detours.hpp"
#include "error.hpp"
#include "string.hpp"
#include "version.hpp"

namespace abit {

using GuardedMainWrapperType = int __cdecl (*)(wchar_t*, HINSTANCE, HINSTANCE, int);

GuardedMainWrapperType O_GuardedMainWrapper = nullptr;

int __cdecl D_GuardedMainWrapper(wchar_t* a, HINSTANCE b, HINSTANCE c, int d)
{
	PrintLine(L"GuardedMainWrapper detour successful! \\o/");
	PrintLine(L"We're now ready to perform early initialization.");

	return O_GuardedMainWrapper(a, b, c, d);
}

void
LoaderInit()
{
	InitializeConsole();
	PrintLine(std::wstring(L"A Byte in Time Loader version ") + VersionWide());

	PrintLine(L"Initializing detour library");
	InitializeDetours();

	PrintLine(L"Detouring GuardedMainWrapper");
	Detour(
		"?GuardedMainWrapper@@YAHPEB_WPEAUHINSTANCE__@@1H@Z",
		&D_GuardedMainWrapper,
		O_GuardedMainWrapper
	);
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
