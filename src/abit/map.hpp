#pragma once

#include <string>
#include <unordered_map>

namespace abit {

struct AsciiCaseInsensitiveHash
{
	size_t operator()(const std::string& s) const;
};

struct AsciiCaseInsensitiveEqual
{
	bool operator()(const std::string& s, const std::string& t) const;
};

template<typename V>
using AsciiCaseInsensitiveMap
	= std::unordered_map<std::string, V, AsciiCaseInsensitiveHash, AsciiCaseInsensitiveEqual>;

}
