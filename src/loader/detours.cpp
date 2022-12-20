#include "detours.hpp"

#include "MinHook.h"

#include "error.hpp"

namespace abit {

template<typename F>
static void
CheckMinHookStatus(MH_STATUS status, F&& context)
{
	if (status != MH_OK) {
		throw Error{ std::string(context()) + ": " + MH_StatusToString(status) };
	}
}

}

auto
abit::InitializeDetours() -> void
{
	MH_STATUS status = MH_Initialize();
	CheckMinHookStatus(status, []() { return "failed to initialize MinHook"; });
}

auto
abit::impl::TypeUnsafeDetour(std::string_view name, void* detour, void** original) -> void
{
	void* target = nullptr;

	MH_STATUS status = MH_CreateHookApiEx(nullptr, name.data(), detour, original, &target);
	CheckMinHookStatus(status, [name]() {
		return std::string("failed to create hook '") + std::string(name) + "'";
	});

	status = MH_EnableHook(target);
	CheckMinHookStatus(status, [name]() {
		return std::string("failed to enable hook '") + std::string(name) + "'";
	});
}
