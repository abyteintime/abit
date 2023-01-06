#pragma once

#include <initializer_list>
#include <utility>

#include "yarnbox/bytecode/opcode.hpp"

namespace yarn {

namespace primitive {

enum Type : uint8_t
{
	PUnsupported, // Not specified (has no data)
	PEmpty,       // No operands (has no data)
	PU8,          // u8 operand
	PU16,         // u16 operand
	PU32,         // u32 operand
	PU64,         // u64 operand
	PInsn,        // single instruction operand
	PInsns,       // multi-instruction operand terminated by a sentinel - the argument
	PDebugInfo,   // optional DebugInfo instruction
	PAnsiString,  // NUL-terminated ANSI string
	PSentinel,    // u8 value (has no data)
};

/// Integer presentation used when dumping bytecode to the user.
/// This is the argument to PU* primitives.
enum IntKind : uint8_t
{
	KUnsigned = 0, // Unsigned
	KSigned,       // Signed
	KFloat,        // Floating-point
	KPointer,      // Pointer
	KObject,       // UObject pointer
	KOffset,       // IP offset: signed, printed as hex
};

bool
HasDataInBytecodeTree(Type t);

}

struct Primitive
{
	primitive::Type type = primitive::PUnsupported;
	uint8_t arg = 0;

	constexpr Primitive() {}

	constexpr Primitive(primitive::Type type)
		: type(type)
	{
	}

	constexpr Primitive(primitive::Type type, uint8_t arg)
		: type(type)
		, arg(arg)
	{
	}

	constexpr Primitive(primitive::Type type, Opcode arg)
		: type(type)
		, arg(static_cast<uint8_t>(arg))
	{
	}

	constexpr Primitive(primitive::Type type, primitive::IntKind arg)
		: type(type)
		, arg(static_cast<uint8_t>(arg))
	{
	}
};

namespace primitive {

constexpr Primitive PS8 = { PU8, KSigned };
constexpr Primitive PS16 = { PU16, KSigned };
constexpr Primitive PS32 = { PU32, KSigned };
constexpr Primitive PS64 = { PU64, KSigned };
constexpr Primitive PFloat = { PU32, KFloat };
constexpr Primitive PPtr = { PU64, KPointer };
constexpr Primitive PObj = { PU64, KObject };
constexpr Primitive POffset = { PU16, KOffset };

}

struct Rule
{
	Primitive base = primitive::PUnsupported;
	bool useData = false;
	Primitive data[8] = {
		primitive::PEmpty, primitive::PEmpty, primitive::PEmpty, primitive::PEmpty,
		primitive::PEmpty, primitive::PEmpty, primitive::PEmpty, primitive::PEmpty,
	};
	size_t dataCount = 0;

	constexpr Rule() {}

	template<typename... Prims>
	constexpr Rule(Prims... prims)
	{
		if (sizeof...(prims) == 1) {
			((base = prims), ...);
		} else {
			useData = true;
			size_t i = 0;
			((data[i++] = prims), ...);
			dataCount = sizeof...(prims);
		}
	}

	inline bool IsUnsupported() const { return !useData && base.type == primitive::PUnsupported; }
};

struct Encoding
{
	Rule opcodes[opcodeCount];

	constexpr Rule& Rule(Opcode opcode) { return opcodes[static_cast<size_t>(opcode)]; }
	constexpr const struct Rule& Rule(Opcode opcode) const
	{
		return opcodes[static_cast<size_t>(opcode)];
	}
};

extern Encoding encoding;

}
