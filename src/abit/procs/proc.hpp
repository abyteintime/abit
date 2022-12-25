#pragma once

#include <cstdint>
#include <type_traits>

#include "abit/procs/init.hpp"
#include "abit/templates.hpp"

namespace abit {

struct CallingConvention
{};

struct Cdecl : CallingConvention
{};
struct ThisCall : CallingConvention
{};

template<typename... Conv>
struct ProcPointer
{
	static_assert(detail::alwaysFalse<Conv...>, "type passed in is not a valid calling convention");
};

template<typename Ret, typename... Args>
struct ProcPointer<Cdecl, Ret, Args...>
{
	using Type = Ret(__cdecl*)(Args...);
};

template<typename Ret, typename... Args>
struct ProcPointer<ThisCall, Ret, Args...>
{
	using Type = Ret(__thiscall*)(Args...);
};

struct Proc
{
	const char* name;
	void* address;

	inline ptrdiff_t GetAddressInMemory()
	{
		return ptrdiff_t(executableBaseAddress) + ptrdiff_t(address);
	}

	template<typename CallConv, typename Ret, typename... Args>
	inline Ret Call(Args... args)
	{
		using FunctionType = typename ProcPointer<CallConv, Ret, Args...>::Type;
		ptrdiff_t functionAddress = GetAddressInMemory();
		if constexpr (!std::is_void_v<Ret>) {
			return (FunctionType(functionAddress))(args...);
		} else {
			(FunctionType(functionAddress))(args...);
		}
	}
};

}