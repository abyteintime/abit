#include "abit/loader/patches.hpp"

#include "MinHook.h"

#include "abit/error.hpp"

#include "abit/loader/console.hpp"
#include "abit/loader/procs.hpp"

namespace abit {

template<typename F>
static void
CheckMinHookStatus(MH_STATUS status, F&& context)
{
	if (status != MH_OK) {
		throw Error{ std::string(context()) + ": " + MH_StatusToString(status) };
	}
}

static HANDLE executableBaseAddress;

static void*
GetFunctionAddressInExecutable(const Proc& proc)
{
	auto* baseAddress = reinterpret_cast<char*>(Proc::baseAddress);
	auto* procAddress = reinterpret_cast<char*>(proc.address);
	auto* execBaseAddress = reinterpret_cast<char*>(executableBaseAddress);
	return reinterpret_cast<void*>(procAddress - baseAddress + execBaseAddress);
}

}

void
abit::InitializePatches()
{
	MH_STATUS status = MH_Initialize();
	CheckMinHookStatus(status, []() { return "failed to initialize MinHook"; });

	executableBaseAddress = GetModuleHandleW(nullptr);
	if (executableBaseAddress == nullptr) {
		throw Error{ "cannot obtain handle to the running executable" };
	}
}

void
abit::impl::TypeUnsafePatch(const abit::Proc& proc, void* detour, void** original)
{
	void* target = nullptr;

	MH_STATUS status = MH_CreateHook(GetFunctionAddressInExecutable(proc), detour, original);
	CheckMinHookStatus(status, [proc]() {
		return std::string("failed to create hook '") + std::string(proc.name) + "'";
	});

	status = MH_EnableHook(target);
	CheckMinHookStatus(status, [proc]() {
		return std::string("failed to enable hook '") + std::string(proc.name) + "'";
	});
}
