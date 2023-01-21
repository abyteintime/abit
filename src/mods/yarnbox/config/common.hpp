#pragma once

#include <vector>

#include "sol/sol.hpp"

namespace yarn::config {

template<typename T>
void
CopyTableToVector(const sol::table& table, std::vector<T>& outVector)
{
	size_t count = table.size();
	outVector.clear();
	outVector.reserve(count);
	for (size_t i = 0; i < count; ++i) {
		outVector.push_back(table[i + 1]);
	}
}

}
