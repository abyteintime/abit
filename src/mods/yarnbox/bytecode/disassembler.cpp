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
	size_t ipAtStart = ip;
	Opcode opcode = NextOpcode();
	Node node = Node{ ipAtStart, opcode };
	switch (opcode) {
		// -
		case Opcode::Stop:
		case Opcode::Nothing:
		case Opcode::Self:
		case Opcode::IntZero:
		case Opcode::IntOne:
		case Opcode::True:
		case Opcode::False:
		case Opcode::InterfaceCast:
		case Opcode::EndOfScript:
			return outTree->AppendNode(node);

		// u8
		case Opcode::ByteConst:
		case Opcode::PrimitiveCast:
			return outTree->AppendNode(node.WithU32Pair(NextU8(), 0));

		// u16
		case Opcode::Jump:
		case Opcode::JumpIfNotEditorOnly:
			return outTree->AppendNode(node.WithU32Pair(NextU16(), 0));

		// u32
		case Opcode::IntConst:
		case Opcode::FloatConst:
			return outTree->AppendNode(node.WithU32Pair(NextU32(), 0));

		// u64
		case Opcode::LocalVariable:
		case Opcode::InstanceVariable:
		case Opcode::DefaultVariable:
		case Opcode::BoolVariable:
		case Opcode::NativeParm:
		case Opcode::ObjectConst:
		case Opcode::NameConst:
			return outTree->AppendNode(node.WithU64(NextU64()));

		// u16 insn
		case Opcode::JumpIfNot: {
			uint16_t address = NextU16();
			NodeIndex condition = Disassemble();
			return outTree->AppendNode(node.WithU32Pair(address, condition));
		}

		// u64 insn
		case Opcode::MetaCast:
		case Opcode::DynamicCast: {
			DataIndex data = outTree->AppendData(2);
			outTree->Data(data, 0) = NextU64();
			outTree->Data(data, 1) = Disassemble();
			return outTree->AppendNode(node.WithU64(data));
		}

		// insn
		case Opcode::DynArrayLength: {
			NodeIndex value = Disassemble();
			return outTree->AppendNode(node.WithU32Pair(value, 0));
		}

		// insn insn
		case Opcode::Let:
		case Opcode::DynArrayElement:
		case Opcode::LetBool: {
			NodeIndex lvalue = Disassemble();
			NodeIndex rvalue = Disassemble();
			return outTree->AppendNode(node.WithU32Pair(lvalue, rvalue));
		}

		// insn DebugInfo?
		case Opcode::Not_PreBool:
		case Opcode::Complement_PreInt:
		case Opcode::Subtract_PreInt:
		case Opcode::AddAdd_PreInt:
		case Opcode::SubtractSubtract_PreInt:
		case Opcode::AddAdd_Int:
		case Opcode::SubtractSubtract_Int:
		case Opcode::Subtract_PreFloat: {
			NodeIndex value = Disassemble();
			OptionalDebugInfo();
			return outTree->AppendNode(node.WithU32Pair(value, 0));
		}

		// insn insn DebugInfo?
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
		case Opcode::Percent_IntInt:
		case Opcode::MultiplyMultiply_FloatFloat:
		case Opcode::Multiply_FloatFloat:
		case Opcode::Divide_FloatFloat:
		case Opcode::Percent_FloatFloat:
		case Opcode::Add_FloatFloat:
		case Opcode::Subtract_FloatFloat:
		case Opcode::Less_FloatFloat:
		case Opcode::Greater_FloatFloat:
		case Opcode::LessEqual_FloatFloat:
		case Opcode::GreaterEqual_FloatFloat:
		case Opcode::EqualEqual_FloatFloat:
		case Opcode::NotEqual_FloatFloat:
		case Opcode::MultiplyEqual_FloatFloat:
		case Opcode::DivideEqual_FloatFloat:
		case Opcode::AddEqual_FloatFloat:
		case Opcode::SubtractEqual_FloatFloat:
		case Opcode::ComplementEqual_FloatFloat: {
			NodeIndex lvalue = Disassemble();
			NodeIndex rvalue = Disassemble();
			OptionalDebugInfo();
			return outTree->AppendNode(node.WithU32Pair(lvalue, rvalue));
		}

		// {1 .. 255}* 0
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
			return outTree->AppendNode(node.WithU64(stringIndex));
		}

		// u32 u32 u32
		case Opcode::RotationConst:
		case Opcode::VectorConst: {
			auto a = static_cast<uint64_t>(NextU32());
			auto b = static_cast<uint64_t>(NextU32());
			auto c = static_cast<uint64_t>(NextU32());
			DataIndex data = outTree->AppendData(2);
			outTree->Data(data, 0) = (a << 32) | b;
			outTree->Data(data, 1) = c;
			return outTree->AppendNode(node.WithU64(data));
		}

		// insn insn insn DebugInfo?
		case Opcode::DynArrayInsert:
		case Opcode::DynArrayRemove: {
			DataIndex data = outTree->AppendData(3);
			outTree->Data(data, 0) = Disassemble();
			outTree->Data(data, 1) = Disassemble();
			outTree->Data(data, 2) = Disassemble();
			OptionalDebugInfo();
			return outTree->AppendNode(node.WithU64(data));
		}

		// insn u16 insn u8 DebugInfo?
		case Opcode::DynArrayAddItem:
		case Opcode::DynArrayRemoveItem: {
			DataIndex data = outTree->AppendData(3);
			outTree->Data(data, 0) = Disassemble();
			outTree->Data(data, 1) = NextU16();
			outTree->Data(data, 2) = Disassemble();
			NextU8();
			OptionalDebugInfo();
			return outTree->AppendNode(node.WithU64(data));
		}

		// insn u16 insn insn u8 DebugInfo?
		case Opcode::DynArrayInsertItem: {
			DataIndex data = outTree->AppendData(4);
			outTree->Data(data, 0) = Disassemble();
			outTree->Data(data, 1) = NextU16();
			outTree->Data(data, 2) = Disassemble();
			outTree->Data(data, 3) = Disassemble();
			NextU8();
			OptionalDebugInfo();
			return outTree->AppendNode(node.WithU64(data));
		}

		case Opcode::OutOfBounds:
			spdlog::warn("Disassembling opcode resulted in an out-of-bounds read");
			return outTree->AppendNode(Node{ ipAtStart, Opcode::OutOfBounds });

		default:
			spdlog::warn(
				"Disassembling opcode '{}' ({} at IP={}) is not supported. Remaining bytecode will "
				"be skipped. This function will not be patchable",
				OpcodeToString(opcode),
				static_cast<uint32_t>(opcode),
				ipAtStart
			);
			return outTree->AppendNode(Node{ ipAtStart, Opcode::Unknown });
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
