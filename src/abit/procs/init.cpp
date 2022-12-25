#include "abit/procs/init.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void* abit::executableBaseAddress;

void
abit::InitializeProcs()
{
	executableBaseAddress = GetModuleHandleW(nullptr);
}
