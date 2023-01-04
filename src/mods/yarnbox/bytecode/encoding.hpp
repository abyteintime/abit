#pragma once

#include <initializer_list>
#include <utility>

#include "yarnbox/bytecode/opcode.hpp"

namespace yarn {

namespace primitive {

enum Type : uint8_t
{
	PUnsupported,
	PEmpty,
	PU8,
	PU16,
	PU32,
	PU64,
	PInsn,
	PDebugInfo,
	PAnsiString,
	PSentinel,
};

enum DataT
{
	Data,
};

}

struct Rule
{
	primitive::Type base = primitive::PUnsupported;
	bool useData = false;
	primitive::Type data[8] = {
		primitive::PEmpty, primitive::PEmpty, primitive::PEmpty, primitive::PEmpty,
		primitive::PEmpty, primitive::PEmpty, primitive::PEmpty, primitive::PEmpty,
	};
	size_t dataCount = 0;

	constexpr Rule() {}

	constexpr Rule(primitive::Type base)
		: base(base)
	{
	}

	constexpr Rule(primitive::DataT, std::initializer_list<primitive::Type> data_)
		: useData(true)
	{
		size_t i = 0;
		for (primitive::Type type : data_) {
			data[i] = type;
		}
		dataCount = data_.size();
	}

	inline bool IsUnsupported() const { return !useData && base == primitive::PUnsupported; }
};

struct Encoding
{
	Rule opcodes[opcodeCount];

	template<typename... Ts>
	constexpr void Rule(Opcode opcode, primitive::Type base, Ts... data)
	{
		if constexpr (sizeof...(data) > 0) {
			opcodes[static_cast<size_t>(opcode)] = { primitive::Data, { data... } };
		} else {
			opcodes[static_cast<size_t>(opcode)] = { base };
		}
	}
};

extern Encoding encoding;

}
