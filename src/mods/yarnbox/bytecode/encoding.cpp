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
	e.Rule(Opcode::EndOfScript) = { PEmpty };
	e.Rule(Opcode::Nothing) = { PEmpty };

	e.Rule(Opcode::Jump) = { POffset };
	e.Rule(Opcode::JumpIfNot) = { POffset, PInsn };
	e.Rule(Opcode::JumpIfNotEditorOnly) = { POffset };
	e.Rule(Opcode::Conditional) = { PInsn, PU16, PInsn, PU16, PInsn };

	e.Rule(Opcode::Assert) = { PU16, PU8, PInsn };

	e.Rule(Opcode::GotoLabel) = { PInsn };

	// Constants

	e.Rule(Opcode::IntZero) = { PEmpty };
	e.Rule(Opcode::IntOne) = { PEmpty };
	e.Rule(Opcode::True) = { PEmpty };
	e.Rule(Opcode::False) = { PEmpty };
	e.Rule(Opcode::ByteConst) = { PU8 };
	e.Rule(Opcode::IntConst) = { PS32 };
	e.Rule(Opcode::FloatConst) = { PFloat };
	e.Rule(Opcode::ObjectConst) = { PObj };
	e.Rule(Opcode::NameConst) = { PU64 };
	e.Rule(Opcode::StringConst) = { PAnsiString };
	e.Rule(Opcode::RotationConst) = { PFloat, PFloat, PFloat };
	e.Rule(Opcode::VectorConst) = { PFloat, PFloat, PFloat };
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

	// Calls
	Primitive fnargs{ PInsns, Opcode::EndFunctionParms };
	e.Rule(Opcode::VirtualFunction) = { PU64, fnargs, PDebugInfo };
	e.Rule(Opcode::FinalFunction) = { PObj, fnargs, PDebugInfo };
	e.Rule(Opcode::EmptyParmValue) = { PEmpty };
	e.Rule(Opcode::EndFunctionParms) = { PEmpty };

	// Parameters
	e.Rule(Opcode::NativeParm) = { PObj };
	e.Rule(Opcode::DefaultParmValue) = { PU16, Primitive{ PInsns, Opcode::EndDefaultParm } };
	e.Rule(Opcode::EndDefaultParm) = { PEmpty };

	// Returns
	e.Rule(Opcode::Return) = { PInsn };
	e.Rule(Opcode::ReturnNothing) = { PObj };

	// Other (functionality yet to be uncovered)
	e.Rule(Opcode::InterfaceContext) = { PInsn };

	// Unary operators

	Rule rUnaryOp{ PInsn, PDebugInfo };
	e.Rule(Opcode::Not_PreBool) = rUnaryOp;
	e.Rule(Opcode::Complement_PreInt) = rUnaryOp;
	e.Rule(Opcode::Subtract_PreInt) = rUnaryOp;
	e.Rule(Opcode::AddAdd_PreInt) = rUnaryOp;
	e.Rule(Opcode::SubtractSubtract_PreInt) = rUnaryOp;
	e.Rule(Opcode::AddAdd_Int) = rUnaryOp;
	e.Rule(Opcode::SubtractSubtract_Int) = rUnaryOp;
	e.Rule(Opcode::Subtract_PreFloat) = rUnaryOp;

	// Binary operators

	Rule rBinaryOp{ PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::DynArrayAdd) = rBinaryOp;
	e.Rule(Opcode::Multiply_IntInt) = rBinaryOp;
	e.Rule(Opcode::Divide_IntInt) = rBinaryOp;
	e.Rule(Opcode::AndAnd_BoolBool) = rBinaryOp;
	e.Rule(Opcode::OrOr_BoolBool) = rBinaryOp;
	e.Rule(Opcode::Add_IntInt) = rBinaryOp;
	e.Rule(Opcode::LessLess_IntInt) = rBinaryOp;
	e.Rule(Opcode::GreaterGreater_IntInt) = rBinaryOp;
	e.Rule(Opcode::GreaterGreaterGreater_IntInt) = rBinaryOp;
	e.Rule(Opcode::Less_IntInt) = rBinaryOp;
	e.Rule(Opcode::Greater_IntInt) = rBinaryOp;
	e.Rule(Opcode::LessEqual_IntInt) = rBinaryOp;
	e.Rule(Opcode::GreaterEqual_IntInt) = rBinaryOp;
	e.Rule(Opcode::EqualEqual_IntInt) = rBinaryOp;
	e.Rule(Opcode::NotEqual_IntInt) = rBinaryOp;
	e.Rule(Opcode::And_IntInt) = rBinaryOp;
	e.Rule(Opcode::Xor_IntInt) = rBinaryOp;
	e.Rule(Opcode::Or_IntInt) = rBinaryOp;
	e.Rule(Opcode::AddEqual_IntInt) = rBinaryOp;
	e.Rule(Opcode::SubtractEqual_IntInt) = rBinaryOp;
	e.Rule(Opcode::Percent_IntInt) = rBinaryOp;
	e.Rule(Opcode::MultiplyMultiply_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::Multiply_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::Divide_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::Percent_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::Add_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::Subtract_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::Less_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::Greater_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::LessEqual_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::GreaterEqual_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::EqualEqual_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::NotEqual_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::MultiplyEqual_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::DivideEqual_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::AddEqual_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::SubtractEqual_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::ComplementEqual_FloatFloat) = rBinaryOp;
	e.Rule(Opcode::EqualEqual_ObjectObject) = rBinaryOp;
	e.Rule(Opcode::NotEqual_ObjectObject) = rBinaryOp;

	// Casts

	e.Rule(Opcode::InterfaceCast) = { PEmpty };
	e.Rule(Opcode::PrimitiveCast) = { PU8, PInsn };
	e.Rule(Opcode::MetaCast) = { PObj, PInsn };
	e.Rule(Opcode::DynamicCast) = { PObj, PInsn };

	// Objects

	e.Rule(Opcode::New) = { PInsn, PInsn, PInsn, PInsn, PInsn };
	e.Rule(Opcode::Context) = { PInsn, PU16, PObj, PU8, PInsn };

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
