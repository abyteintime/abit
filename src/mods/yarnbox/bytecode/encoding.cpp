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

	e.Rule(Opcode::Jump) = { POAbs };
	e.Rule(Opcode::JumpIfNot) = { POAbs, PInsn };
	e.Rule(Opcode::JumpIfNotEditorOnly) = { POAbs };
	e.Rule(Opcode::Conditional) = { PInsn, PORel, PInsn, PORel, PInsn };

	e.Rule(Opcode::Assert) = { PU16, PU8, PInsn };

	e.Rule(Opcode::GotoLabel) = { PInsn };

	e.Rule(Opcode::EatReturnValue) = { PObj, PInsn };

	// Constants

	e.Rule(Opcode::IntZero) = { PEmpty };
	e.Rule(Opcode::IntOne) = { PEmpty };
	e.Rule(Opcode::True) = { PEmpty };
	e.Rule(Opcode::False) = { PEmpty };
	e.Rule(Opcode::ByteConst) = { PU8 };
	e.Rule(Opcode::IntConst) = { PS32 };
	e.Rule(Opcode::IntConstByte) = { PU8 };
	e.Rule(Opcode::FloatConst) = { PFloat };
	e.Rule(Opcode::ObjectConst) = { PObj };
	e.Rule(Opcode::NameConst) = { PName };
	e.Rule(Opcode::StringConst) = { PAnsiString };
	e.Rule(Opcode::RotationConst) = { PFloat, PFloat, PFloat };
	e.Rule(Opcode::VectorConst) = { PFloat, PFloat, PFloat };
	e.Rule(Opcode::NoObject) = { PEmpty };

	// Variables

	e.Rule(Opcode::Self) = { PEmpty };
	e.Rule(Opcode::LocalVariable) = { PObj };
	e.Rule(Opcode::LocalOutVariable) = { PObj };
	e.Rule(Opcode::InstanceVariable) = { PObj };
	e.Rule(Opcode::DefaultVariable) = { PObj };
	e.Rule(Opcode::BoolVariable) = { PInsn };
	e.Rule(Opcode::Let) = { PInsn, PInsn };
	e.Rule(Opcode::LetBool) = { PInsn, PInsn };
	e.Rule(Opcode::LetDelegate) = { PInsn, PInsn };
	e.Rule(Opcode::ArrayElement) = { PInsn, PInsn };

	// Functions and function calls

	// Calls
	Primitive rFnArgs{ PInsns, Opcode::EndFunctionParms };
	e.Rule(Opcode::VirtualFunction) = { PName, rFnArgs, PDebugInfo };
	e.Rule(Opcode::FinalFunction) = { PObj, rFnArgs, PDebugInfo };
	e.Rule(Opcode::DelegateFunction) = { PU8, PObj, PName, rFnArgs };
	e.Rule(Opcode::GlobalFunction) = { PName, rFnArgs, PDebugInfo };
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

	// Rule shortcuts for functions

	Rule rFn0{ PSentinel, PDebugInfo };
	Rule rFn1{ PInsn, PSentinel, PDebugInfo };
	Rule rFn2{ PInsn, PInsn, PSentinel, PDebugInfo };
	Rule rFn3{ PInsn, PInsn, PInsn, PSentinel, PDebugInfo };
	Rule rFn4{ PInsn, PInsn, PInsn, PInsn, PSentinel, PDebugInfo };
	Rule rFn5{ PInsn, PInsn, PInsn, PInsn, PInsn, PSentinel, PDebugInfo };
	Rule rFn6{ PInsn, PInsn, PInsn, PInsn, PInsn, PInsn, PSentinel, PDebugInfo };
	Rule rFn7plus{ rFnArgs, PDebugInfo };

	// Bools

	e.Rule(Opcode::Not_PreBool) = rFn1;

	// These bool operators are unlike other binary operators because they short-circuit.
	Rule rShortCircuitingBinaryOp{ PInsn, PSentinel, PORel, PInsn, PSentinel };
	e.Rule(Opcode::AndAnd_BoolBool) = rShortCircuitingBinaryOp;
	e.Rule(Opcode::OrOr_BoolBool) = rShortCircuitingBinaryOp;

	e.Rule(Opcode::EqualEqual_BoolBool) = rFn2;
	e.Rule(Opcode::NotEqual_BoolBool) = rFn2;
	e.Rule(Opcode::XorXor_BoolBool) = rFn2;

	// Bytes

	e.Rule(Opcode::MultiplyEqual_ByteByte) = rFn2;
	e.Rule(Opcode::DivideEqual_ByteByte) = rFn2;
	e.Rule(Opcode::AddEqual_ByteByte) = rFn2;
	e.Rule(Opcode::SubtractEqual_ByteByte) = rFn2;
	e.Rule(Opcode::AddAdd_PreByte) = rFn1;
	e.Rule(Opcode::SubtractSubtract_PreByte) = rFn1;
	e.Rule(Opcode::AddAdd_Byte) = rFn1;
	e.Rule(Opcode::SubtractSubtract_Byte) = rFn1;
	e.Rule(Opcode::MultiplyEqual_ByteFloat) = rFn2;

	// Ints

	// Operators
	e.Rule(Opcode::Complement_PreInt) = rFn1;
	e.Rule(Opcode::Subtract_PreInt) = rFn1;
	e.Rule(Opcode::AddAdd_PreInt) = rFn1;
	e.Rule(Opcode::SubtractSubtract_PreInt) = rFn1;
	e.Rule(Opcode::AddAdd_Int) = rFn1;
	e.Rule(Opcode::SubtractSubtract_Int) = rFn1;
	e.Rule(Opcode::Multiply_IntInt) = rFn2;
	e.Rule(Opcode::Divide_IntInt) = rFn2;
	e.Rule(Opcode::Add_IntInt) = rFn2;
	e.Rule(Opcode::Subtract_IntInt) = rFn2;
	e.Rule(Opcode::LessLess_IntInt) = rFn2;
	e.Rule(Opcode::GreaterGreater_IntInt) = rFn2;
	e.Rule(Opcode::GreaterGreaterGreater_IntInt) = rFn2;
	e.Rule(Opcode::Less_IntInt) = rFn2;
	e.Rule(Opcode::Greater_IntInt) = rFn2;
	e.Rule(Opcode::LessEqual_IntInt) = rFn2;
	e.Rule(Opcode::GreaterEqual_IntInt) = rFn2;
	e.Rule(Opcode::EqualEqual_IntInt) = rFn2;
	e.Rule(Opcode::NotEqual_IntInt) = rFn2;
	e.Rule(Opcode::And_IntInt) = rFn2;
	e.Rule(Opcode::Xor_IntInt) = rFn2;
	e.Rule(Opcode::Or_IntInt) = rFn2;
	e.Rule(Opcode::AddEqual_IntInt) = rFn2;
	e.Rule(Opcode::SubtractEqual_IntInt) = rFn2;
	e.Rule(Opcode::Percent_IntInt) = rFn2;
	e.Rule(Opcode::MultiplyEqual_IntFloat) = rFn2;
	e.Rule(Opcode::DivideEqual_IntFloat) = rFn2;

	// Functions
	e.Rule(Opcode::Rand) = rFn1;
	e.Rule(Opcode::Min) = rFn2;
	e.Rule(Opcode::Max) = rFn2;
	e.Rule(Opcode::Clamp) = rFn3;

	// Floats

	// Operators
	e.Rule(Opcode::Subtract_PreFloat) = rFn1;
	e.Rule(Opcode::MultiplyMultiply_FloatFloat) = rFn2;
	e.Rule(Opcode::Multiply_FloatFloat) = rFn2;
	e.Rule(Opcode::Divide_FloatFloat) = rFn2;
	e.Rule(Opcode::Percent_FloatFloat) = rFn2;
	e.Rule(Opcode::Add_FloatFloat) = rFn2;
	e.Rule(Opcode::Subtract_FloatFloat) = rFn2;
	e.Rule(Opcode::Less_FloatFloat) = rFn2;
	e.Rule(Opcode::Greater_FloatFloat) = rFn2;
	e.Rule(Opcode::LessEqual_FloatFloat) = rFn2;
	e.Rule(Opcode::GreaterEqual_FloatFloat) = rFn2;
	e.Rule(Opcode::EqualEqual_FloatFloat) = rFn2;
	e.Rule(Opcode::NotEqual_FloatFloat) = rFn2;
	e.Rule(Opcode::MultiplyEqual_FloatFloat) = rFn2;
	e.Rule(Opcode::DivideEqual_FloatFloat) = rFn2;
	e.Rule(Opcode::AddEqual_FloatFloat) = rFn2;
	e.Rule(Opcode::SubtractEqual_FloatFloat) = rFn2;
	e.Rule(Opcode::ComplementEqual_FloatFloat) = rFn2;

	// Functions
	e.Rule(Opcode::Abs) = rFn1;
	e.Rule(Opcode::Sin) = rFn1;
	e.Rule(Opcode::Cos) = rFn1;
	e.Rule(Opcode::Tan) = rFn1;
	e.Rule(Opcode::Atan) = rFn1;
	e.Rule(Opcode::Exp) = rFn1;
	e.Rule(Opcode::Loge) = rFn1;
	e.Rule(Opcode::Sqrt) = rFn1;
	e.Rule(Opcode::Square) = rFn1;
	e.Rule(Opcode::FRand) = rFn0;
	e.Rule(Opcode::FMin) = rFn2;
	e.Rule(Opcode::FMax) = rFn2;
	e.Rule(Opcode::FClamp) = rFn3;
	e.Rule(Opcode::Lerp) = rFn3;
	e.Rule(Opcode::Round) = rFn1;

	// Vectors

	// Operators
	e.Rule(Opcode::Subtract_PreVector) = rFn1;
	e.Rule(Opcode::Multiply_VectorFloat) = rFn2;
	e.Rule(Opcode::Multiply_FloatVector) = rFn2;
	e.Rule(Opcode::Divide_VectorFloat) = rFn2;
	e.Rule(Opcode::Add_VectorVector) = rFn2;
	e.Rule(Opcode::Subtract_VectorVector) = rFn2;
	e.Rule(Opcode::Multiply_VectorVector) = rFn2;
	e.Rule(Opcode::EqualEqual_VectorVector) = rFn2;
	e.Rule(Opcode::NotEqual_VectorVector) = rFn2;
	e.Rule(Opcode::Dot_VectorVector) = rFn2;
	e.Rule(Opcode::Cross_VectorVector) = rFn2;
	e.Rule(Opcode::AddEqual_VectorVector) = rFn2;
	e.Rule(Opcode::SubtractEqual_VectorVector) = rFn2;
	e.Rule(Opcode::MultiplyEqual_VectorFloat) = rFn2;
	e.Rule(Opcode::DivideEqual_VectorFloat) = rFn2;
	e.Rule(Opcode::MultiplyEqual_VectorVector) = rFn2;

	// Functions
	e.Rule(Opcode::VSize) = rFn1;
	e.Rule(Opcode::VSizeSq) = rFn1;
	e.Rule(Opcode::Normal) = rFn1;
	e.Rule(Opcode::Normal2D) = rFn1;
	e.Rule(Opcode::VRand) = rFn0;
	e.Rule(Opcode::IsZero) = rFn1;

	// Rotators

	// Operators
	e.Rule(Opcode::EqualEqual_RotatorRotator) = rFn2;
	e.Rule(Opcode::NotEqual_RotatorRotator) = rFn2;
	e.Rule(Opcode::LessLess_VectorRotator) = rFn2;
	e.Rule(Opcode::GreaterGreater_VectorRotator) = rFn2;
	e.Rule(Opcode::Multiply_RotatorFloat) = rFn2;
	e.Rule(Opcode::Multiply_FloatRotator) = rFn2;
	e.Rule(Opcode::Divide_RotatorFloat) = rFn2;
	e.Rule(Opcode::MultiplyEqual_RotatorFloat) = rFn2;
	e.Rule(Opcode::DivideEqual_RotatorFloat) = rFn2;
	e.Rule(Opcode::Add_RotatorRotator) = rFn2;
	e.Rule(Opcode::Subtract_RotatorRotator) = rFn2;
	e.Rule(Opcode::AddEqual_RotatorRotator) = rFn2;
	e.Rule(Opcode::SubtractEqual_RotatorRotator) = rFn2;

	// Strings

	// Operators
	e.Rule(Opcode::Concat_StrStr) = rFn2;
	e.Rule(Opcode::At_StrStr) = rFn2;
	e.Rule(Opcode::EqualEqual_StrStr) = rFn2;
	e.Rule(Opcode::NotEqual_StrStr) = rFn2;
	e.Rule(Opcode::ComplementEqual_StrStr) = rFn2;
	e.Rule(Opcode::Less_StrStr) = rFn2;
	e.Rule(Opcode::LessEqual_StrStr) = rFn2;
	e.Rule(Opcode::Greater_StrStr) = rFn2;
	e.Rule(Opcode::GreaterEqual_StrStr) = rFn2;
	e.Rule(Opcode::ConcatEqual_StrStr) = rFn2;
	e.Rule(Opcode::AtEqual_StrStr) = rFn2;
	e.Rule(Opcode::SubtractEqual_StrStr) = rFn2;

	// Functions
	e.Rule(Opcode::Len) = rFn1;
	e.Rule(Opcode::InStr) = rFn5;
	e.Rule(Opcode::Mid) = rFn3;
	e.Rule(Opcode::Left) = rFn2;
	e.Rule(Opcode::Right) = rFn2;
	e.Rule(Opcode::Caps) = rFn1;
	e.Rule(Opcode::Locs) = rFn1;
	e.Rule(Opcode::Chr) = rFn1;
	e.Rule(Opcode::Asc) = rFn1;
	e.Rule(Opcode::Repl) = rFn4;

	// Names

	e.Rule(Opcode::EqualEqual_NameName) = rFn2;
	e.Rule(Opcode::NotEqual_NameName) = rFn2;

	// Objects

	e.Rule(Opcode::New) = { PInsn, PInsn, PInsn, PInsn, PInsn };
	e.Rule(Opcode::Context) = { PInsn, PU16, PObj, PU8, PInsn };
	e.Rule(Opcode::ClassContext) = { PInsn, PU16, PObj, PU8, PInsn };

	e.Rule(Opcode::EqualEqual_ObjectObject) = rFn2;
	e.Rule(Opcode::NotEqual_ObjectObject) = rFn2;

	e.Rule(Opcode::ClassIsChildOf) = rFn2;
	e.Rule(Opcode::IsA) = rFn1;

	// Structs

	e.Rule(Opcode::StructCmpEq) = { PObj, PInsn, PInsn };
	e.Rule(Opcode::StructCmpNe) = { PObj, PInsn, PInsn };
	e.Rule(Opcode::StructMember) = { PObj, PObj, PU8, PU8, PInsn };

	// Casts

	e.Rule(Opcode::InterfaceCast) = { PObj, PInsn };
	e.Rule(Opcode::PrimitiveCast) = { PPrimitiveCast };
	e.Rule(Opcode::MetaCast) = { PObj, PInsn };
	e.Rule(Opcode::DynamicCast) = { PObj, PInsn };

	// Most primitive casts are simple and only accept a single instruction.
	for (size_t i = size_t(PrimitiveCast::InterfaceToObject);
		 i <= size_t(PrimitiveCast::StringToName);
		 ++i) {
		PrimitiveCast cast = static_cast<PrimitiveCast>(i);
		e.Rule(cast) = { PInsn };
	}
	// Other casts are a little different.
	e.Rule(PrimitiveCast::ObjectToInterface) = e.Rule(Opcode::InterfaceCast);

	// Delegates

	e.Rule(Opcode::EmptyDelegate) = { PEmpty };
	e.Rule(Opcode::DelegateProperty) = { PName, PObj };

	e.Rule(Opcode::EqualEqual_DelegateDelegate) = rFn2;
	e.Rule(Opcode::NotEqual_DelegateDelegate) = rFn2;
	e.Rule(Opcode::EqualEqual_DelegateFunction) = rFn2;
	e.Rule(Opcode::NotEqual_DelegateFunction) = rFn2;

	// Dynamic arrays

	e.Rule(Opcode::DynArrayLength) = { PInsn };
	e.Rule(Opcode::DynArrayElement) = { PInsn, PInsn };
	e.Rule(Opcode::DynArrayAdd) = rFn2;
	e.Rule(Opcode::DynArrayInsert) = { PInsn, PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::DynArrayRemove) = { PInsn, PInsn, PInsn, PDebugInfo };
	e.Rule(Opcode::DynArrayAddItem) = { PInsn, PU16, PInsn, PSentinel, PDebugInfo };
	e.Rule(Opcode::DynArrayInsertItem) = { PInsn, PU16, PInsn, PInsn, PSentinel, PDebugInfo };
	e.Rule(Opcode::DynArrayRemoveItem) = { PInsn, PU16, PInsn, PSentinel, PDebugInfo };
	e.Rule(Opcode::DynArrayFind) = { PInsn, PORel, PInsn, PSentinel, PDebugInfo };
	e.Rule(Opcode::DynArrayFindStruct) = { PInsn, PORel, PInsn, PInsn, PSentinel, PDebugInfo };

	// Iterators

	e.Rule(Opcode::Iterator) = { PInsn, PORel };
	e.Rule(Opcode::IteratorPop) = { PEmpty };
	e.Rule(Opcode::Continue) = { PEmpty };

	e.Rule(Opcode::DynArrayIterator) = { PInsn, PInsn, PSentinel, PInsn, PORel };

	// States

	e.Rule(Opcode::GotoState) = rFn4;
	e.Rule(Opcode::IsInState) = rFn2;
	e.Rule(Opcode::GetStateName) = rFn0;

	// Actor

	e.Rule(Opcode::Actor_Sleep) = rFn1;
	e.Rule(Opcode::Actor_FinishAnim) = rFn2;
	e.Rule(Opcode::Actor_SetCollision) = rFn3;
	e.Rule(Opcode::Actor_SetCollisionSize) = rFn2;
	e.Rule(Opcode::Actor_Move) = rFn1;
	e.Rule(Opcode::Actor_SetLocation) = rFn1;
	e.Rule(Opcode::Actor_SetRotation) = rFn1;
	e.Rule(Opcode::Actor_MoveSmooth) = rFn1;
	e.Rule(Opcode::Actor_AutonomousPhysics) = rFn1;
	e.Rule(Opcode::Actor_SetBase) = rFn4;
	e.Rule(Opcode::Actor_SetOwner) = rFn1;
	e.Rule(Opcode::Actor_SetPhysics) = rFn1;
	e.Rule(Opcode::Actor_Trace) = rFn7plus;
	e.Rule(Opcode::Actor_Destroy) = rFn0;
	e.Rule(Opcode::Actor_SetTimer) = rFn5;
	e.Rule(Opcode::Actor_MakeNoise) = rFn2;
	e.Rule(Opcode::Actor_PlayerCanSeeMe) = rFn1;

	e.Rule(Opcode::Actor_AllActors) = rFn3;
	e.Rule(Opcode::Actor_ChildActors) = rFn2;
	e.Rule(Opcode::Actor_BasedActors) = rFn2;
	e.Rule(Opcode::Actor_TouchingActors) = rFn2;
	e.Rule(Opcode::Actor_TracedActors) = rFn7plus;
	e.Rule(Opcode::Actor_VisibleActors) = rFn4;
	e.Rule(Opcode::Actor_VisibleCollidingActors) = rFn7plus;
	e.Rule(Opcode::Actor_DynamicActors) = rFn3;
	e.Rule(Opcode::Actor_CollidingActors) = rFn7plus;

	// Controller

	e.Rule(Opcode::Controller_GetURLMap) = rFn0;
	e.Rule(Opcode::Controller_FastTrace) = rFn4;

	return e;
}();
