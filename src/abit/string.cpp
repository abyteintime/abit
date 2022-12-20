#include "string.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "error.hpp"

std::wstring
abit::Widen(std::string_view string)
{
	if (string.empty()) {
		return L"";
	}

	int size_needed =
		MultiByteToWideChar(CP_UTF8, 0, &string.at(0), static_cast<int>(string.size()), nullptr, 0);
	if (size_needed <= 0) {
		throw Error{ std::string{ "MultiByteToWideChar() failed with error code " } +
					 std::to_string(size_needed) };
	}

	std::wstring result(size_needed, 0);
	MultiByteToWideChar(
		CP_UTF8, 0, &string.at(0), static_cast<int>(string.size()), &result.at(0), size_needed
	);
	return result;
}
