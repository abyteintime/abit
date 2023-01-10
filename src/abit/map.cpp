#include "abit/map.hpp"

#include "abit/string.hpp"

using namespace abit;

size_t
AsciiCaseInsensitiveHash::operator()(const std::string& s) const
{
	// FNV-1a hash function.
	// http://www.isthe.com/chongo/tech/comp/fnv/index.html
	size_t h = 14695981039346656037ULL;
	for (char ch : s) {
		size_t c = static_cast<size_t>(CharToLowerAscii(ch));
		h ^= c;
		h *= 1099511628211ULL;
	}
	return h;
}

bool
AsciiCaseInsensitiveEqual::operator()(const std::string& s, const std::string& t) const
{
	if (s.size() != t.size()) {
		return false;
	}

	for (size_t i = 0; i < s.size(); ++i) {
		if (CharToLowerAscii(s[i]) != CharToLowerAscii(t[i])) {
			return false;
		}
	}
	return true;
}