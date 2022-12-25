#pragma once

#include "abit/dll_macros.hpp"

#ifdef AByteInTime_Procs_EXPORTS
#define ABIT_PROCS_API ABIT_DLL_EXPORT
#else
#define ABIT_PROCS_API ABIT_DLL_IMPORT
#endif

namespace abit {

ABIT_PROCS_API extern void* executableBaseAddress;

ABIT_PROCS_API void
InitializeProcs();

}
