#pragma once

#include <cstdint>
#include <type_traits>

#include "abit/dll_macros.hpp"

#include "abit/procs/init.hpp"

namespace abit {

struct Symbol
{
	const char* name;
	void* address;

	inline ptrdiff_t GetAddressInMemory() const
	{
		return ptrdiff_t(executableBaseAddress) + ptrdiff_t(address);
	}
};

struct Global : Symbol
{
	template<typename T>
	inline T* As() const
	{
		return reinterpret_cast<T*>(GetAddressInMemory());
	}
};

struct Proc : Symbol
{
	template<typename Ret, typename... Args>
	inline Ret Call(Args... args) const
	{
		using FunctionType = Ret (*)(Args...);
		ptrdiff_t functionAddress = GetAddressInMemory();
		if constexpr (!std::is_void_v<Ret>) {
			return (FunctionType(functionAddress))(args...);
		} else {
			(FunctionType(functionAddress))(args...);
		}
	}
};

}
