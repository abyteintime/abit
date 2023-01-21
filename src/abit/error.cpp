#include "abit/error.hpp"

#include <system_error>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace abit;

Error::Error(std::string what)
	: message(std::move(what))
{
}

Error
Error::System(std::string_view _what)
{
	int code = GetLastError();
	return Error{ std::string(_what) + ": " + std::system_category().message(code) };
}

const char*
Error::what() const noexcept
{
	return message.c_str();
}
