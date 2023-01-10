#pragma once

#include <string>
#include <unordered_map>

#include "abit.hpp"

namespace abit {

struct ABIT_API AsciiCaseInsensitiveHash
{
	size_t operator()(const std::string& s) const;
};

struct ABIT_API AsciiCaseInsensitiveEqual
{
	bool operator()(const std::string& s, const std::string& t) const;
};

template<typename V>
using AsciiCaseInsensitiveMap =
	std::unordered_map<std::string, V, AsciiCaseInsensitiveHash, AsciiCaseInsensitiveEqual>;

}
