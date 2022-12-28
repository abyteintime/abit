#include "abit/loader/patches.hpp"

#include "MinHook.h"

#include "abit/error.hpp"

#include "abit/loader/logging.hpp"

namespace abit {

template<typename F>
static void
CheckMinHookStatus(MH_STATUS status, F&& context)
{
	if (status != MH_OK) {
		throw Error{ std::string(context()) + ": " + MH_StatusToString(status) };
	}
}

static void*
GetFunctionAddressInExecutable(const Proc& proc)
{
	auto execBaseAddress = reinterpret_cast<ptrdiff_t>(executableBaseAddress);
	auto procAddress = reinterpret_cast<ptrdiff_t>(proc.address);
	return reinterpret_cast<void*>(execBaseAddress + procAddress);
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
