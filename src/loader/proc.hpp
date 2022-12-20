#pragma once

namespace abit {

struct Proc
{
	static void* const baseAddress;

	const char* name;
	void* address;
};

}