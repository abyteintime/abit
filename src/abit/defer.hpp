#pragma once

namespace abit {

template<typename F>
class Defer
{
	F func;

public:
	Defer(F&& func)
	  : func(func)
	{
	}
	~Defer() { func(); }
};

}
