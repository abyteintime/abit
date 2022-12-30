#include "yarnbox/bytecode/disassembler.hpp"

#include "abit/loader/logging.hpp"

using namespace yarn;

Disassembler::Disassembler(const uint8_t* bytecode, size_t length, BytecodeTree& outTree)
	: bytecode(bytecode)
	, length(length)
	, outTree(&outTree)
{
}

BytecodeTree::NodeIndex
Disassembler::Disassemble()
{
	Opcode opcode = NextOpcode();
	switch (opcode) {
		default:
			spdlog::warn(
				"Disassembling opcode {} is not supported. Remaining bytecode will be skipped. "
				"Patches applied to this chunk may be incomplete",
				OpcodeToString(opcode)
			);
			return outTree->AppendNode({ ip, Opcode::Unknown });
	}
}

bool
Disassembler::AtEnd() const
{
	return ip >= length;
}

uint8_t
Disassembler::CurrentByte() const
{
	return bytecode[ip];
}

void
Disassembler::Advance(size_t by)
{
	ip += by;
}

Opcode
Disassembler::CurrentLowOpcode() const
{
	return static_cast<Opcode>(CurrentByte());
}

Opcode
Disassembler::NextOpcode()
{
	switch (CurrentLowOpcode()) {
		case Opcode::HighNative0:
		case Opcode::HighNative1:
		case Opcode::HighNative2:
		case Opcode::HighNative3:
		case Opcode::HighNative4:
		case Opcode::HighNative5:
		case Opcode::HighNative6:
		case Opcode::HighNative7:
		case Opcode::HighNative8:
		case Opcode::HighNative9:
		case Opcode::HighNative10:
		case Opcode::HighNative11:
		case Opcode::HighNative12:
		case Opcode::HighNative13:
		case Opcode::HighNative14:
		case Opcode::HighNative15: {
			uint32_t level =
				static_cast<uint32_t>(CurrentByte() - static_cast<uint8_t>(Opcode::HighNative0));
			Advance();
			uint8_t offset = CurrentByte();
			Advance();

			return static_cast<Opcode>(level * 256 + offset);
		}

		default: {
			Opcode opcode = CurrentLowOpcode();
			Advance();

			return opcode;
		}
	}
}
