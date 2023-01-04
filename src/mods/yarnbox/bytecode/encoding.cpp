#include "yarnbox/bytecode/encoding.hpp"

using namespace yarn;

Encoding yarn::encoding = []() {
	using namespace yarn::primitive;

	Encoding e;

	e.Rule(Opcode::Stop, PEmpty);
	e.Rule(Opcode::Nothing, PEmpty);
	e.Rule(Opcode::Self, PEmpty);
	e.Rule(Opcode::IntZero, PEmpty);
	e.Rule(Opcode::IntOne, PEmpty);
	e.Rule(Opcode::True, PEmpty);
	e.Rule(Opcode::False, PEmpty);
	e.Rule(Opcode::NoObject, PEmpty);
	e.Rule(Opcode::InterfaceCast, PEmpty);
	e.Rule(Opcode::EndOfScript, PEmpty);

	e.Rule(Opcode::ByteConst, PU8);
	e.Rule(Opcode::PrimitiveCast, PU8);

	e.Rule(Opcode::Jump, PU16);
	e.Rule(Opcode::JumpIfNotEditorOnly, PU16);

	e.Rule(Opcode::IntConst, PU32);
	e.Rule(Opcode::FloatConst, PU32);

	e.Rule(Opcode::LocalVariable, PU64);
	e.Rule(Opcode::InstanceVariable, PU64);
	e.Rule(Opcode::DefaultVariable, PU64);
	e.Rule(Opcode::VirtualFunction, PU64);
	e.Rule(Opcode::FinalFunction, PU64);
	e.Rule(Opcode::BoolVariable, PU64);
	e.Rule(Opcode::NativeParm, PU64);
	e.Rule(Opcode::ObjectConst, PU64);
	e.Rule(Opcode::NameConst, PU64);

	e.Rule(Opcode::JumpIfNot, PU16, PInsn);

	e.Rule(Opcode::MetaCast, PU64, PInsn);
	e.Rule(Opcode::DynamicCast, PU64, PInsn);

	e.Rule(Opcode::DynArrayLength, PInsn);

	e.Rule(Opcode::Let, PInsn, PInsn);
	e.Rule(Opcode::DynArrayElement, PInsn, PInsn);
	e.Rule(Opcode::LetBool, PInsn, PInsn);
	e.Rule(Opcode::Context, PInsn, PInsn);

	e.Rule(Opcode::Not_PreBool, PInsn, PDebugInfo);
	e.Rule(Opcode::Complement_PreInt, PInsn, PDebugInfo);
	e.Rule(Opcode::Subtract_PreInt, PInsn, PDebugInfo);
	e.Rule(Opcode::AddAdd_PreInt, PInsn, PDebugInfo);
	e.Rule(Opcode::SubtractSubtract_PreInt, PInsn, PDebugInfo);
	e.Rule(Opcode::AddAdd_Int, PInsn, PDebugInfo);
	e.Rule(Opcode::SubtractSubtract_Int, PInsn, PDebugInfo);
	e.Rule(Opcode::Subtract_PreFloat, PInsn, PDebugInfo);

	e.Rule(Opcode::DynArrayAdd, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Multiply_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Divide_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::AndAnd_BoolBool, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::OrOr_BoolBool, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Add_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::LessLess_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::GreaterGreater_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::GreaterGreaterGreater_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Less_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Greater_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::LessEqual_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::GreaterEqual_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::EqualEqual_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::NotEqual_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::And_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Xor_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Or_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::AddEqual_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::SubtractEqual_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Percent_IntInt, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::MultiplyMultiply_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Multiply_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Divide_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Percent_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Add_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Subtract_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Less_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::Greater_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::LessEqual_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::GreaterEqual_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::EqualEqual_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::NotEqual_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::MultiplyEqual_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::DivideEqual_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::AddEqual_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::SubtractEqual_FloatFloat, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::ComplementEqual_FloatFloat, PInsn, PInsn, PDebugInfo);

	e.Rule(Opcode::StringConst, PAnsiString);

	e.Rule(Opcode::RotationConst, PU32, PU32, PU32);
	e.Rule(Opcode::VectorConst, PU32, PU32, PU32);

	e.Rule(Opcode::DynArrayInsert, PInsn, PInsn, PInsn, PDebugInfo);
	e.Rule(Opcode::DynArrayRemove, PInsn, PInsn, PInsn, PDebugInfo);

	e.Rule(Opcode::DynArrayAddItem, PInsn, PU16, PInsn, PSentinel, PDebugInfo);
	e.Rule(Opcode::DynArrayRemoveItem, PInsn, PU16, PInsn, PSentinel, PDebugInfo);

	e.Rule(Opcode::DynArrayInsertItem, PInsn, PU16, PInsn, PInsn, PSentinel, PDebugInfo);

	return e;
}();
