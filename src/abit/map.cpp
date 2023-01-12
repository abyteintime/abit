#include "abit/map.hpp"

#include "abit/fnv.hpp"
#include "abit/string.hpp"

using namespace abit;

size_t
AsciiCaseInsensitiveHash::operator()(const std::string& s) const
{
	Fnv1a64 hasher;
	for (char ch : s) {
		uint64_t c = static_cast<uint64_t>(CharToLowerAscii(ch));
		hasher.Mix(c);
	}
	return static_cast<size_t>(hasher.hash);
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