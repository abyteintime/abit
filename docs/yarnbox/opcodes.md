# How to read this table

The columns are:
- **Index** - index in the `GNatives` array.
- **Name** - name of the `exec` function corresponding to the opcode, suffixed with `_?` if the exact name is not (yet) known.
- **Description** - informal description of the opcode's behavior.
- **Operands** - how the opcode's operands are encoded in the bytecode.
  Lack of operands is denoted with a dash `-`, and operands that haven't been reversed yet are denoted with the word `unknown`.

## Encoding

This document PEG-like language to describe how instructions are encoded formally.
- `123` - match an exact byte
- `{}` - match a byte from a given set. Possible elements are:
  - `123` - a single byte
  - `1 .. 10` - a range of bytes
- `rule <- abc` - define a rule
- `rule` - match a rule
- `(rule)` - grouping
- `a b` - match rule `a` then rule `b`
- `a?` - match `a` optionally (0 or 1 occurrence)
- `a*` - match 0 or more occurrences of `a`
- `a+` - match 1 or more occurrences of `a`


Available rules are:
```
byte <- {0 .. 255}

u8 <- byte
u16 <- byte byte
u32 <- byte byte byte byte
u64 <- byte byte byte byte byte byte byte byte

ptr <- u64

lowOpcode <- {0 .. 95, 112 .. 255}
highNative <- {96 .. 111}
opcode <- lowOpcode | highNative byte

// `operands` is specified by the Operands column in the table below.
insn <- opcode operands
```
Rules may capture variables which can be referred to in the description, with the syntax `variable@rule`.

# Disclaimer

The following table may be incomplete. Actual bytecode may contain instructions that are not (yet)
in this table; ie. instructions that are registered dynamically and do not appear in Ghidra.

As mentioned before, the actual names of certain dynamically registered instructions may be my best
guesses. These names are suffixed with `_?`.

# Instructions

Index | Name | Operands | Description
:-: | --- | --- | ---
0 | `LocalVariable` | `property@ptr` | Loads `property` (`UProperty*`) into GProperty and writes its value to the return value address.
1 | `InstanceVariable` | `property@ptr` | Loads `property` (`UProperty*`) into GProperty; then reads an instance variable on the object into the return value address.
2 | `DefaultVariable` | `property@ptr` | Similar to InstanceVariable but reads from the class default object.
3 | `StateVariable` | `property@ptr` | -
4 | `EventStart_?` | unknown | Emitted as the first opcode of events.
5 | `Switch` | unknown | Switch on a couple values. Not sure how this works quite yet.
6 | `Jump` | `offset@u16` | Unconditional jump by 16-bit relative `offset` inside the current chunk of bytecode.
7 | `JumpIfNot` | `offset@u16 cond@insn` | Conditional jump by 16-bit `offset`. Jumps if `cond` returns zero.
8 | `Stop` | - | Sets the instruction pointer to the null pointer, which causes a crash.
9 | `Assert` | `cond@insn` | Logs an error message if `cond` is evaluated to zero.
10 | `Case` | unknown | Probably something to do with `Switch`, but I haven't reversed this yet.
11 | `Nothing` | - | Literally does nothing.
12 | - | - | Hole.
13 | `GotoLabel` | unknown | -
14 | `EatReturnValue` | unknown | -
15 | `Let` | `lvalue@insn rvalue@insn` | Evaluates `lvalue`, which should be an instruction that sets `GProperty`, `GPropObject`, and `GPropAddr` (such as one of the `*Variable` instructions.) If `GRuntimeUCFlags & 1` is set, evaluates `rvalue` passing `GPropAddr` directly as the return value. Otherwise it clears `GRuntimeUCFlags & 2` and modifies an array's length to `rvalue`. (the `lvalue` is expected to reveal an array property.)
16 | `DynArrayElement` | `index@insn array@insn` | Not sure about the order of arguments. Loads an `array` property into `GProperty` and `GPropObject`, and sets `GPropAddr` to the array's base address offset by the given index. If the return value pointer is not null, reads the value from the array into the return value pointer.
17 | `New` | unknown | -
18 | `ClassContext` | unknown | -
19 | `MetaCast` | `targetclass@u64 fromclass@insn` | Performs a check that the class `fromclass` is or extends `targetclass`. If so, the class is the value returned. Otherwise returns `None`.
20 | `LetBool` | `lvalue@insn rvalue@insn` | Similar to `Let` but specifically for boolean and boolean array properties. Unlike `Let` it does not seem to have any special support for arrays, as far as I can tell it'll always write to the first element. Just like `BoolVariable` it coerces the written value to either 1 or 0 and ANDs the result with the `bBoolValueEnabled` thingamajig.
21 | `EndDefaultParm` | N/A | Sentinel value for a `DefaultParmValue` instruction.
22 | `EndFunctionParms` | unknown | -
23 | `Self` | - | Sets the return value to `this`.
24 | - | - | Hole.
25 | `Context` | unknown | -
26 | `ArrayElement` | unknown | -
27 | `VirtualFunction` | unknown | -
28 | `FinalFunction` | unknown | -
29 | `IntConst` | `x@u32` | Writes a `uint32_t` to the return value. The endianness is platform-specific.
30 | `FloatConst` | `x@u32` | Writes a `float` to the return value. The endianness is platform-specific.
31 | `StringConst` | `string@({1 .. 255}* 0)` | Writes a string constant to the return value (assumed to be an `FString`.)
32 | `ObjectConst` | `object@ptr` | Writes a `UObject*` to the return value.
33 | `NameConst` | `name@u64` | Writes a constant `FName` to the return value. Note that `FName` is a trivial 8 byte-long type.
34 | `RotationConst` | `pitch@u32 yaw@u32 roll@u32` | Writes a constant `FRotator` to the return value.
35 | `VectorConst` | `x@u32 y@u32 z@u32` | Writes a constant `FVector` to the return value.
36 | `ByteConst` | `x@byte` | Writes a constant `uint8_t` to the return value.
37 | `IntZero` | - | Writes the integer 0 to the return address.
38 | `IntOne` | - | Writes the integer 1 to the return address.
39 | `True` | - | Alias for `IntOne`.
40 | `False` | - | Alias for `IntZero`.
41 | `NativeParm` | `param@ptr` | Loads a native parameter into `GPropAddr` and does *something*. I haven't discovered what yet. Either way, it's generated as part of `native` functions.
42 | `NoObject` | unknown | -
43 | - | - | Hole.
44 | `IntConstByte` | unknown | -
45 | `BoolVariable` | `property@u64` | Loads `property` (`UProperty*`) into GProperty and writes its value to the return value address. Unlike other `*Variable` functions, coerces the property's value to a boolean (either 1 or 0, but not anything else.) ANDs the result with one of the property's fields, but I haven't reverse engineered what it is yet (I assume something like `bBoolValueEnabled`.)
46 | `DynamicCast` | `class@u64 object@insn` | Attempts to cast an `object` into the target `class` (which can also be an interface.) If the cast fails - that is, the object is not of the given class or does not implement the given interface, returns null. Otherwise returns the casted object.
47 | `Iterator` | unknown | -
48 | `IteratorPop` | unknown | -
49 | - | - | Hole.
50 | `StructCmpEq` | unknown | -
51 | `StructCmpNe` | unknown | -
52 | `UnicodeStringConst` | unknown | -
53 | `StructMember` | unknown | -
54 | `DynArrayLength` | `array@insn` | Expects that `array` loads an array property into `GProperty`, `GPropObject`, and `GPropAddr`, then reads the length of the array into the return value.
55 | `GlobalFunction` | unknown | -
56 | `PrimitiveCast` | `type@u8` | Performs a primitive cast with the given `type`. See [primitive casts](#primitive-casts) for a list of available cast types. The function which performs the cast is obtained from the global array `GCasts`.
57 | `DynArrayInsert` | `array@insn index@insn num@insn DebugInfo?` | Inserts `num` zero elements at `index` inside the given `array`.
58 | `ReturnNothing` | unknown | -
59 | `EqualEqual_DelegateDelegate` | unknown | -
60 | `NotEqual_DelegateDelegate` | unknown | -
61 | `EqualEqual_DelegateFunction` | unknown | -
62 | `NotEqual_DelegateFunction` | unknown | -
63 | `EmptyDelegate` | unknown | -
64 | `DynArrayRemove` | `array@insn index@insn num@insn DebugInfo?` | Removes `num` elements at `index` from the given `array`.
65 | `DebugInfo` | unknown | -
66 | `DelegateFunction` | unknown | -
67 | `DelegateProperty` | unknown | -
68 | `LetDelegate` | unknown | -
69 | `Conditional` | unknown | -
70 | `DynArrayFind` | unknown | -
71 | `DynArrayFindStruct` | unknown | -
72 | `LocalOutVariable` | unknown | -
73 | `DefaultParmValue` | `jump@u16 default@(!EndDefaultParm insn)+ EndDefaultParm` | Used for evaluating `optional` parameters. If `GRuntimeUCFlags & 0x2` is not set, the instruction pointer will be offset by `jump`. Otherwise all instructions matched by `default` will be executed. Unsets the flag after it's done executing, regardless if the default value was evaluated or not.
74 | `EmptyParmValue` | unknown | -
75 | `InstanceDelegate` | unknown | -
76 .. 80 | - | - | Hole.
81 | `InterfaceContext` | unknown | -
82 | `InterfaceCast` | - | Casts the context object to an interface using [primitive cast](#primitive-casts) 70 `ObjectToInterface`.
83 | `EndOfScript` | unknown | -
84 | `DynArrayAdd` | `array@insn num@insn DebugInfo?` | Adds `num` zero elements at the end of the `array`.
85 | `DynArrayAddItem` | `array@insn jumpoffset@u16 item@insn u8 DebugInfo?` | Append an `item` to the end of the `array`. If `array` doesn't produce a property (`GPropAddr` is null,) perform a jump by `jumpoffset` bytes. The sentinel past `item` is unused. This is quite a complex instruction, I'm not sure if I got it totally right.
86 | `DynArrayRemoveItem` | `array@insn jumpoffset@u16 item@insn u8 DebugInfo?` | Encoded exactly like `DynArrayAddItem`, except removes an item. I don't know how the removal mechanism works.
87 | `DynArrayInsertItem` | `array@insn jumpoffset@u16 index@insn item@insn u8 DebugInfo?` | Like `DynArrayAddItem`, but for inserting at a given index.
88 | `DynArrayIterator` | unknown | The encoding of this is _wild._
89 | `DynArraySort` | unknown | Even worse for this.
90 | `JumpIfNotEditorOnly` | `u16` | Does nothing, since this is a game build.
91 .. 95 | - | - | Hole.
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
129 | `Not_PreBool` | `x@insn DebugInfo?` | Boolean NOT; returns the inverse of `x`.
130 | `AndAnd_BoolBool` | `x@insn y@insn` | Short-circuiting boolean AND. Will not evaluate `y` if `x` is `False`.
131 | `XorXor_BoolBool` | unknown | -
132 | `OrOr_BoolBool` | `x@insn y@insn` | Short-circuiting boolean OR. Will not evaluate `y` if `x` is `True`.
133 | `MultiplyEqual_ByteByte` | unknown | -
134 | `DivideEqual_ByteByte` | unknown | -
135 | `AddEqual_ByteByte` | unknown | -
136 | `SubtractEqual_ByteByte` | unknown | -
137 | `AddAdd_PreByte` | unknown | -
138 | `SubtractSubtract_PreByte` | unknown | -
139 | `AddAdd_Byte` | unknown | -
140 | `SubtractSubtract_Byte` | unknown | -
141 | `Complement_PreInt` | `x@insn DebugInfo?` | Bitwise NOTs the 32-bit integer `x`.
142 | `EqualEqual_RotatorRotator` | unknown | -
143 | `Subtract_PreInt` | `x@insn DebugInfo?` | Unary minus (negation) operator for 32-bit integers `-x`.
144 | `Multiply_IntInt` | `y@insn x@insn DebugInfo?` | Multiplies two 32-bit integers and stores the result in the return value. NOTE: `y` is evaluated before `x`, but the operation is `x * y`.
145 | `Divide_IntInt` | `x@insn y@insn DebugInfo?` | Divides 32-bit integers `x` by `y`. If `y` is zero, logs an error and the result is zero.
146 | `Add_IntInt` | `y@insn x@insn DebugInfo?` | Adds two 32-bit integers and stores the result in the return value. NOTE: `y` is evaluated before `x`, but the operation is `x + y`.
147 | `Subtract_IntInt` | `x@insn y@insn DebugInfo?` | Subtracts the 32-bit integer `y` from `x`.
148 | `LessLess_IntInt` | `x@insn y@insn DebugInfo?` | Left-shifts the 32-bit integer `x` by `y` bits. `y` is moduloed by 32 so `1 << 32` is the same as `1 << 1`.
149 | `GreaterGreater_IntInt` | `x@insn y@insn DebugInfo?` | Right-shifts the signed 32-bit integer `x` by `y` bits. `y` is moduloed by 32 so `1 >> 32` is the same as `1 >> 1`.
150 | `Less_IntInt` | `x@insn y@insn DebugInfo?` | Compares two signed 32-bit integers `x < y`. The return value is widened to a `uint32_t`.
151 | `Greater_IntInt` | `x@insn y@insn DebugInfo?` | Compares two signed 32-bit integers `x > y`. The return value is widened to a `uint32_t`.
152 | `LessEqual_IntInt` | `x@insn y@insn DebugInfo?` | Compares two signed 32-bit integers `x <= y`. The return value is widened to a `uint32_t`.
153 | `GreaterEqual_IntInt` | `x@insn y@insn DebugInfo?` | Compares two signed 32-bit integers `x >= y`. The return value is widened to a `uint32_t`.
154 | `EqualEqual_IntInt` | `x@insn y@insn DebugInfo?` | Compares two signed 32-bit integers `x == y`. The return value is widened to a `uint32_t`.
155 | `NotEqual_IntInt` | `x@insn y@insn DebugInfo?` | Compares two signed 32-bit integers `x != y`. The return value is widened to a `uint32_t`.
156 | `And_IntInt` | `x@insn y@insn DebugInfo?` | Bitwise ANDs the two 32-bit integers `x` and `y`.
157 | `Xor_IntInt` | `x@insn y@insn DebugInfo?` | Bitwise XORs the two 32-bit integers `x` and `y`.
158 | `Or_IntInt` | `x@insn y@insn DebugInfo?` | Bitwise ORs the two 32-bit integers `x` and `y`.
159 | `MultiplyEqual_IntFloat` | unknown | -
160 | `DivideEqual_IntFloat` | unknown | -
161 | `AddEqual_IntInt` | `lvalue@insn rvalue@insn DebugInfo?` | Increments the 32-bit integer variable `lvalue` by `rvalue`. Returns the new value. If `lvalue` is not an lvalue, returns `lvalue + rvalue` with no side effects.
162 | `SubtractEqual_IntInt` | `lvalue@insn rvalue@insn DebugInfo?` | Decrements the 32-bit integer variable `lvalue` by `rvalue`. Returns the new value. If `lvalue` is not an lvalue, returns `lvalue - rvalue` with no side effects.
163 | `AddAdd_PreInt` | `lvalue@insn DebugInfo?` | Prefix increment operator for ints `++a`. Increments the variable `lvalue` by one. Returns the new value (after incrementing.)
164 | `SubtractSubtract_PreInt` | `lvalue@insn DebugInfo?` | Prefix decrement operator for ints `--a`. Increments the variable `lvalue` by one. Returns the new value (after decrementing.)
165 | `AddAdd_Int` | `lvalue@insn DebugInfo?` | Postfix increment operator for ints `a++`. Increments the variable `lvalue` by one. Returns the old value (before incrementing.)
166 | `SubtractSubtract_Int` | `lvalue@insn DebugInfo?` | Postfix decrement operator for ints `a--`. Increments the variable `lvalue` by one. Returns the old value (before decrementing.)
167 | `Rand` | unknown | -
168 | `At_StrStr` | unknown | -
169 | `Subtract_PreFloat` | `x@insn DebugInfo?` | Unary minus (negation) for floats `-x`.
170 | `MultiplyMultiply_FloatFloat` | `x@insn n@insn DebugInfo?` | Exponentiation operator `x ** y` (C math function `powf`.) Raises `x` to the `n`-th power.
171 | `Multiply_FloatFloat` | `y@insn x@insn DebugInfo?` | Multiplies two floats and stores the result in the return value. NOTE: `y` is evaluated before `x`, but the operation is `x * y`.
172 | `Divide_FloatFloat` | `x@insn y@insn DebugInfo?` | Divides float `x` by `y`. If `y` is zero, logs an error and the result is as defined by IEEE 754.
173 | `Percent_FloatFloat` | `x@u32 y@u32 DebugInfo?` | Returns the remainder of dividing `x` by `y` (this is not the same as modulo.) If `y` is zero, logs an error and the result is as defined by IEEE 754. The error message erroneously calls this operation modulo, even though it returns the remainder of division.
174 | `Add_FloatFloat` | `y@insn x@insn DebugInfo?` | Adds two floats together and stores the result in the return value. NOTE: `y` is evaluated before `x`, but the operation is `x + y`.
175 | `Subtract_FloatFloat` | `x@insn y@insn DebugInfo?` | Subtracts the float `y` from `x`.
176 | `Less_FloatFloat` | `x@insn y@insn DebugInfo?` | Compares two floats `x < y`. The return value is widened to a `uint32_t`.
177 | `Greater_FloatFloat` | `x@insn y@insn DebugInfo?` | Compares two floats `x > y`. The return value is widened to a `uint32_t`.
178 | `LessEqual_FloatFloat` | `x@insn y@insn DebugInfo?` | Compares two floats `x <= y`. The return value is widened to a `uint32_t`.
179 | `GreaterEqual_FloatFloat` | `x@insn y@insn DebugInfo?` | Compares two floats `x >= y`. The return value is widened to a `uint32_t`.
180 | `EqualEqual_FloatFloat` | `x@insn y@insn DebugInfo?` | Compares two floats `x == y`. The return value is widened to a `uint32_t`.
181 | `NotEqual_FloatFloat` | `x@insn y@insn DebugInfo?` | Compares two floats `x != y`. The return value is widened to a `uint32_t`.
182 | `MultiplyEqual_FloatFloat` | `lvalue@insn rvalue@insn DebugInfo?` | Multiplies a float in place `lvalue *= rvalue`. Returns the multiplied value. If `lvalue` is not an lvalue, returns `lvalue * rvalue` with no side effects.
183 | `DivideEqual_FloatFloat` | `lvalue@insn rvalue@insn DebugInfo?` | Divides a float in place `lvalue /= rvalue`. Returns the divided value. If `lvalue` is not an lvalue, returns `lvalue / rvalue` with no side effects. If `rvalue` is zero, logs an error message, and the result is as defined by IEEE 754.
184 | `AddEqual_FloatFloat` | `lvalue@insn rvalue@insn DebugInfo?` | Adds a float in place `lvalue += rvalue`. Returns the multiplied value. If `lvalue` is not an lvalue, returns `lvalue * rvalue` with no side effects.
185 | `SubtractEqual_FloatFloat` | `lvalue@insn rvalue@insn DebugInfo?` | Subtracts a float in place `lvalue -= rvalue`. Returns the multiplied value. If `lvalue` is not an lvalue, returns `lvalue * rvalue` with no side effects.
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
196 | `GreaterGreaterGreater_IntInt` | `x@insn y@insn DebugInfo?` | Right-shifts the unsigned 32-bit integer `x` by `y` bits. `y` is moduloed by 32 so `1 >>> 32` is the same as `1 >>> 1`.
197 | `IsA` | unknown | -
198 | `MultiplyEqual_ByteFloat` | unknown | -
199 | `Round` | unknown | -
200 | - | - | Hole.
201 | `Repl` | unknown | -
202 | - | - | Hole.
203 | `NotEqual_RotatorRotator` | unknown | -
204 .. 209 | - | - | Hole.
210 | `ComplementEqual_FloatFloat` | `x@insn y@insn DebugInfo?` | Approximate equality operator. Same as `abs(x - y) < 0.0001`.
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
233 | - | - | Hole.
234 | `Right` | unknown | -
235 | `Caps` | unknown | -
236 | `Chr` | unknown | -
237 | `Asc` | unknown | -
238 | `Locs` | unknown | -
239 .. 241 | - | - | Hole.
242 | `EqualEqual_BoolBool` | `x@insn y@insn DebugInfo?` | Compares two booleans `x == y`. The return value is widened to a `uint32_t`.
243 | `NotEqual_BoolBool` | `x@insn y@insn DebugInfo?` | Compares two booleans `x != y`. The return value is widened to a `uint32_t`.
244 | `FMin` | unknown | -
245 | `FMax` | unknown | -
246 | `FClamp` | unknown | -
247 | `Lerp` | unknown | -
248 | - | - | Hole.
249 | `Min` | unknown | -
250 | `Max` | unknown | -
251 | `Clamp` | unknown | -
252 | `VRand` | unknown | -
253 | `Percent_IntInt` | `x@insn y@insn DebugInfo?` | Returns the remainder of dividing `x` by `y` (this is not the same as modulo.) If `y` is zero, logs an error and the result is zero. The log message calls this operation modulo even though it's not.
254 | `EqualEqual_NameName` | unknown | -
255 | `NotEqual_NameName` | unknown | -
256 .. 257 | - | - | Hole.
258 | `ClassIsChildOf` | unknown | -
259 .. 269 | - | - | Hole.
270 | `Add_QuatQuat` | unknown | -
271 | `Subtract_QuatQuat` | unknown | -
272 .. 274 | - | - | Hole.
275 | `LessLess_VectorRotator` | unknown | -
276 | `GreaterGreater_VectorRotator` | unknown | -
277 .. 278 | - | - | Hole.
279 | `Destroy_?` | unknown | First opcode in `AActor::OutsideWorldBounds`, `AActor::VolumeBasedDestroy`.
280 | - | - | Hole.
281 | `IsInState` | unknown | -
282 .. 283 | - | - | Hole.
284 | `GetStateName` | unknown | -
285 .. 286 | - | - | Hole.
287 | `Multiply_RotatorFloat` | unknown | -
288 | `Multiply_FloatRotator` | unknown | -
289 | `Divide_RotatorFloat` | unknown | -
290 | `MultiplyEqual_RotatorFloat` | unknown | -
291 | `DivideEqual_RotatorFloat` | unknown | -
292 .. 295 | - | - | Hole.
296 | `Multiply_VectorVector` | unknown | -
297 | `MultiplyEqual_VectorVector` | unknown | -
298 | - | - | Hole.
299 | `SetRotation_?` | unknown | First opcode in `AController::SetLocation`, `AController::SetRotation`.
300 | `MirrorVectorByNormal` | unknown | -
301 .. 315 | - | - | Hole.
316 | `Add_RotatorRotator` | unknown | -
317 | `Subtract_RotatorRotator` | unknown | -
318 | `AddEqual_RotatorRotator` | unknown | -
319 | `SubtractEqual_RotatorRotator` | unknown | -
320 | `RotRand` | unknown | -
321 | - | - | Hole.
322 | `ConcatEqual_StrStr` | unknown | -
323 | `AtEqual_StrStr` | unknown | -
324 | `SubtractEqual_StrStr` | unknown | -
325 .. 513 | - | - | Hole.
514 | `LineOfSightTo_?` | unknown | First opcode in `AController::EnemyJustTeleported`.
515 .. 535 | - | - | Hole.
536 | `SaveConfig` | unknown | -
537 .. 1499 | - | - | Hole.
1500 | `ProjectOnTo` | unknown | -
1501 | `IsZero` | unknown | -
1502 .. 3969 | - | - | Hole.
3970 | `SetPhysics_?` | unknown | First opcode in `AActor::FellOutOfWorld`.
3971 .. 4093 | - | - | Hole.

## Yarnbox extensions

Yarnbox reserves the following otherwise free opcodes for implementing its functionality:

Index | Name | Operands | Description
:-: | --- | --- | ---
4094 | `OutOfBounds` | N/A | Marker emitted by the disassembler in case it reads an out-of-bounds opcode.
4095 | `Unknown` | N/A | Marker emitted by the disassembler when it doesn't know how a certain opcode is encoded. Not an actual executable opcode, do not use.

# Primitive casts

The `PrimitiveCast` opcode supports up to 255 different casts. Here is a table listing all the
known ones:

Index | Name | Comment
:-: | --- | ---
0 .. 53 | - | Hole.
54 | `InterfaceToObject` | -
55 | `InterfaceToString` | -
56 | `InterfaceToBool` | -
57 | `RotatorToVector` | -
58 | `ByteToInt` | -
59 | `ByteToBool` | -
60 | `ByteToFloat` | -
61 | `IntToByte` | -
62 | `IntToBool` | -
63 | `IntToFloat` | -
64 | `BoolToByte` | -
65 | `BoolToInt` | -
66 | `BoolToFloat` | -
67 | `FloatToByte` | -
68 | `FloatToInt` | -
69 | `FloatToBool` | -
70 | `ObjectToInterface` | -
71 | `ObjectToBool` | -
72 | `NameToBool` | -
73 | `StringToByte` | -
74 | `StringToInt` | -
75 | `StringToBool` | -
76 | `StringToFloat` | -
77 | `StringToVector` | -
78 | `StringToRotator` | -
79 | `VectorToBool` | -
80 | `VectorToRotator` | -
81 | `RotatorToBool` | -
82 | `ByteToString` | -
83 | `IntToString` | -
84 | `BoolToString` | -
85 | `FloatToString` | -
86 | `ObjectToString` | -
87 | `NameToString` | -
88 | `VectorToString` | -
89 | `RotatorToString` | -
90 | `DelegateToString` | -
91 .. 95 | - | Hole.
96 | `StringToName` | -
97 .. 255 | - | Hole.
