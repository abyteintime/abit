#pragma once

#if defined(__clang__)
#define ABIT_DLL_EXPORT [[gnu::dllexport]]
#define ABIT_DLL_IMPORT [[gnu::dllimport]]
#elif defined(_MSC_VER)
#define ABIT_DLL_EXPORT __declspec(dllexport)
#define ABIT_DLL_IMPORT __declspec(dllimport)
#else
#error "Unsupported compiler"
#endif
