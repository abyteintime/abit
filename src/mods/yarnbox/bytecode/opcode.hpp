#pragma once

#include <cstdint>
#include <string_view>

namespace yarn {

#define YARN_X_OPCODES(EXPAND) \
	EXPAND(LocalVariable, 0) \
	EXPAND(InstanceVariable, 1) \
	EXPAND(DefaultVariable, 2) \
	EXPAND(StateVariable, 3) \
	EXPAND(Return, 4) \
	EXPAND(Switch, 5) \
	EXPAND(Jump, 6) \
	EXPAND(JumpIfNot, 7) \
	EXPAND(Stop, 8) \
	EXPAND(Assert, 9) \
	EXPAND(Case, 10) \
	EXPAND(Nothing, 11) \
	EXPAND(GotoLabel, 13) \
	EXPAND(EatReturnValue, 14) \
	EXPAND(Let, 15) \
	EXPAND(DynArrayElement, 16) \
	EXPAND(New, 17) \
	EXPAND(ClassContext, 18) \
	EXPAND(MetaCast, 19) \
	EXPAND(LetBool, 20) \
	EXPAND(EndDefaultParm, 21) \
	EXPAND(EndFunctionParms, 22) \
	EXPAND(Self, 23) \
	EXPAND(Context, 25) \
	EXPAND(ArrayElement, 26) \
	EXPAND(VirtualFunction, 27) \
	EXPAND(FinalFunction, 28) \
	EXPAND(IntConst, 29) \
	EXPAND(FloatConst, 30) \
	EXPAND(StringConst, 31) \
	EXPAND(ObjectConst, 32) \
	EXPAND(NameConst, 33) \
	EXPAND(RotationConst, 34) \
	EXPAND(VectorConst, 35) \
	EXPAND(ByteConst, 36) \
	EXPAND(IntZero, 37) \
	EXPAND(IntOne, 38) \
	EXPAND(True, 39) \
	EXPAND(False, 40) \
	EXPAND(NativeParm, 41) \
	EXPAND(NoObject, 42) \
	EXPAND(IntConstByte, 44) \
	EXPAND(BoolVariable, 45) \
	EXPAND(DynamicCast, 46) \
	EXPAND(Iterator, 47) \
	EXPAND(IteratorPop, 48) \
	EXPAND(Continue, 49) \
	EXPAND(StructCmpEq, 50) \
	EXPAND(StructCmpNe, 51) \
	EXPAND(UnicodeStringConst, 52) \
	EXPAND(StructMember, 53) \
	EXPAND(DynArrayLength, 54) \
	EXPAND(GlobalFunction, 55) \
	EXPAND(PrimitiveCast, 56) \
	EXPAND(DynArrayInsert, 57) \
	EXPAND(ReturnNothing, 58) \
	EXPAND(EqualEqual_DelegateDelegate, 59) \
	EXPAND(NotEqual_DelegateDelegate, 60) \
	EXPAND(EqualEqual_DelegateFunction, 61) \
	EXPAND(NotEqual_DelegateFunction, 62) \
	EXPAND(EmptyDelegate, 63) \
	EXPAND(DynArrayRemove, 64) \
	EXPAND(DebugInfo, 65) \
	EXPAND(DelegateFunction, 66) \
	EXPAND(DelegateProperty, 67) \
	EXPAND(LetDelegate, 68) \
	EXPAND(Conditional, 69) \
	EXPAND(DynArrayFind, 70) \
	EXPAND(DynArrayFindStruct, 71) \
	EXPAND(LocalOutVariable, 72) \
	EXPAND(DefaultParmValue, 73) \
	EXPAND(EmptyParmValue, 74) \
	EXPAND(InstanceDelegate, 75) \
	EXPAND(InterfaceContext, 81) \
	EXPAND(InterfaceCast, 82) \
	EXPAND(EndOfScript, 83) \
	EXPAND(DynArrayAdd, 84) \
	EXPAND(DynArrayAddItem, 85) \
	EXPAND(DynArrayRemoveItem, 86) \
	EXPAND(DynArrayInsertItem, 87) \
	EXPAND(DynArrayIterator, 88) \
	EXPAND(DynArraySort, 89) \
	EXPAND(JumpIfNotEditorOnly, 90) \
	EXPAND(HighNative0, 96) \
	EXPAND(HighNative1, 97) \
	EXPAND(HighNative2, 98) \
	EXPAND(HighNative3, 99) \
	EXPAND(HighNative4, 100) \
	EXPAND(HighNative5, 101) \
	EXPAND(HighNative6, 102) \
	EXPAND(HighNative7, 103) \
	EXPAND(HighNative8, 104) \
	EXPAND(HighNative9, 105) \
	EXPAND(HighNative10, 106) \
	EXPAND(HighNative11, 107) \
	EXPAND(HighNative12, 108) \
	EXPAND(HighNative13, 109) \
	EXPAND(HighNative14, 110) \
	EXPAND(HighNative15, 111) \
	EXPAND(Concat_StrStr, 112) \
	EXPAND(GotoState, 113) \
	EXPAND(EqualEqual_ObjectObject, 114) \
	EXPAND(Less_StrStr, 115) \
	EXPAND(Greater_StrStr, 116) \
	EXPAND(Enable, 117) \
	EXPAND(Disable, 118) \
	EXPAND(NotEqual_ObjectObject, 119) \
	EXPAND(LessEqual_StrStr, 120) \
	EXPAND(GreaterEqual_StrStr, 121) \
	EXPAND(EqualEqual_StrStr, 122) \
	EXPAND(NotEqual_StrStr, 123) \
	EXPAND(ComplementEqual_StrStr, 124) \
	EXPAND(Len, 125) \
	EXPAND(InStr, 126) \
	EXPAND(Mid, 127) \
	EXPAND(Left, 128) \
	EXPAND(Not_PreBool, 129) \
	EXPAND(AndAnd_BoolBool, 130) \
	EXPAND(XorXor_BoolBool, 131) \
	EXPAND(OrOr_BoolBool, 132) \
	EXPAND(MultiplyEqual_ByteByte, 133) \
	EXPAND(DivideEqual_ByteByte, 134) \
	EXPAND(AddEqual_ByteByte, 135) \
	EXPAND(SubtractEqual_ByteByte, 136) \
	EXPAND(AddAdd_PreByte, 137) \
	EXPAND(SubtractSubtract_PreByte, 138) \
	EXPAND(AddAdd_Byte, 139) \
	EXPAND(SubtractSubtract_Byte, 140) \
	EXPAND(Complement_PreInt, 141) \
	EXPAND(EqualEqual_RotatorRotator, 142) \
	EXPAND(Subtract_PreInt, 143) \
	EXPAND(Multiply_IntInt, 144) \
	EXPAND(Divide_IntInt, 145) \
	EXPAND(Add_IntInt, 146) \
	EXPAND(Subtract_IntInt, 147) \
	EXPAND(LessLess_IntInt, 148) \
	EXPAND(GreaterGreater_IntInt, 149) \
	EXPAND(Less_IntInt, 150) \
	EXPAND(Greater_IntInt, 151) \
	EXPAND(LessEqual_IntInt, 152) \
	EXPAND(GreaterEqual_IntInt, 153) \
	EXPAND(EqualEqual_IntInt, 154) \
	EXPAND(NotEqual_IntInt, 155) \
	EXPAND(And_IntInt, 156) \
	EXPAND(Xor_IntInt, 157) \
	EXPAND(Or_IntInt, 158) \
	EXPAND(MultiplyEqual_IntFloat, 159) \
	EXPAND(DivideEqual_IntFloat, 160) \
	EXPAND(AddEqual_IntInt, 161) \
	EXPAND(SubtractEqual_IntInt, 162) \
	EXPAND(AddAdd_PreInt, 163) \
	EXPAND(SubtractSubtract_PreInt, 164) \
	EXPAND(AddAdd_Int, 165) \
	EXPAND(SubtractSubtract_Int, 166) \
	EXPAND(Rand, 167) \
	EXPAND(At_StrStr, 168) \
	EXPAND(Subtract_PreFloat, 169) \
	EXPAND(MultiplyMultiply_FloatFloat, 170) \
	EXPAND(Multiply_FloatFloat, 171) \
	EXPAND(Divide_FloatFloat, 172) \
	EXPAND(Percent_FloatFloat, 173) \
	EXPAND(Add_FloatFloat, 174) \
	EXPAND(Subtract_FloatFloat, 175) \
	EXPAND(Less_FloatFloat, 176) \
	EXPAND(Greater_FloatFloat, 177) \
	EXPAND(LessEqual_FloatFloat, 178) \
	EXPAND(GreaterEqual_FloatFloat, 179) \
	EXPAND(EqualEqual_FloatFloat, 180) \
	EXPAND(NotEqual_FloatFloat, 181) \
	EXPAND(MultiplyEqual_FloatFloat, 182) \
	EXPAND(DivideEqual_FloatFloat, 183) \
	EXPAND(AddEqual_FloatFloat, 184) \
	EXPAND(SubtractEqual_FloatFloat, 185) \
	EXPAND(Abs, 186) \
	EXPAND(Sin, 187) \
	EXPAND(Cos, 188) \
	EXPAND(Tan, 189) \
	EXPAND(Atan, 190) \
	EXPAND(Exp, 191) \
	EXPAND(Loge, 192) \
	EXPAND(Sqrt, 193) \
	EXPAND(Square, 194) \
	EXPAND(FRand, 195) \
	EXPAND(GreaterGreaterGreater_IntInt, 196) \
	EXPAND(IsA, 197) \
	EXPAND(MultiplyEqual_ByteFloat, 198) \
	EXPAND(Round, 199) \
	EXPAND(Repl, 201) \
	EXPAND(NotEqual_RotatorRotator, 203) \
	EXPAND(ComplementEqual_FloatFloat, 210) \
	EXPAND(Subtract_PreVector, 211) \
	EXPAND(Multiply_VectorFloat, 212) \
	EXPAND(Multiply_FloatVector, 213) \
	EXPAND(Divide_VectorFloat, 214) \
	EXPAND(Add_VectorVector, 215) \
	EXPAND(Subtract_VectorVector, 216) \
	EXPAND(EqualEqual_VectorVector, 217) \
	EXPAND(NotEqual_VectorVector, 218) \
	EXPAND(Dot_VectorVector, 219) \
	EXPAND(Cross_VectorVector, 220) \
	EXPAND(MultiplyEqual_VectorFloat, 221) \
	EXPAND(DivideEqual_VectorFloat, 222) \
	EXPAND(AddEqual_VectorVector, 223) \
	EXPAND(SubtractEqual_VectorVector, 224) \
	EXPAND(VSize, 225) \
	EXPAND(Normal, 226) \
	EXPAND(Normal2D, 227) \
	EXPAND(VSizeSq, 228) \
	EXPAND(GetAxes, 229) \
	EXPAND(GetUnAxes, 230) \
	EXPAND(LogInternal, 231) \
	EXPAND(WarnInternal, 232) \
	EXPAND(Right, 234) \
	EXPAND(Caps, 235) \
	EXPAND(Chr, 236) \
	EXPAND(Asc, 237) \
	EXPAND(Locs, 238) \
	EXPAND(EqualEqual_BoolBool, 242) \
	EXPAND(NotEqual_BoolBool, 243) \
	EXPAND(FMin, 244) \
	EXPAND(FMax, 245) \
	EXPAND(FClamp, 246) \
	EXPAND(Lerp, 247) \
	EXPAND(Min, 249) \
	EXPAND(Max, 250) \
	EXPAND(Clamp, 251) \
	EXPAND(VRand, 252) \
	EXPAND(Percent_IntInt, 253) \
	EXPAND(EqualEqual_NameName, 254) \
	EXPAND(NotEqual_NameName, 255) \
	EXPAND(Actor_Sleep, 256) \
	EXPAND(ClassIsChildOf, 258) \
	EXPAND(Actor_FinishAnim, 261) \
	EXPAND(Actor_SetCollision, 262) \
	EXPAND(Actor_Move, 266) \
	EXPAND(Actor_SetLocation, 267) \
	EXPAND(Add_QuatQuat, 270) \
	EXPAND(Subtract_QuatQuat, 271) \
	EXPAND(Actor_SetOwner, 272) \
	EXPAND(LessLess_VectorRotator, 275) \
	EXPAND(GreaterGreater_VectorRotator, 276) \
	EXPAND(Actor_Trace, 277) \
	EXPAND(Actor_Destroy, 279) \
	EXPAND(Actor_SetTimer, 280) \
	EXPAND(IsInState, 281) \
	EXPAND(Actor_SetCollisionSize, 283) \
	EXPAND(GetStateName, 284) \
	EXPAND(Multiply_RotatorFloat, 287) \
	EXPAND(Multiply_FloatRotator, 288) \
	EXPAND(Divide_RotatorFloat, 289) \
	EXPAND(MultiplyEqual_RotatorFloat, 290) \
	EXPAND(DivideEqual_RotatorFloat, 291) \
	EXPAND(Multiply_VectorVector, 296) \
	EXPAND(MultiplyEqual_VectorVector, 297) \
	EXPAND(Actor_SetBase, 298) \
	EXPAND(Actor_SetRotation, 299) \
	EXPAND(MirrorVectorByNormal, 300) \
	EXPAND(Actor_AllActors, 304) \
	EXPAND(Actor_ChildActors, 305) \
	EXPAND(Actor_BasedActors, 306) \
	EXPAND(Actor_TouchingActors, 307) \
	EXPAND(Actor_TracedActors, 309) \
	EXPAND(Actor_VisibleActors, 311) \
	EXPAND(Actor_VisibleCollidingActors, 312) \
	EXPAND(Actor_DynamicActors, 313) \
	EXPAND(Add_RotatorRotator, 316) \
	EXPAND(Subtract_RotatorRotator, 317) \
	EXPAND(AddEqual_RotatorRotator, 318) \
	EXPAND(SubtractEqual_RotatorRotator, 319) \
	EXPAND(RotRand, 320) \
	EXPAND(Actor_CollidingActors, 321) \
	EXPAND(ConcatEqual_StrStr, 322) \
	EXPAND(AtEqual_StrStr, 323) \
	EXPAND(SubtractEqual_StrStr, 324) \
	EXPAND(Actor_PollSleep, 384) \
	EXPAND(Actor_PollFinishAnim, 385) \
	EXPAND(Controller_MoveTo, 500) \
	EXPAND(Controller_Unknown1, 501) \
	EXPAND(Controller_MoveToward, 502) \
	EXPAND(Controller_Unknown2, 503) \
	EXPAND(Controller_FinishRotation, 508) \
	EXPAND(Controller_PollFinishRotation, 509) \
	EXPAND(Actor_MakeNoise, 512) \
	EXPAND(Controller_LineOfSightTo, 514) \
	EXPAND(Controller_FindPathToward, 517) \
	EXPAND(Controller_FindPathTo, 518) \
	EXPAND(Controller_ActorReachable, 520) \
	EXPAND(Controller_PointReachable, 521) \
	EXPAND(PlayerController_FindStairRotation, 524) \
	EXPAND(Controller_FindRandomDest, 525) \
	EXPAND(Controller_PickWallAdjust, 526) \
	EXPAND(Controller_WaitForLanding, 527) \
	EXPAND(Controller_PollWaitForLanding, 528) \
	EXPAND(Controller_PickTarget, 531) \
	EXPAND(Actor_PlayerCanSeeMe, 532) \
	EXPAND(Controller_CanSee, 533) \
	EXPAND(SaveConfig, 536) \
	EXPAND(Controller_CanSeeByPoints, 537) \
	EXPAND(PlayerController_UpdateURL, 546) \
	EXPAND(Controller_GetURLMap, 547) \
	EXPAND(Controller_FastTrace, 548) \
	EXPAND(ProjectOnTo, 1500) \
	EXPAND(IsZero, 1501) \
	EXPAND(Actor_MoveSmooth, 3969) \
	EXPAND(Actor_SetPhysics, 3970) \
	EXPAND(Actor_AutonomousPhysics, 3971) \
	EXPAND(Overwritten, 4049) \
	EXPAND(BytecodeTree, 4050) \
	EXPAND(FlattenedPrimitiveCast, 4051) \
	EXPAND(OutOfBounds, 4094) \
	EXPAND(Unknown, 4095)

#define YARN__EXPAND_AS_ENUM_MEMBERS(Name, Index) Name = Index,

enum class Opcode : uint16_t
{
	YARN_X_OPCODES(YARN__EXPAND_AS_ENUM_MEMBERS)
};

constexpr size_t opcodeCount = 4096;

std::string_view
OpcodeToString(Opcode opcode);

#define YARN_X_PRIMITIVE_CASTS(EXPAND) \
	EXPAND(InterfaceToObject, 54) \
	EXPAND(InterfaceToString, 55) \
	EXPAND(InterfaceToBool, 56) \
	EXPAND(RotatorToVector, 57) \
	EXPAND(ByteToInt, 58) \
	EXPAND(ByteToBool, 59) \
	EXPAND(ByteToFloat, 60) \
	EXPAND(IntToByte, 61) \
	EXPAND(IntToBool, 62) \
	EXPAND(IntToFloat, 63) \
	EXPAND(BoolToByte, 64) \
	EXPAND(BoolToInt, 65) \
	EXPAND(BoolToFloat, 66) \
	EXPAND(FloatToByte, 67) \
	EXPAND(FloatToInt, 68) \
	EXPAND(FloatToBool, 69) \
	EXPAND(ObjectToInterface, 70) \
	EXPAND(ObjectToBool, 71) \
	EXPAND(NameToBool, 72) \
	EXPAND(StringToByte, 73) \
	EXPAND(StringToInt, 74) \
	EXPAND(StringToBool, 75) \
	EXPAND(StringToFloat, 76) \
	EXPAND(StringToVector, 77) \
	EXPAND(StringToRotator, 78) \
	EXPAND(VectorToBool, 79) \
	EXPAND(VectorToRotator, 80) \
	EXPAND(RotatorToBool, 81) \
	EXPAND(ByteToString, 82) \
	EXPAND(IntToString, 83) \
	EXPAND(BoolToString, 84) \
	EXPAND(FloatToString, 85) \
	EXPAND(ObjectToString, 86) \
	EXPAND(NameToString, 87) \
	EXPAND(VectorToString, 88) \
	EXPAND(RotatorToString, 89) \
	EXPAND(DelegateToString, 90) \
	EXPAND(StringToName, 96)

enum class PrimitiveCast : uint8_t
{
	YARN_X_PRIMITIVE_CASTS(YARN__EXPAND_AS_ENUM_MEMBERS)
};

constexpr size_t primitiveCastCount = 256;
constexpr size_t firstPrimitiveCast = static_cast<size_t>(PrimitiveCast::InterfaceToObject);

#undef YARN__EXPAND_AS_ENUM_MEMBERS

static inline constexpr Opcode
PrimitiveCastToOpcode(PrimitiveCast cast)
{
	return Opcode(size_t(cast) - firstPrimitiveCast + size_t(Opcode::FlattenedPrimitiveCast));
}

std::string_view
OpcodeToString(Opcode opcode);

std::string_view
PrimitiveCastToString(PrimitiveCast cast);

}
