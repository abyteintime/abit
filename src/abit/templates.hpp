#pragma once

namespace abit::detail {

template<typename... T>
constexpr bool alwaysFalse = false;

}

namespace abit {

template<typename To, typename From>
static inline To
BitCast(From x)
{
	union
	{
		To to;
		From from;
	} as;
	as.from = x;
	return as.to;
}

}
