#include "abit/loader/mod.hpp"

#include "abit/error.hpp"

using namespace abit;

Mod::Mod(const std::filesystem::path& dllPath)
{
	std::wstring wdllPath = dllPath.wstring();

	dll = LoadLibraryW(wdllPath.c_str());
	if (dll == nullptr) {
		throw Error::System("failed to load mod DLL");
	}

	modInit = reinterpret_cast<ModInitProc>(GetProcAddress(dll, "ABiT_ModInit"));
	if (modInit == nullptr) {
		throw Error{
			"mod is missing ModInit entry point (the function should be called ABiT_ModInit)"
		};
	}
}

Mod::~Mod()
{
	if (dll != nullptr) {
		FreeLibrary(dll);
	}
}

Mod::Mod(Mod&& other)
{
	dll = other.dll;
	other.dll = nullptr;
}

void
Mod::CallModInit()
{
	(modInit)();
}
