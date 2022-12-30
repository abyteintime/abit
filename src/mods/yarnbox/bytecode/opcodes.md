# How to read this table

The columns are:
- **Index** - index in the `GNatives` array.
- **Name** - name of the `exec` function corresponding to the opcode, suffixed with `_?` if the exact name is not (yet) known.
- **Description** - informal description of the opcode's behavior.
- **Operands** - how the opcode's operands are encoded in the bytecode.
  Lack of operands is denoted with a dash `-`, and operands that haven't been reversed yet are denoted with the word `unknown`.

## Encoding

This document PEG-like language to describe how instructions are encoded formally.
- `rule <- abc` - define a rule
- `rule` - match a rule
- `a b` - match rule `a` then rule `b`
- `123` - match an exact byte
- `{}` - match a byte from a given set. Possible elements are:
  - `123` - a single byte
  - `1 .. 10` - a range of bytes

Available rules are:
```
byte <- {0 .. 255}

u8 <- byte
u16 <- byte byte
u32 <- byte byte byte byte
u64 <- byte byte byte byte byte byte byte byte

ptr <- u64

lowOpcode <- {
  0 .. 3, 5 .. 11, 13 .. 20, 22, 23, 25 .. 42, 44 .. 48, 50 .. 75, 81 .. 90, 112 .. 199, 201, 203,
  210 .. 232, 234 .. 238, 242 .. 247, 249 .. 255
}
highNative <- {96 .. 111}
opcode <- lowOpcode | highNative byte

// `operands` is specified by the Operands column in the table below.
insn <- opcode operands
```
Rules may capture variables which can be referred to in the description, with the syntax `variable@rule`.

# Instructions

Index | Name | Operands | Description
:-: | --- | --- | ---
0 | `LocalVariable` | `property@ptr` | Loads `property` (UProperty) into GProperty and writes its value to the return value address.
1 | `InstanceVariable` | `property@ptr` | Loads `property` (UProperty) into GProperty; then reads an instance variable on the object into the return value address.
2 | `DefaultVariable` | `property@ptr` | Similar to InstanceVariable but reads from the class default object.
3 | `StateVariable` | `property@ptr` | -
4 | `EventStart_?` | unknown | Emitted as the first opcode of events.
5 | `Switch` | unknown | Switch on a couple values. Not sure how this works quite yet.
6 | `Jump` | `offset@u16` | Unconditional jump by 16-bit relative `offset` inside the current chunk of bytecode.
7 | `JumpIfNot` | `u16 insn` | Conditional jump by 16-bit `offset`. Jumps if `insn` returns zero.
8 | `Stop` | - | Sets the instruction pointer to the null pointer, which causes a crash.
9 | `Assert` | `cond@insn` | Logs an error message if `cond` is evaluated to zero.
10 | `Case` | unknown | Probably something to do with `Switch`, but I haven't reversed this yet.
11 | `Nothing` | - | Literally does nothing.
13 | `GotoLabel` | unknown | -
14 | `EatReturnValue` | unknown | -
15 | `Let` | unknown | -
16 | `DynArrayElement` | unknown | -
17 | `New` | unknown | -
18 | `ClassContext` | unknown | -
19 | `MetaCast` | unknown | -
20 | `LetBool` | unknown | -
22 | `EndFunctionParms` | unknown | -
23 | `Self` | unknown | -
25 | `Context` | unknown | -
26 | `ArrayElement` | unknown | -
27 | `VirtualFunction` | unknown | -
28 | `FinalFunction` | unknown | -
29 | `IntConst` | unknown | -
30 | `FloatConst` | unknown | -
31 | `StringConst` | unknown | -
32 | `ObjectConst` | unknown | -
33 | `NameConst` | unknown | -
34 | `RotationConst` | unknown | -
35 | `VectorConst` | unknown | -
36 | `ByteConst` | unknown | -
37 | `IntZero` | unknown | -
38 | `IntOne` | unknown | -
39 | `True` | unknown | -
40 | `False` | unknown | -
41 | `NativeParm` | unknown | -
42 | `NoObject` | unknown | -
44 | `IntConstByte` | unknown | -
45 | `BoolVariable` | unknown | -
46 | `DynamicCast` | unknown | -
47 | `Iterator` | unknown | -
48 | `IteratorPop` | unknown | -
50 | `StructCmpEq` | unknown | -
51 | `StructCmpNe` | unknown | -
52 | `UnicodeStringConst` | unknown | -
53 | `StructMember` | unknown | -
54 | `DynArrayLength` | unknown | -
55 | `GlobalFunction` | unknown | -
56 | `PrimitiveCast` | unknown | -
57 | `DynArrayInsert` | unknown | -
58 | `ReturnNothing` | unknown | -
59 | `EqualEqual_DelegateDelegate` | unknown | -
60 | `NotEqual_DelegateDelegate` | unknown | -
61 | `EqualEqual_DelegateFunction` | unknown | -
62 | `NotEqual_DelegateFunction` | unknown | -
63 | `EmptyDelegate` | unknown | -
64 | `DynArrayRemove` | unknown | -
65 | `DebugInfo` | unknown | -
66 | `DelegateFunction` | unknown | -
67 | `DelegateProperty` | unknown | -
68 | `LetDelegate` | unknown | -
69 | `Conditional` | unknown | -
70 | `DynArrayFind` | unknown | -
71 | `DynArrayFindStruct` | unknown | -
72 | `LocalOutVariable` | unknown | -
73 | `DefaultParmValue` | unknown | -
74 | `EmptyParmValue` | unknown | -
75 | `InstanceDelegate` | unknown | -
81 | `InterfaceContext` | unknown | -
82 | `InterfaceCast` | unknown | -
83 | `EndOfScript` | unknown | -
84 | `DynArrayAdd` | unknown | -
85 | `DynArrayAddItem` | unknown | -
86 | `DynArrayRemoveItem` | unknown | -
87 | `DynArrayInsertItem` | unknown | -
88 | `DynArrayIterator` | unknown | -
89 | `DynArraySort` | unknown | -
90 | `JumpIfNotEditorOnly` | unknown | -
96 | `HighNative0` | `n@byte` | `HighNative` instructions execute opcodes whose indices are above 255. This one's redundant to just running the low opcode `n`.
97 | `HighNative1` | `n@byte` | This one executes opcode `256 + n`.
98 | `HighNative2` | `n@byte` | This one executes opcode `512 + n`.
99 | `HighNative3` | `n@byte` | This one executes opcode `768 + n`.
100 | `HighNative4` | `n@byte` | This one executes opcode `1024 + n`.
101 | `HighNative5` | `n@byte` | This one executes opcode `1280 + n`.
102 | `HighNative6` | `n@byte` | This one executes opcode `1536 + n`.
103 | `HighNative7` | `n@byte` | This one executes opcode `1792 + n`.
104 | `HighNative8` | `n@byte` | This one executes opcode `2048 + n`.
105 | `HighNative9` | `n@byte` | This one executes opcode `2304 + n`.
106 | `HighNative10` | `n@byte` | This one executes opcode `2560 + n`.
107 | `HighNative11` | `n@byte` | This one executes opcode `2816 + n`.
108 | `HighNative12` | `n@byte` | This one executes opcode `3072 + n`.
109 | `HighNative13` | `n@byte` | This one executes opcode `3328 + n`.
110 | `HighNative14` | `n@byte` | This one executes opcode `3584 + n`.
111 | `HighNative15` | `n@byte` | This one executes opcode `3840 + n`. Having all the `HighNative` opcodes allows for execution of opcodes up to 4095.
112 | `Concat_StrStr` | unknown | -
113 | `GotoState` | unknown | -
114 | `EqualEqual_ObjectObject` | unknown | -
115 | `Less_StrStr` | unknown | -
116 | `Greater_StrStr` | unknown | -
117 | `Enable` | unknown | -
118 | `Disable` | unknown | -
119 | `NotEqual_ObjectObject` | unknown | -
120 | `LessEqual_StrStr` | unknown | -
121 | `GreaterEqual_StrStr` | unknown | -
122 | `EqualEqual_StrStr` | unknown | -
123 | `NotEqual_StrStr` | unknown | -
124 | `ComplementEqual_StrStr` | unknown | -
125 | `Len` | unknown | -
126 | `InStr` | unknown | -
127 | `Mid` | unknown | -
128 | `Left` | unknown | -
129 | `Not_PreBool` | unknown | -
130 | `AndAnd_BoolBool` | unknown | -
131 | `XorXor_BoolBool` | unknown | -
132 | `OrOr_BoolBool` | unknown | -
133 | `MultiplyEqual_ByteByte` | unknown | -
134 | `DivideEqual_ByteByte` | unknown | -
135 | `AddEqual_ByteByte` | unknown | -
136 | `SubtractEqual_ByteByte` | unknown | -
137 | `AddAdd_PreByte` | unknown | -
138 | `SubtractSubtract_PreByte` | unknown | -
139 | `AddAdd_Byte` | unknown | -
140 | `SubtractSubtract_Byte` | unknown | -
141 | `Complement_PreInt` | unknown | -
142 | `EqualEqual_RotatorRotator` | unknown | -
143 | `Subtract_PreInt` | unknown | -
144 | `Multiply_IntInt` | unknown | -
145 | `Divide_IntInt` | unknown | -
146 | `Add_IntInt` | unknown | -
147 | `Subtract_IntInt` | unknown | -
148 | `LessLess_IntInt` | unknown | -
149 | `GreaterGreater_IntInt` | unknown | -
150 | `Less_IntInt` | unknown | -
151 | `Greater_IntInt` | unknown | -
152 | `LessEqual_IntInt` | unknown | -
153 | `GreaterEqual_IntInt` | unknown | -
154 | `EqualEqual_IntInt` | unknown | -
155 | `NotEqual_IntInt` | unknown | -
156 | `And_IntInt` | unknown | -
157 | `Xor_IntInt` | unknown | -
158 | `Or_IntInt` | unknown | -
159 | `MultiplyEqual_IntFloat` | unknown | -
160 | `DivideEqual_IntFloat` | unknown | -
161 | `AddEqual_IntInt` | unknown | -
162 | `SubtractEqual_IntInt` | unknown | -
163 | `AddAdd_PreInt` | unknown | -
164 | `SubtractSubtract_PreInt` | unknown | -
165 | `AddAdd_Int` | unknown | -
166 | `SubtractSubtract_Int` | unknown | -
167 | `Rand` | unknown | -
168 | `At_StrStr` | unknown | -
169 | `Subtract_PreFloat` | unknown | -
170 | `MultiplyMultiply_FloatFloat` | unknown | -
171 | `Multiply_FloatFloat` | unknown | -
172 | `Divide_FloatFloat` | unknown | -
173 | `Percent_FloatFloat` | unknown | -
174 | `Add_FloatFloat` | unknown | -
175 | `Subtract_FloatFloat` | unknown | -
176 | `Less_FloatFloat` | unknown | -
177 | `Greater_FloatFloat` | unknown | -
178 | `LessEqual_FloatFloat` | unknown | -
179 | `GreaterEqual_FloatFloat` | unknown | -
180 | `EqualEqual_FloatFloat` | unknown | -
181 | `NotEqual_FloatFloat` | unknown | -
182 | `MultiplyEqual_FloatFloat` | unknown | -
183 | `DivideEqual_FloatFloat` | unknown | -
184 | `AddEqual_FloatFloat` | unknown | -
185 | `SubtractEqual_FloatFloat` | unknown | -
186 | `Abs` | unknown | -
187 | `Sin` | unknown | -
188 | `Cos` | unknown | -
189 | `Tan` | unknown | -
190 | `Atan` | unknown | -
191 | `Exp` | unknown | -
192 | `Loge` | unknown | -
193 | `Sqrt` | unknown | -
194 | `Square` | unknown | -
195 | `FRand` | unknown | -
196 | `GreaterGreaterGreater_IntInt` | unknown | -
197 | `IsA` | unknown | -
198 | `MultiplyEqual_ByteFloat` | unknown | -
199 | `Round` | unknown | -
201 | `Repl` | unknown | -
203 | `NotEqual_RotatorRotator` | unknown | -
210 | `ComplementEqual_FloatFloat` | unknown | -
211 | `Subtract_PreVector` | unknown | -
212 | `Multiply_VectorFloat` | unknown | -
213 | `Multiply_FloatVector` | unknown | -
214 | `Divide_VectorFloat` | unknown | -
215 | `Add_VectorVector` | unknown | -
216 | `Subtract_VectorVector` | unknown | -
217 | `EqualEqual_VectorVector` | unknown | -
218 | `NotEqual_VectorVector` | unknown | -
219 | `Dot_VectorVector` | unknown | -
220 | `Cross_VectorVector` | unknown | -
221 | `MultiplyEqual_VectorFloat` | unknown | -
222 | `DivideEqual_VectorFloat` | unknown | -
223 | `AddEqual_VectorVector` | unknown | -
224 | `SubtractEqual_VectorVector` | unknown | -
225 | `VSize` | unknown | -
226 | `Normal` | unknown | -
227 | `Normal2D` | unknown | -
228 | `VSizeSq` | unknown | -
229 | `GetAxes` | unknown | -
230 | `GetUnAxes` | unknown | -
231 | `LogInternal` | unknown | -
232 | `WarnInternal` | unknown | -
234 | `Right` | unknown | -
235 | `Caps` | unknown | -
236 | `Chr` | unknown | -
237 | `Asc` | unknown | -
238 | `Locs` | unknown | -
242 | `EqualEqual_BoolBool` | unknown | -
243 | `NotEqual_BoolBool` | unknown | -
244 | `FMin` | unknown | -
245 | `FMax` | unknown | -
246 | `FClamp` | unknown | -
247 | `Lerp` | unknown | -
249 | `Min` | unknown | -
250 | `Max` | unknown | -
251 | `Clamp` | unknown | -
252 | `VRand` | unknown | -
253 | `Percent_IntInt` | unknown | -
254 | `EqualEqual_NameName` | unknown | -
255 | `NotEqual_NameName` | unknown | -
258 | `ClassIsChildOf` | unknown | -
270 | `Add_QuatQuat` | unknown | -
271 | `Subtract_QuatQuat` | unknown | -
275 | `LessLess_VectorRotator` | unknown | -
276 | `GreaterGreater_VectorRotator` | unknown | -
279 | `Destroy_?` | unknown | First opcode in `AActor::OutsideWorldBounds`, `AActor::VolumeBasedDestroy`.
281 | `IsInState` | unknown | -
284 | `GetStateName` | unknown | -
287 | `Multiply_RotatorFloat` | unknown | -
288 | `Multiply_FloatRotator` | unknown | -
289 | `Divide_RotatorFloat` | unknown | -
290 | `MultiplyEqual_RotatorFloat` | unknown | -
291 | `DivideEqual_RotatorFloat` | unknown | -
296 | `Multiply_VectorVector` | unknown | -
297 | `MultiplyEqual_VectorVector` | unknown | -
299 | `SetRotation_?` | unknown | First opcode in `AController::SetLocation`, `AController::SetRotation`.
300 | `MirrorVectorByNormal` | unknown | -
316 | `Add_RotatorRotator` | unknown | -
317 | `Subtract_RotatorRotator` | unknown | -
318 | `AddEqual_RotatorRotator` | unknown | -
319 | `SubtractEqual_RotatorRotator` | unknown | -
320 | `RotRand` | unknown | -
322 | `ConcatEqual_StrStr` | unknown | -
323 | `AtEqual_StrStr` | unknown | -
324 | `SubtractEqual_StrStr` | unknown | -
514 | `LineOfSightTo_?` | unknown | First opcode in `AController::EnemyJustTeleported`.
536 | `SaveConfig` | unknown | -
1500 | `ProjectOnTo` | unknown | -
1501 | `IsZero` | unknown | -
3970 | `SetPhysics_?` | unknown | First opcode in `AActor::FellOutOfWorld`.

# Yarnbox extensions

Yarnbox reserves the following otherwise free opcodes for implementing its functionality:

Index | Name | Operands | Description
:-: | --- | --- | ---
4095 | `Unknown` | N/A | Marker emitted by the disassembler when it doesn't know how a certain opcode is encoded. Not an actual executable opcode, do not use.
