#pragma once

#include "yarnbox/bytecode/opcode.hpp"
#include "yarnbox/bytecode/tree.hpp"

namespace yarn {

struct Disassembler
{
	const uint8_t* bytecode;
	size_t length;
	BytecodeTree* outTree;

	size_t ip = 0;

	Disassembler(const uint8_t* bytecode, size_t length, BytecodeTree& outTree);

	BytecodeTree::NodeIndex Disassemble();
	bool AtEnd() const;

private:
	uint8_t CurrentByte() const;
	void Advance(size_t by = 1);

	Opcode CurrentLowOpcode() const;
	Opcode NextOpcode();
};

}
