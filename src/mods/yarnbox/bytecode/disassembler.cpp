#include "yarnbox/bytecode/disassembler.hpp"

#include "abit/loader/logging.hpp"

using namespace yarn;

using NodeIndex = BytecodeTree::NodeIndex;
using DataIndex = BytecodeTree::DataIndex;
using Node = BytecodeTree::Node;

Disassembler::Disassembler(const uint8_t* bytecode, size_t length, BytecodeTree& outTree)
	: bytecode(bytecode)
	, length(length)
	, outTree(&outTree)
{
}

NodeIndex
Disassembler::Disassemble()
{
	Opcode opcode = NextOpcode();
	switch (opcode) {
		case Opcode::Stop:
		case Opcode::Nothing:
		case Opcode::Self:
		case Opcode::IntZero:
		case Opcode::IntOne:
		case Opcode::True:
		case Opcode::False:
		case Opcode::EndOfScript:
			return outTree->AppendNode(Node{ ip, opcode });

		case Opcode::ByteConst:
			return outTree->AppendNode(Node{ ip, opcode }.WithU32Pair(NextU8(), 0));

		case Opcode::Jump:
		case Opcode::JumpIfNotEditorOnly:
			return outTree->AppendNode(Node{ ip, opcode }.WithU32Pair(NextU16(), 0));

		case Opcode::IntConst:
		case Opcode::FloatConst:
			return outTree->AppendNode(Node{ ip, opcode }.WithU32Pair(NextU32(), 0));

		case Opcode::LocalVariable:
		case Opcode::InstanceVariable:
		case Opcode::DefaultVariable:
		case Opcode::BoolVariable:
		case Opcode::NativeParm:
		case Opcode::ObjectConst:
		case Opcode::NameConst:
			return outTree->AppendNode(Node{ ip, opcode }.WithU64(NextU64()));

		case Opcode::JumpIfNot: {
			uint16_t address = NextU16();
			NodeIndex condition = Disassemble();
			return outTree->AppendNode(Node{ ip, opcode }.WithU32Pair(address, condition));
		}

		case Opcode::Not_PreBool:
		case Opcode::Complement_PreInt: {
			NodeIndex value = Disassemble();
			return outTree->AppendNode(Node{ ip, opcode }.WithU32Pair(value, 0));
		}

		case Opcode::Let:
		case Opcode::LetBool:
		case Opcode::Multiply_IntInt:
		case Opcode::Divide_IntInt:
		case Opcode::AndAnd_BoolBool:
		case Opcode::OrOr_BoolBool:
		case Opcode::Add_IntInt:
		case Opcode::LessLess_IntInt:
		case Opcode::GreaterGreater_IntInt:
		case Opcode::GreaterGreaterGreater_IntInt:
		case Opcode::Less_IntInt:
		case Opcode::Greater_IntInt:
		case Opcode::LessEqual_IntInt:
		case Opcode::GreaterEqual_IntInt:
		case Opcode::EqualEqual_IntInt:
		case Opcode::NotEqual_IntInt:
		case Opcode::And_IntInt:
		case Opcode::Xor_IntInt:
		case Opcode::Or_IntInt:
		case Opcode::AddEqual_IntInt:
		case Opcode::SubtractEqual_IntInt:
		case Opcode::Percent_IntInt: {
			NodeIndex lvalue = Disassemble();
			NodeIndex rvalue = Disassemble();
			return outTree->AppendNode(Node{ ip, opcode }.WithU32Pair(lvalue, rvalue));
		}

		case Opcode::StringConst: {
			size_t start = ip;
			while (CurrentByte() != '\x00') {
				Advance();
			}
			Advance();
			size_t end = ip;
			const char* data = reinterpret_cast<const char*>(&bytecode[start]);
			std::string ansiString{ std::string_view(data, end - start) };
			DataIndex stringIndex = outTree->AppendString(std::move(ansiString));
			return outTree->AppendNode(Node{ ip, opcode }.WithU64(stringIndex));
		}

		case Opcode::RotationConst:
		case Opcode::VectorConst: {
			auto a = static_cast<uint64_t>(NextU32());
			auto b = static_cast<uint64_t>(NextU32());
			auto c = static_cast<uint64_t>(NextU32());
			DataIndex data = outTree->AppendData(2);
			outTree->Data(data, 0) = (a << 32) | b;
			outTree->Data(data, 1) = c;
			return outTree->AppendNode(Node{ ip, opcode }.WithU64(data));
		}

		case Opcode::OutOfBounds:
			spdlog::warn("Disassembling opcode resulted in an out-of-bounds read");
			return outTree->AppendNode(Node{ ip, Opcode::OutOfBounds });

		default:
			spdlog::warn(
				"Disassembling opcode '{}' ({} at IP={}) is not supported. Remaining bytecode will "
				"be skipped. Patches applied to this chunk may be incomplete",
				OpcodeToString(opcode),
				static_cast<uint32_t>(opcode),
				ip
			);
			return outTree->AppendNode(Node{ ip, Opcode::Unknown });
	}
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
