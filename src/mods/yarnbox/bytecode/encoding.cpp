#include "yarnbox/bytecode/encoding.hpp"

using namespace yarn;

bool
yarn::primitive::HasDataInBytecodeTree(Type t)
{
	return !(t == PDebugInfo || t == PSentinel);
}

Encoding yarn::encoding = []() {
	using namespace yarn::primitive;

	Encoding e;

	// Control flow

	e.Rule(Opcode::Stop) = { PEmpty };
	e.Rule(Opcode::Nothing) = { PEmpty };
	e.Rule(Opcode::Jump) = { POffset };
	e.Rule(Opcode::JumpIfNot) = { POffset, PInsn };
	e.Rule(Opcode::JumpIfNotEditorOnly) = { POffset };
	e.Rule(Opcode::EndOfScript) = { PEmpty };

	e.Rule(Opcode::GotoLabel) = { PInsn };

	// Constants

	e.Rule(Opcode::IntZero) = { PEmpty };
	e.Rule(Opcode::IntOne) = { PEmpty };
	e.Rule(Opcode::True) = { PEmpty };
	e.Rule(Opcode::False) = { PEmpty };
	e.Rule(Opcode::ByteConst) = { PU8 };
	e.Rule(Opcode::IntConst) = { PU32 };
	e.Rule(Opcode::FloatConst) = { PU32 };
	e.Rule(Opcode::ObjectConst) = { PObj };
	e.Rule(Opcode::NameConst) = { PU64 };
	e.Rule(Opcode::StringConst) = { PAnsiString };
	e.Rule(Opcode::RotationConst) = { PU32, PU32, PU32 };
	e.Rule(Opcode::VectorConst) = { PU32, PU32, PU32 };
	e.Rule(Opcode::NoObject) = { PEmpty };

	// Variables

	e.Rule(Opcode::Self) = { PEmpty };
	e.Rule(Opcode::LocalVariable) = { PObj };
	e.Rule(Opcode::InstanceVariable) = { PObj };
	e.Rule(Opcode::DefaultVariable) = { PObj };
	e.Rule(Opcode::BoolVariable) = { PInsn };
	e.Rule(Opcode::Let) = { PInsn, PInsn };
	e.Rule(Opcode::LetBool) = { PInsn, PInsn };
	e.Rule(Opcode::StructMember) = { PObj, PObj, PU8, PU8, PInsn };

	// Functions and function calls

	auto fnargs = Primitive{ PInsns, Opcode::EndFunctionParms };
	e.Rule(Opcode::EmptyParmValue) = { PEmpty };
	e.Rule(Opcode::VirtualFunction) = { PU64, fnargs, PDebugInfo };
	e.Rule(Opcode::FinalFunction) = { PObj, fnargs, PDebugInfo };
	e.Rule(Opcode::EndDefaultParm) = { PEmpty };
	e.Rule(Opcode::EndFunctionParms) = { PEmpty };
	e.Rule(Opcode::Context) = { PInsn, PInsn };
	e.Rule(Opcode::Return) = { PInsn };
	e.Rule(Opcode::NativeParm) = { PObj };

	e.Rule(Opcode::InterfaceContext) = { PInsn };

	// Unary operators

	e.Rule(Opcode::Not_PreBool) = { PInsn, PDebugInfo };
	e.Rule(Opcode::Complement_PreInt) = { PInsn, PDebugInfo };
	e.Rule(Opcode::Subtract_PreInt) = { PInsn, PDebugInfo };
	e.Rule(Opcode::AddAdd_PreInt) = { PInsn, PDebugInfo };
	e.Rule(Opcode::SubtractSubtract_PreInt) = { PInsn, PDebugInfo };
	e.Rule(Opcode::AddAdd_Int) = { PInsn, PDebugInfo };
	e.Rule(Opcode::SubtractSubtract_Int) = { PInsn, PDebugInfo };
	e.Rule(Opcode::Subtract_PreFloat) = { PInsn, PDebugInfo };

	// Binary operators

	e.Rule(Opcode::DynArrayAdd) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Multiply_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Divide_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::AndAnd_BoolBool) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::OrOr_BoolBool) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Add_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::LessLess_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::GreaterGreater_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::GreaterGreaterGreater_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Less_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Greater_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::LessEqual_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::GreaterEqual_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::EqualEqual_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::NotEqual_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::And_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Xor_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Or_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::AddEqual_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::SubtractEqual_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Percent_IntInt) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::MultiplyMultiply_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Multiply_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Divide_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Percent_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Add_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Subtract_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Less_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::Greater_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::LessEqual_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::GreaterEqual_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::EqualEqual_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::NotEqual_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::MultiplyEqual_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::DivideEqual_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::AddEqual_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::SubtractEqual_FloatFloat) = { PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::ComplementEqual_FloatFloat) = { PInsn, PInsn, PDebugInfo };

	// Casts

	e.Rule(Opcode::InterfaceCast) = { PEmpty };
	e.Rule(Opcode::PrimitiveCast) = { PU8, PInsn };
	e.Rule(Opcode::MetaCast) = { PObj, PInsn };
	e.Rule(Opcode::DynamicCast) = { PObj, PInsn };

	// Dynamic arrays

	e.Rule(Opcode::DynArrayLength) = { PInsn };
	e.Rule(Opcode::DynArrayElement) = { PInsn, PInsn };
	e.Rule(Opcode::DynArrayInsert) = { PInsn, PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::DynArrayRemove) = { PInsn, PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::DynArrayAddItem) = { PInsn, PU16, PInsn, PSentinel, PDebugInfo };
	e.Rule(Opcode::DynArrayRemoveItem) = { PInsn, PU16, PInsn, PSentinel, PDebugInfo };
	e.Rule(Opcode::DynArrayInsertItem) = { PInsn, PU16, PInsn, PInsn, PSentinel, PDebugInfo };

	// Iterators

	e.Rule(Opcode::IteratorPop) = { PEmpty };

	return e;
}();
