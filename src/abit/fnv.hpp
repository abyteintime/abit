#pragma once

#include <cstdint>

namespace abit {

/// 64-bit FNV-1a hash function.
/// http://www.isthe.com/chongo/tech/comp/fnv/index.html
///
/// To compute an FNV-1a hash, create an instance of this struct, and call `Mix` with your 64-bit
/// numbers. After you're done, the hash can be found in the `hash` field.
struct Fnv1a64
{
	/// The final hash value.
	uint64_t hash = 14695981039346656037ULL;

	inline void Mix(uint64_t x)
	{
		hash ^= x;
		hash *= 1099511628211ULL;
	}
};

}