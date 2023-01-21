#include "abit/string.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "abit/error.hpp"

using namespace abit;

std::wstring
abit::Widen(std::string_view string)
{
	if (string.empty()) {
		return L"";
	}

	int sizeNeeded = MultiByteToWideChar(
		CP_UTF8, 0, &string.at(0), static_cast<int>(string.size()), nullptr, 0
	);
	if (sizeNeeded <= 0) {
		throw Error{ std::string{ "MultiByteToWideChar() failed with error code " }
					 + std::to_string(sizeNeeded) };
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
		throw Error{ std::string{ "MultiByteToWideChar() failed with error code " }
					 + std::to_string(sizeNeeded) };
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

char
abit::CharToLowerAscii(char c)
{
	if (c >= 'A' && c <= 'Z') {
		return c + 0x20;
	} else {
		return c;
	}
}

char
abit::CharToUpperAscii(char c)
{
	if (c >= 'a' && c <= 'z') {
		return c - 0x20;
	} else {
		return c;
	}
}

std::string
abit::StringToLowerAscii(std::string_view string)
{
	std::string result;
	result.reserve(string.size());
	for (char c : string) {
		result += CharToLowerAscii(c);
	}
	return result;
}

std::string
abit::StringToUpperAscii(std::string_view string)
{
	std::string result;
	result.reserve(string.size());
	for (char c : string) {
		result += CharToUpperAscii(c);
	}
	return result;
}

bool
abit::StartsWithCaseInsensitive(std::wstring_view string, std::wstring_view prefix)
{
	if (string.size() < prefix.size()) {
		return false;
	}
	for (size_t i = 0; i < prefix.size(); ++i) {
		if (CharToLowerAscii(string[i]) != CharToLowerAscii(prefix[i])) {
			return false;
		}
	}
	return true;
}
