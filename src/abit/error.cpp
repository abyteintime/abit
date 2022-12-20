#include "error.hpp"

#include <system_error>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace abit;

Error
Error::System(std::string_view _what)
{
	int code = GetLastError();
	return Error{ std::string(_what) + ": " + std::system_category().message(code) };
}
