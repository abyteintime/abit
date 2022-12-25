#include "abit/string.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "abit/error.hpp"

std::wstring
abit::Widen(std::string_view string)
{
	if (string.empty()) {
		return L"";
	}

	int sizeNeeded =
		MultiByteToWideChar(CP_UTF8, 0, &string.at(0), static_cast<int>(string.size()), nullptr, 0);
	if (sizeNeeded <= 0) {
		throw Error{ std::string{ "MultiByteToWideChar() failed with error code " } +
					 std::to_string(sizeNeeded) };
	}

	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(
		CP_UTF8, 0, &string.at(0), static_cast<int>(string.size()), &result.at(0), sizeNeeded
	);
	return result;
}

std::string
abit::Narrow(std::wstring_view string)
{
	if (string.empty()) {
		return "";
	}

	int sizeNeeded = WideCharToMultiByte(
		CP_UTF8, 0, &string.at(0), static_cast<int>(string.size()), nullptr, 0, nullptr, nullptr
	);
	if (sizeNeeded <= 0) {
		throw Error{ std::string{ "MultiByteToWideChar() failed with error code " } +
					 std::to_string(sizeNeeded) };
	}

	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(
		CP_UTF8,
		0,
		&string.at(0),
		static_cast<int>(string.size()),
		&result.at(0),
		sizeNeeded,
		nullptr,
		nullptr
	);
	return result;
}
