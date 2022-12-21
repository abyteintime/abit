#include "abit/dll_macros.hpp"

#ifdef AByteInTime_Common_EXPORTS
#define ABIT_API ABIT_DLL_EXPORT
#else
#define ABIT_API ABIT_DLL_IMPORT
#endif
