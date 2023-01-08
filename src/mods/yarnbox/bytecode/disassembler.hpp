#pragma once

#include <memory>
#include <optional>
#include <utility>

#include "yarnbox/bytecode/encoding.hpp"
#include "yarnbox/bytecode/opcode.hpp"
#include "yarnbox/bytecode/tree.hpp"

namespace yarn {

struct Disassembler
{
	struct Stats
	{
		uint32_t occurrencesOfUnknownOpcodes[opcodeCount] = { 0 };
		size_t outOfBoundsReads = 0;

		struct Summary
		{
			std::vector<std::pair<Opcode, uint32_t>> occurrencesOfUnknownOpcodes;
			size_t totalOccurrencesOfUnknownOpcodes = 0;
		};

		Summary ComputeSummary();
	};

	const uint8_t* bytecode;
	size_t length;
	BytecodeTree* outTree;

	size_t ip = 0;
	Stats* outStats = nullptr;

	Disassembler(const uint8_t* bytecode, size_t length, BytecodeTree& outTree);

	void EnableStatCollection(Stats& outStats);

	BytecodeTree::NodeIndex Disassemble();
	bool AtEnd() const { return ip >= length; }

	bool ShouldStopDisassembling() { return outTree->IsBytecodeBogusAt(ip); }

private:
	BytecodeTree::NodeIndex DisassembleOpcode(size_t ipAtStart, Opcode opcode);

	inline uint8_t CurrentByte() const { return AtEnd() ? 0 : bytecode[ip]; }
	inline void Advance(ptrdiff_t by = 1) { ip += by; }

	inline Opcode CurrentLowOpcode() const
	{
		return AtEnd() ? Opcode::OutOfBounds : static_cast<Opcode>(CurrentByte());
	}
	Opcode NextOpcode();

	inline uint8_t NextU8()
	{
		uint8_t b = CurrentByte();
		Advance();
		return b;
	}

	inline uint16_t NextU16()
	{
		auto b = static_cast<uint16_t>(NextU8());
		auto a = static_cast<uint16_t>(NextU8());
		return (a << 8) | b;
	}

	inline uint32_t NextU32()
	{
		auto d = static_cast<uint32_t>(NextU8());
		auto c = static_cast<uint32_t>(NextU8());
		auto b = static_cast<uint32_t>(NextU8());
		auto a = static_cast<uint32_t>(NextU8());
		return (a << 24) | (b << 16) | (c << 8) | d;
	}

	inline uint64_t NextU64()
	{
		auto h = static_cast<uint64_t>(NextU8());
		auto g = static_cast<uint64_t>(NextU8());
		auto f = static_cast<uint64_t>(NextU8());
		auto e = static_cast<uint64_t>(NextU8());
		auto d = static_cast<uint64_t>(NextU8());
		auto c = static_cast<uint64_t>(NextU8());
		auto b = static_cast<uint64_t>(NextU8());
		auto a = static_cast<uint64_t>(NextU8());
		return (a << 56) | (b << 48) | (c << 40) | (d << 32) | (e << 24) | (f << 16) | (g << 8) | h;
	}

	inline uint64_t NextPtr() { return NextU64(); }

	inline void OptionalDebugInfo()
	{
		if (CurrentLowOpcode() == Opcode::DebugInfo) {
			NextU8();
			if (CurrentByte() == 100) {
				// DebugInfo is not supported.
				__debugbreak();
			}
			Advance(-1);
		}
	}

	std::optional<uint64_t>
	InterpretPrimitive(Opcode contextOpcode, size_t contextIp, Primitive primitive);
};

}
