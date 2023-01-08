# How to read this table

The columns are:
- **Index** - index in the `GNatives` array.
- **Name** - name of the `exec` function corresponding to the opcode, suffixed with `_?` if the exact name is not known.
  - The name can be namespaced, like `Actor.MoveTo`, if the `exec` function lives inside a class
    that's not `UObject` (such as `AActor`.)
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
- `?a` - match `a`, but don't actually consume anything
- `!a` - match anything but `a`, but don't actually consume anything
- `opcode.X` - match the opcode `X`

Available rules are:
```
byte <- {0 .. 255}

u8 <- byte
u16 <- byte byte
u32 <- byte byte byte byte
u64 <- byte byte byte byte byte byte byte byte

// These rules are there for metadata reasons.
orel <- u16   // relative offset
oabs <- u16   // absolute offset
ptr <- u64    // any pointer
obj <- ptr    // UObject*
name <- u64   // FName

lowOpcode <- {0 .. 95, 112 .. 255}
highNative <- {96 .. 111}
opcode <- lowOpcode | highNative byte

// `operands` is specified by the Operands column in the table below.
insn <- opcode operands

fnargs <- insn* opcode.EndFunctionParms DebugInfo?
```
Rules may capture variables which can be referred to in the description, with the syntax `variable@rule`.

# Disclaimer

The following table may be incooabsmplete. Actual bytecode may contain instructions that are not (yet)
in this table; ie. instructions that are registered dynamically and do not appear in Ghidra.

As mentioned before, the actual names of certain dynamically registered instructions may be my best
guesses. These names are suffixed with `_?`.

# Instructions

Index | Name | Operands | Description
:-: | --- | --- | ---
0 | `LocalVariable` | `property@obj` | Loads `property` (`UProperty*`) into GProperty and writes its value to the return value address.
1 | `InstanceVariable` | `property@obj` | Loads `property` (`UProperty*`) into GProperty; then reads an instance variable on the object into the return value address.
2 | `DefaultVariable` | `property@obj` | Similar to InstanceVariable but reads from the class default object.
3 | `StateVariable` | `property@obj` | -
4 | `Return_?` | `value@insn` | So common that it's probably the return opcode. Looking at some functions' source code it would seem like the return opcode indeed; however I haven't found its handling code yet. Since it terminates the execution of a function it cannot be an `exec`.
5 | `Switch` | unknown | Switch on a couple values. Not sure how this works quite yet.
6 | `Jump` | `offset@oabs` | Unconditional jump to 16-bit absolute `offset` inside the current chunk of bytecode.
7 | `JumpIfNot` | `offset@oabs cond@insn` | Conditional jump to 16-bit absolute `offset`. Jumps if `cond` returns zero.
8 | `Stop` | - | Sets the instruction pointer to the null pointer, which causes a crash.
9 | `Assert` | `line@u16 category@u8 cond@insn` | Logs an error message if `cond` is evaluated to zero. The error message is `Assertion failed, line %i` with the format operand being `line`. `category` can be 0 to change the log category to `0x2f9` instead of `0x301`; I haven't discovered what all these categories are called yet.
10 | `Case` | unknown | Probably something to do with `Switch`, but I haven't reversed this yet.
11 | `Nothing` | - | Literally does nothing.
12 | unknown | unknown | -
13 | `GotoLabel` | `insn` | -
14 | `EatReturnValue` | unknown | -
15 | `Let` | `lvalue@insn rvalue@insn` | Evaluates `lvalue`, which should be an instruction that sets `GProperty`, `GPropObject`, and `GPropAddr` (such as one of the `*Variable` instructions.) If `GRuntimeUCFlags & 1` is set, evaluates `rvalue` passing `GPropAddr` directly as the return value. Otherwise it clears the Value Omitted flag and modifies an array's length to `rvalue`. (the `lvalue` is expected to reveal an array property.)
16 | `DynArrayElement` | `index@insn array@insn` | Loads an `array` property into `GProperty` and `GPropObject`, and sets `GPropAddr` to the array's base address offset by the given index. If the return value pointer is not null, reads the value from the array into the return value pointer.
17 | `New` | `outer@insn name@insn flags@insn class@insn template@insn` | Creates a new object with the given parameters. The types are `Object outer`, `String name`, `Int flags`, `Class class`, `Object template`.
18 | `ClassContext` | `this@insn jump@orel property@obj type@u8 then@insn` | Similar to `Context` (25), but `this` is expected to be a class, and `then` is executed on the class default object instead of `this`.
19 | `MetaCast` | `targetclass@obj fromclass@insn` | Performs a check that the class `fromclass` is or extends `targetclass`. If so, the class is the value returned. Otherwise returns `None`.
20 | `LetBool` | `lvalue@insn rvalue@insn` | Similar to `Let` but specifically for boolean and boolean array properties. Unlike `Let` it does not seem to have any special support for arrays, as far as I can tell it'll always write to the first element. Just like `BoolVariable` it coerces the written value to either 1 or 0 and ANDs the result with the `bBoolValueEnabled` thingamajig.
21 | `EndDefaultParm` | - | Sentinel value for a `DefaultParmValue` instruction.
22 | `EndFunctionParms` | N/A | Sentinel value for function arguments. Unlike the name suggests, it is actually for function arguments (values passed to the function by the caller) and not parameters (the variables inside the function that contain these values.) If reached, makes the instruction pointer go back by 1, probably to let an instruction dispatcher in a function call consume this value.
23 | `Self` | - | Sets the return value to `this`.
24 | unknown | unknown | -
25 | `Context` | `this@insn jump@orel property@obj type@u8 then@insn` | When successful, executes the expression `then` on the object `this`. When `this` is null however, reads `property` and `type` to figure out the return value's size and zero it out; `type` is used if `property` is null. Then jumps over `then`.
26 | `ArrayElement` | `index@insn array@insn` | Similar to `DynArrayElement`, but works on statically-sized arrays.
27 | `VirtualFunction` | `name@name fnargs` | Calls a function by its `name` (`FName`), indirectly.
28 | `FinalFunction` | `function@obj fnargs` | Calls a `function` (`UFunction*`) directly.
29 | `IntConst` | `x@u32` | Writes a `uint32_t` to the return value. The endianness is platform-specific.
30 | `FloatConst` | `x@u32` | Writes a `float` to the return value. The endianness is platform-specific.
31 | `StringConst` | `string@({1 .. 255}* 0)` | Writes a string constant to the return value (assumed to be an `FString`.)
32 | `ObjectConst` | `object@obj` | Writes a `UObject*` to the return value.
33 | `NameConst` | `name@name` | Writes a constant `FName` to the return value. Note that `FName` is a trivial 8 byte-long type.
34 | `RotationConst` | `pitch@u32 yaw@u32 roll@u32` | Writes a constant `FRotator` to the return value.
35 | `VectorConst` | `x@u32 y@u32 z@u32` | Writes a constant `FVector` to the return value.
36 | `ByteConst` | `x@byte` | Writes a constant `uint8_t` to the return value.
37 | `IntZero` | - | Writes the integer 0 to the return address.
38 | `IntOne` | - | Writes the integer 1 to the return address.
39 | `True` | - | Alias for `IntOne`.
40 | `False` | - | Alias for `IntZero`.
41 | `NativeParm` | `param@obj` | Loads a native parameter into `GPropAddr` and does *something*. I haven't discovered what yet. Either way, it's generated as part of `native` functions.
42 | `NoObject` | - | Returns `None`.
43 | - | - | Invalid opcode.
44 | `IntConstByte` | `x@u8` | Writes a constant `int` to the return value. This is used for small integers whose value does not exceed 255.
45 | `BoolVariable` | `lvalue@insn` | -
46 | `DynamicCast` | `class@u64 object@insn` | Attempts to cast an `object` into the target `class` (which can also be an interface.) If the cast fails - that is, the object is not of the given class or does not implement the given interface, returns null. Otherwise returns the casted object.
47 | `Iterator` | `insn orel (!opcode.Continue insn)* opcode.Continue` | -
48 | `IteratorPop` | - | -
49 | `Continue` | N/A | Emitted at the end of `Iterator` bodies. Signals to the iterator that the body should be executed from the beginning.
50 | `StructCmpEq` | unknown | -
51 | `StructCmpNe` | unknown | -
52 | `UnicodeStringConst` | unknown | -
53 | `StructMember` | `struct@obj field@obj u8 u8 insn` | -
54 | `DynArrayLength` | `array@insn` | Expects that `array` loads an array property into `GProperty`, `GPropObject`, and `GPropAddr`, then reads the length of the array into the return value.
55 | `GlobalFunction` | `name@name fnargs` | Calls a global function by name.
56 | `PrimitiveCast` | `type@u8 value@insn` | Performs a primitive cast of the given `type` on the provided `value`. See [primitive casts](#primitive-casts) for a list of available cast types. The function which performs the cast is obtained from the global array `GCasts`. Note that although I'm listing all casts as having the same `value@insn` operand, it would be possible for them to differ per cast type, as the operand is read by the primitive cast's `exec` function rather than `execPrimitiveCast` itself.
57 | `DynArrayInsert` | `array@insn index@insn num@insn DebugInfo?` | Inserts `num` zero elements at `index` inside the given `array`.
58 | `ReturnNothing` | `retval@obj` | Emits a warning that a non-void function reached its `EndOfScript` without hitting a `return` instruction. `retval` is a pointer to the `ReturnValue` property for this function, and is used to zero out the return value.
59 | `EqualEqual_DelegateDelegate` | `a@insn b@insn u8 DebugInfo?` | Compares two delegates `a == b`.
60 | `NotEqual_DelegateDelegate` | `a@insn b@insn u8 DebugInfo?` | Compares two delegates `a != b`.
61 | `EqualEqual_DelegateFunction` | `a@insn b@insn u8 DebugInfo?` | Alias for `EqualEqual_DelegateDelegate`.
62 | `NotEqual_DelegateFunction` | `a@insn b@insn u8 DebugInfo?` | Alias for `NotEqual_DelegateDelegate`.
63 | `EmptyDelegate` | - | Returns a delegate that does nothing.
64 | `DynArrayRemove` | `array@insn index@insn num@insn DebugInfo?` | Removes `num` elements at `index` from the given `array`.
65 | `DebugInfo` | unknown | -
66 | `DelegateFunction` | `u8 obj name fnargs` | Probably some sort of delegate function call; I don't know exactly how this works.
67 | `DelegateProperty` | `name@name property@obj` | Looks up a delegate by name. This is used when passing delegates as arguments to functions. Sometimes `property` is non-null, however I haven't figured out what it does yet.
68 | `LetDelegate` | `lvalue@insn rvalue@insn` | -
69 | `Conditional` | `cond@insn overt@orel true@insn overf@orel false@insn` | Evaluates `cond` to determine which instruction to execute. If `cond` is false, jumps `overt + 4` bytes forward to land directly on the `false` instruction and execute it. If `cond` is true, does not perform the jump, but rather executes `true` directly and takes a jump `overf + 2` bytes forward.
70 | `DynArrayFind` | `array@insn jump@orel value@insn u8 DebugInfo?` | Finds a `value` inside of an `array`. If `array` is null, the `value` is not evaluated (it's jumped over using the `jump` offset.)
71 | `DynArrayFindStruct` | `array@insn jump@orel name@insn value@insn u8 DebugInfo?` | Finds a struct whose field named `name` is equal to `value` inside an `array` of structs. If `array` is null, the `name` and `value` are not evaluated (they're jumped over using the `jump` offset.)
72 | `LocalOutVariable` | `property@obj` | Similar to `LocalVariable`, but does some stuff specifically to support passing variables via parameters. Not entirely sure what, though.
73 | `DefaultParmValue` | `jump@orel default@(!EndDefaultParm insn)+ EndDefaultParm` | Used for evaluating `optional` parameters. If the Value Omitted flag is not set (an argument to the function is provided,) the instruction pointer will be offset by `jump`. Otherwise all instructions matched by `default` will be executed. Unsets the flag after it's done executing, regardless if the default value was evaluated or not.
74 | `EmptyParmValue` | - | Sets the Value Omitted flag, and nulls out `GPropObject`, `GPropAddress`, and `GProperty`.
75 | `InstanceDelegate` | unknown | -
76 .. 80 | - | Invalid opcodes.
81 | `InterfaceContext` | `insn` | -
82 | `InterfaceCast` | - | Casts the context object to an interface using [primitive cast](#primitive-casts) 70 `ObjectToInterface`.
83 | `EndOfScript` | - | Sentinel placed at the end of all chunks of bytecode.
84 | `DynArrayAdd` | `array@insn num@insn DebugInfo?` | Adds `num` zero elements at the end of the `array`.
85 | `DynArrayAddItem` | `array@insn jumpoffset@orel item@insn u8 DebugInfo?` | Append an `item` to the end of the `array`. If `array` doesn't produce a property (`GPropAddr` is null,) perform a jump by `jumpoffset` bytes. The sentinel past `item` is unused. This is quite a complex instruction, I'm not sure if I got it totally right.
86 | `DynArrayRemoveItem` | `array@insn jumpoffset@orel item@insn u8 DebugInfo?` | Encoded exactly like `DynArrayAddItem`, except removes an item. I don't know how the removal mechanism works.
87 | `DynArrayInsertItem` | `array@insn jumpoffset@orel index@insn item@insn u8 DebugInfo?` | Like `DynArrayAddItem`, but for inserting at a given index.
88 | `DynArrayIterator` | `array@insn outelement@insn outindex@insn u8 jump@orel (!opcode.Continue insn)* opcode.Continue` | Iterates over all elements inside a dynamic array `array`. `outelement` is the location in which to store the current element, `outindex` is the location in which to store the current index. The latter is optional and `EmptyParmValue` can be used to not store the index. Followed by that is the length(?) of the iterator body, so that the iterator knows how many bytes to jump over when the iteration is that. After that there's the body itself, which is what the iterator is going to execute every iteration.
89 | `DynArraySort` | unknown | Even worse for this.
90 | `JumpIfNotEditorOnly` | `oabs` | Does nothing, since this is a game build.
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
112 | `Concat_StrStr` | `a@insn b@insn u8 DebugInfo?` | Concatenates two strings together.
113 | `GotoState` | unknown | -
114 | `EqualEqual_ObjectObject` | `a@insn b@insn u8 DebugInfo?` | Compares two objects `a == b`.
115 | `Less_StrStr` | `a@insn b@insn u8 DebugInfo?` | Compares two strings `a < b` lexicographically.
116 | `Greater_StrStr` | `a@insn b@insn u8 DebugInfo?` | Compares two strings `a > b` lexicographically.
117 | `Enable` | unknown | -
118 | `Disable` | unknown | -
119 | `NotEqual_ObjectObject` | `a@insn b@insn u8 DebugInfo?` | Compares two objects `a != b`.
120 | `LessEqual_StrStr` | `a@insn b@insn u8 DebugInfo?` | Compares two strings `a <= b` lexicographically.
121 | `GreaterEqual_StrStr` | `a@insn b@insn u8 DebugInfo?` | Compares two strings `a >= b` lexicographically.
122 | `EqualEqual_StrStr` | `a@insn b@insn u8 DebugInfo?` | Compares two strings `a == b`.
123 | `NotEqual_StrStr` | `a@insn b@insn u8 DebugInfo?` | Compares two strings `a != b`.
124 | `ComplementEqual_StrStr` | `a@insn b@insn u8 DebugInfo?` | Compares two strings `a == b`, but case-insensitively (this is the UnrealScript `a ~= b` operator.)
125 | `Len` | unknown | -
126 | `InStr` | unknown | -
127 | `Mid` | unknown | -
128 | `Left` | unknown | -
129 | `Not_PreBool` | `x@insn u8 DebugInfo?` | Boolean NOT; returns the inverse of `x`.
130 | `AndAnd_BoolBool` | `x@insn u8 jump@orel y@insn u8` | Short-circuiting boolean AND. If `x` is `False`, it'll jump the IP `jump` bytes forward, so as short-circuit over `y`. Otherwise it'll evaluate `y`. The `u8` sentinels are not used in any way; they can be any byte.
131 | `XorXor_BoolBool` | unknown | Alias for `NotEqual_BoolBool`.
132 | `OrOr_BoolBool` | `x@insn u8 jump@orel y@insn u8` | Short-circuiting boolean OR. If `x` is `True`, it'll jump the IP `jump` bytes forward, so as to short-circuit over `y`. Otherwise it'll evaluate `y`. The `u8` sentinels are not used in any way; they can be any byte.
133 | `MultiplyEqual_ByteByte` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Multiplies byte variable `lvalue` by `rvalue` in place.
134 | `DivideEqual_ByteByte` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Divides byte variable `lvalue` by `rvalue` in place.
135 | `AddEqual_ByteByte` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Adds byte `rvalue` to byte variable `lvalue` in place.
136 | `SubtractEqual_ByteByte` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Subtracts byte `rvalue` from byte variable `lvalue` in place.
137 | `AddAdd_PreByte` | `lvalue@insn` | Increments a byte variable in place. Returns the new value of the variable (after it was incremented.)
138 | `SubtractSubtract_PreByte` | `lvalue@insn` | Decrements a byte variable in place. Returns the new value of the variable (after it was decremented.)
139 | `AddAdd_Byte` | `lvalue@insn` | Increments a byte variable in place. Returns the old value of the variable (before it was incremented.)
140 | `SubtractSubtract_Byte` | `lvalue@insn` | Decrements a byte variable in place. Returns the old value of the variable (before it was decremented.)
141 | `Complement_PreInt` | `x@insn u8 DebugInfo?` | Bitwise NOTs the 32-bit integer `x`.
142 | `EqualEqual_RotatorRotator` | `x@insn y@insn u8 DebugInfo?` | Compares two rotators `x == y`.
143 | `Subtract_PreInt` | `x@insn u8 DebugInfo?` | Unary minus (negation) operator for 32-bit integers `-x`.
144 | `Multiply_IntInt` | `y@insn x@insn u8 DebugInfo?` | Multiplies two 32-bit integers and stores the result in the return value. NOTE: `y` is evaluated before `x`, but the operation is `x * y`.
145 | `Divide_IntInt` | `x@insn y@insn u8 DebugInfo?` | Divides 32-bit integers `x` by `y`. If `y` is zero, logs an error and the result is zero.
146 | `Add_IntInt` | `y@insn x@insn u8 DebugInfo?` | Adds two 32-bit integers and stores the result in the return value. NOTE: `y` is evaluated before `x`, but the operation is `x + y`.
147 | `Subtract_IntInt` | `x@insn y@insn u8 DebugInfo?` | Subtracts the 32-bit integer `y` from `x`.
148 | `LessLess_IntInt` | `x@insn y@insn u8 DebugInfo?` | Left-shifts the 32-bit integer `x` by `y` bits. `y` is moduloed by 32 so `1 << 32` is the same as `1 << 1`.
149 | `GreaterGreater_IntInt` | `x@insn y@insn u8 DebugInfo?` | Right-shifts the signed 32-bit integer `x` by `y` bits. `y` is moduloed by 32 so `1 >> 32` is the same as `1 >> 1`.
150 | `Less_IntInt` | `x@insn y@insn u8 DebugInfo?` | Compares two signed 32-bit integers `x < y`. The return value is widened to a `uint32_t`.
151 | `Greater_IntInt` | `x@insn y@insn u8 DebugInfo?` | Compares two signed 32-bit integers `x > y`. The return value is widened to a `uint32_t`.
152 | `LessEqual_IntInt` | `x@insn y@insn u8 DebugInfo?` | Compares two signed 32-bit integers `x <= y`. The return value is widened to a `uint32_t`.
153 | `GreaterEqual_IntInt` | `x@insn y@insn u8 DebugInfo?` | Compares two signed 32-bit integers `x >= y`. The return value is widened to a `uint32_t`.
154 | `EqualEqual_IntInt` | `x@insn y@insn u8 DebugInfo?` | Compares two signed 32-bit integers `x == y`. The return value is widened to a `uint32_t`.
155 | `NotEqual_IntInt` | `x@insn y@insn u8 DebugInfo?` | Compares two signed 32-bit integers `x != y`. The return value is widened to a `uint32_t`.
156 | `And_IntInt` | `x@insn y@insn u8 DebugInfo?` | Bitwise ANDs the two 32-bit integers `x` and `y`.
157 | `Xor_IntInt` | `x@insn y@insn u8 DebugInfo?` | Bitwise XORs the two 32-bit integers `x` and `y`.
158 | `Or_IntInt` | `x@insn y@insn u8 DebugInfo?` | Bitwise ORs the two 32-bit integers `x` and `y`.
159 | `MultiplyEqual_IntFloat` | `x@insn y@insn u8 DebugInfo?` | -
160 | `DivideEqual_IntFloat` | `x@insn y@insn u8 DebugInfo?` | -
161 | `AddEqual_IntInt` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Increments the 32-bit integer variable `lvalue` by `rvalue`. Returns the new value. If `lvalue` is not an lvalue, returns `lvalue + rvalue` with no side effects.
162 | `SubtractEqual_IntInt` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Decrements the 32-bit integer variable `lvalue` by `rvalue`. Returns the new value. If `lvalue` is not an lvalue, returns `lvalue - rvalue` with no side effects.
163 | `AddAdd_PreInt` | `lvalue@insn u8 DebugInfo?` | Prefix increment operator for ints `++a`. Increments the variable `lvalue` by one. Returns the new value (after incrementing.)
164 | `SubtractSubtract_PreInt` | `lvalue@insn u8 DebugInfo?` | Prefix decrement operator for ints `--a`. Increments the variable `lvalue` by one. Returns the new value (after decrementing.)
165 | `AddAdd_Int` | `lvalue@insn u8 DebugInfo?` | Postfix increment operator for ints `a++`. Increments the variable `lvalue` by one. Returns the old value (before incrementing.)
166 | `SubtractSubtract_Int` | `lvalue@insn u8 DebugInfo?` | Postfix decrement operator for ints `a--`. Increments the variable `lvalue` by one. Returns the old value (before decrementing.)
167 | `Rand` | unknown | -
168 | `At_StrStr` | `a@insn b@insn u8 DebugInfo?` | Joins two strings together with a space inbetween.
169 | `Subtract_PreFloat` | `x@insn u8 DebugInfo?` | Unary minus (negation) for floats `-x`.
170 | `MultiplyMultiply_FloatFloat` | `x@insn n@insn u8 DebugInfo?` | Exponentiation operator `x ** y` (C math function `powf`.) Raises `x` to the `n`-th power.
171 | `Multiply_FloatFloat` | `y@insn x@insn u8 DebugInfo?` | Multiplies two floats and stores the result in the return value. NOTE: `y` is evaluated before `x`, but the operation is `x * y`.
172 | `Divide_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Divides float `x` by `y`. If `y` is zero, logs an error and the result is as defined by IEEE 754.
173 | `Percent_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Returns the remainder of dividing `x` by `y` (this is not the same as modulo.) If `y` is zero, logs an error and the result is as defined by IEEE 754. The error message erroneously calls this operation modulo, even though it returns the remainder of division.
174 | `Add_FloatFloat` | `y@insn x@insn u8 DebugInfo?` | Adds two floats together and stores the result in the return value. NOTE: `y` is evaluated before `x`, but the operation is `x + y`.
175 | `Subtract_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Subtracts the float `y` from `x`.
176 | `Less_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Compares two floats `x < y`. The return value is widened to a `uint32_t`.
177 | `Greater_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Compares two floats `x > y`. The return value is widened to a `uint32_t`.
178 | `LessEqual_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Compares two floats `x <= y`. The return value is widened to a `uint32_t`.
179 | `GreaterEqual_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Compares two floats `x >= y`. The return value is widened to a `uint32_t`.
180 | `EqualEqual_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Compares two floats `x == y`. The return value is widened to a `uint32_t`.
181 | `NotEqual_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Compares two floats `x != y`. The return value is widened to a `uint32_t`.
182 | `MultiplyEqual_FloatFloat` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Multiplies a float in place `lvalue *= rvalue`. Returns the multiplied value. If `lvalue` is not an lvalue, returns `lvalue * rvalue` with no side effects.
183 | `DivideEqual_FloatFloat` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Divides a float in place `lvalue /= rvalue`. Returns the divided value. If `lvalue` is not an lvalue, returns `lvalue / rvalue` with no side effects. If `rvalue` is zero, logs an error message, and the result is as defined by IEEE 754.
184 | `AddEqual_FloatFloat` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Adds a float in place `lvalue += rvalue`. Returns the multiplied value. If `lvalue` is not an lvalue, returns `lvalue * rvalue` with no side effects.
185 | `SubtractEqual_FloatFloat` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Subtracts a float in place `lvalue -= rvalue`. Returns the multiplied value. If `lvalue` is not an lvalue, returns `lvalue * rvalue` with no side effects.
186 | `Abs` | `x@insn u8 DebugInfo?` | Returns the absolute value of a float.
187 | `Sin` | `x@insn u8 DebugInfo?` | Returns the sine of `x`.
188 | `Cos` | `x@insn u8 DebugInfo?` | Returns the cosine of `x`.
189 | `Tan` | `x@insn u8 DebugInfo?` | Returns the tangent of `x`.
190 | `Atan` | `x@insn u8 DebugInfo?` | Returns the arc tangent of `x`.
191 | `Exp` | `x@insn u8 DebugInfo?` | Returns the constant e raised to the power of `x`.
192 | `Loge` | `x@insn u8 DebugInfo?` | Returns the natural logarithm of `x`.
193 | `Sqrt` | `x@insn u8 DebugInfo?` | Returns the square root of `x`.
194 | `Square` | `x@insn u8 DebugInfo?` | Returns the square of `x`.
195 | `FRand` | `u8 DebugInfo?` | Returns a random float between 0 and 1.
196 | `GreaterGreaterGreater_IntInt` | `x@insn y@insn u8 DebugInfo?` | Right-shifts the unsigned 32-bit integer `x` by `y` bits. `y` is moduloed by 32 so `1 >>> 32` is the same as `1 >>> 1`.
197 | `IsA` | `classname@insn u8 DebugInfo?` | The `Object.IsA` function. Checks if the object is of a class with the given `classname` (`FName`), with respect to the inheritance hierarchy.
198 | `MultiplyEqual_ByteFloat` | unknown | -
199 | `Round` | unknown | -
200 | - | - | Hole.
201 | `Repl` | unknown | -
202 | - | - | Hole.
203 | `NotEqual_RotatorRotator` | `x@insn y@insn u8 DebugInfo?` | Compares two rotators `x != y`.
204 .. 209 | - | - | Hole.
210 | `ComplementEqual_FloatFloat` | `x@insn y@insn u8 DebugInfo?` | Approximate equality operator. Same as `abs(x - y) < 0.0001`.
211 | `Subtract_PreVector` | `x@insn u8 DebugInfo?` | Unary minus (negation) for floats `-x`.
212 | `Multiply_VectorFloat` | `x@insn y@insn u8 DebugInfo?` | Multiplies a vector by a float `x * y`.
213 | `Multiply_FloatVector` | `x@insn y@insn u8 DebugInfo?` | Same as above, with the argument order swapped.
214 | `Divide_VectorFloat` | `x@insn y@insn u8 DebugInfo?` | Divides a vector by a float `x / y`.
215 | `Add_VectorVector` | `x@insn y@insn u8 DebugInfo?` | Adds two vectors to each other `x + y`.
216 | `Subtract_VectorVector` | `x@insn y@insn u8 DebugInfo?` | Subtracts two vectors from each other `x - y`.
217 | `EqualEqual_VectorVector` | `x@insn y@insn u8 DebugInfo?` | Compares two vectors `x == y`.
218 | `NotEqual_VectorVector` | `x@insn y@insn u8 DebugInfo?` | Compares two vectors `x != y`.
219 | `Dot_VectorVector` | `x@insn y@insn u8 DebugInfo?` | Computes the dot product of two vectors `x` and `y`.
220 | `Cross_VectorVector` | `x@insn y@insn u8 DebugInfo?` | Computes the cross product of two vectors `x` and `y`.
221 | `MultiplyEqual_VectorFloat` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Multiplies the vector variable `lvalue` in place by `rvalue`.
222 | `DivideEqual_VectorFloat` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Divides the vector variable `lvalue` in place by `rvalue`.
223 | `AddEqual_VectorVector` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Adds the vector `rvalue` to the vector variable `lvalue` in place.
224 | `SubtractEqual_VectorVector` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Subtracts the vector `rvalue` from the vector variable `lvalue` in place.
225 | `VSize` | `vec@insn u8 DebugInfo?` | Returns the Euclidian length of the vector `vec`. This is the same as `Sqrt(vec.X * vec.X + vec.Y * vec.Y)`
226 | `Normal` | `vec@insn u8 DebugInfo?` | Returns the vector `vec`, normalized. This is the same as `vec / VSize(vec)`.
227 | `Normal2D` | `vec@insn u8 DebugInfo?` | -
228 | `VSizeSq` | `vec@insn u8 DebugInfo?` | Same as `VSize`, except the result is not square-rooted. As such this is equivalent to `vec.X * vec.X + vec.Y * vec.Y`.
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
242 | `EqualEqual_BoolBool` | `x@insn y@insn u8 DebugInfo?` | Compares two booleans `x == y`. The return value is widened to a `uint32_t`.
243 | `NotEqual_BoolBool` | `x@insn y@insn u8 DebugInfo?` | Compares two booleans `x != y`. The return value is widened to a `uint32_t`.
244 | `FMin` | `x@insn y@insn u8 DebugInfo?` | Returns the smaller of the two float values `x` and `y`.
245 | `FMax` | `x@insn y@insn u8 DebugInfo?` | Returns the larger of the two float values `x` and `y`.
246 | `FClamp` | `x@insn min@insn max@insn u8 DebugInfo?` | Returns the number `x` clamped to fit between the interval `min .. max`. This is the same as `FMax(FMin(x, max), min)`.
247 | `Lerp` | `a@insn b@insn t@insn` | Linearly interpolates between `a` and `b` using the factor `t`. This is the same as `a + t * (b - a)`.
248 | - | - | Hole.
249 | `Min` | unknown | -
250 | `Max` | unknown | -
251 | `Clamp` | unknown | -
252 | `VRand` | unknown | -
253 | `Percent_IntInt` | `x@insn y@insn u8 DebugInfo?` | Returns the remainder of dividing `x` by `y` (this is not the same as modulo.) If `y` is zero, logs an error and the result is zero. The log message calls this operation modulo even though it's not.
254 | `EqualEqual_NameName` | `x@insn y@insn u8 DebugInfo?` | Compares two `FName`s `x == y`. Effectively an alias for `EqualEqual_ObjectObject`, since `FName` and `UObject*` have the same size, so the compiler optimizes this away to share the same function.
255 | `NotEqual_NameName` | `x@insn y@insn u8 DebugInfo?` | Compares two `FName`s `x != y`. Effectively an alias for `NotEqual_ObjectObject`, since `FName` and `UObject*` have the same size, so the compiler optimizes this away to share the same function.
256 | `Actor.Sleep` | unknown | -
257 | - | - | Hole.
258 | `ClassIsChildOf` | `class@insn base@insn u8 DebugInfo?` | Returns whether the given `class` extends the class `base` by walking its inheritance hierarchy.
259 .. 260 | - | - | Hole.
261 | `Actor.FinishAnim` | unknown | -
262 | `Actor.SetCollision` | unknown | -
263 .. 265 | - | - | Hole.
266 | `Actor.Move` | unknown | -
267 | `Actor.SetLocation` | unknown | -
268 .. 269 | - | - | Hole.
270 | `Add_QuatQuat` | unknown | -
271 | `Subtract_QuatQuat` | unknown | -
272 | `Actor.SetOwner` | unknown | -
273 .. 274 | - | - | Hole.
275 | `LessLess_VectorRotator` | `x@insn y@insn u8 DebugInfo?` | -
276 | `GreaterGreater_VectorRotator` | `x@insn y@insn u8 DebugInfo?` | -
277 | `Actor.Trace` | unknown | -
278 | - | - | Hole.
279 | `Actor.Destroy` | unknown | -
280 | `Actor.SetTimer` | unknown | -
281 | `IsInState` | unknown | -
282 | - | - | Hole.
283 | `Actor.SetCollisionSize` | unknown | -
284 | `GetStateName` | unknown | -
285 .. 286 | - | - | Hole.
287 | `Multiply_RotatorFloat` | `x@insn y@insn u8 DebugInfo?` | -
288 | `Multiply_FloatRotator` | `x@insn y@insn u8 DebugInfo?` | -
289 | `Divide_RotatorFloat` | `x@insn y@insn u8 DebugInfo?` | -
290 | `MultiplyEqual_RotatorFloat` | `lvalue@insn rvalue@insn u8 DebugInfo?` | -
291 | `DivideEqual_RotatorFloat` | `lvalue@insn rvalue@insn u8 DebugInfo?` | -
292 .. 295 | - | - | Hole.
296 | `Multiply_VectorVector` | `x@insn y@insn u8 DebugInfo?` | Multiplies two vectors together component-wise.
297 | `MultiplyEqual_VectorVector` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Multiplies the vector stored in the variable `lvalue` by the vector `rvalue` in place.
298 | `Actor.SetBase` | unknown | -
299 | `Actor.SetRotation` | unknown | -
300 | `MirrorVectorByNormal` | unknown | -
301 .. 303 | - | - | Hole.
304 | `Actor.AllActors` | unknown | -
305 | `Actor.ChildActors` | unknown | -
306 | `Actor.BasedActors` | unknown | -
307 | `Actor.TouchingActors` | unknown | -
308 | - | - | Hole.
309 | `Actor.TracedActors` | unknown | -
310 | - | - | Hole.
311 | `Actor.VisibleActors` | unknown | -
312 | `Actor.VisibleCollidingActors` | unknown | -
313 | `Actor.DynamicActors` | unknown | -
314 .. 315 | - | - | Hole.
316 | `Add_RotatorRotator` | `x@insn y@insn u8 DebugInfo?` | -
317 | `Subtract_RotatorRotator` | `x@insn y@insn u8 DebugInfo?` | -
318 | `AddEqual_RotatorRotator` | `lvalue@insn rvalue@insn u8 DebugInfo?` | -
319 | `SubtractEqual_RotatorRotator` | `lvalue@insn rvalue@insn u8 DebugInfo?` | -
320 | `RotRand` | unknown | -
321 | `Actor.CollidingActors` | unknown | -
322 | `ConcatEqual_StrStr` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Concatenates a string `rvalue` onto another string `lvalue` in place.
323 | `AtEqual_StrStr` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Joins a string `rvalue` onto another string `lvalue` in place, adding a space inbetween.
324 | `SubtractEqual_StrStr` | `lvalue@insn rvalue@insn u8 DebugInfo?` | Removes all occurrences of a string `rvalue` from another string `lvalue` in place.
325 .. 383 | - | - | Hole.
384 | `Actor.PollSleep` | unknown | -
385 | `Actor.PollFinishAnim` | unknown | -
386 .. 499 | - | - | Hole.
500 | `Controller.MoveTo` | unknown | -
501 | `Controller.Unknown1_?` | - | There is a function pointer there but the debug symbol does not directly point to a function.
502 | `Controller.MoveToward` | unknown | -
503 | `Controller.Unknown2_?` | - | There is a function pointer there but the debug symbol does not directly point to a function.
504 .. 507 | - | - | Hole.
508 | `Controller.FinishRotation` | unknown | -
509 | `Controller.PollFinishRotation` | unknown | -
510 .. 511 | - | - | Hole.
512 | `Actor.MakeNoise` | unknown | -
513 | - | - | Hole.
514 | `Controller.LineOfSightTo` | unknown | -
515 .. 516 | - | - | Hole.
517 | `Controller.FindPathToward` | unknown | -
518 | `Controller.FindPathTo` | unknown | -
519 | - | - | Hole.
520 | `Controller.ActorReachable` | unknown | -
521 | `Controller.PointReachable` | unknown | -
522 .. 523 | - | - | Hole.
524 | `PlayerController.FindStairRotation` | unknown | -
525 | `Controller.FindRandomDest` | unknown | -
526 | `Controller.PickWallAdjust` | unknown | -
527 | `Controller.WaitForLanding` | unknown | -
528 | `Controller.PollWaitForLanding` | unknown | -
529 .. 530 | - | - | Hole.
531 | `Controller.PickTarget` | unknown | -
532 | `Actor.PlayerCanSeeMe` | unknown | -
533 | `Controller.CanSee` | unknown | -
534 .. 535 | - | - | Hole.
536 | `SaveConfig` | unknown | -
537 | `Controller.CanSeeByPoints` | unknown | -
538 .. 545 | - | - | Hole.
546 | `PlayerController.UpdateURL` | unknown | -
547 | `Controller.GetURLMap` | unknown | -
548 | `Controller.FastTrace` | unknown | -
549 .. 1499 | - | - | Hole.
1500 | `ProjectOnTo` | unknown | -
1501 | `IsZero` | unknown | -
1502 .. 3968 | - | - | Hole.
3969 | `Actor.MoveSmooth` | unknown | -
3970 | `Actor.SetPhysics` | unknown | -
3971 | `Actor.AutonomousPhysics` | unknown | -
3972 .. 4093 | - | - | Hole.

## Notes

- Instructions with the description "invalid opcode" will cause deserialization to fail with the
  error message `Bad expr token %02x`, with the opcode as its format argument.
- Certain opcodes have the name "unknown," which means that the opcode is valid but its name
  or functionality hasn't been uncovered yet.
  - Certain unknown opcodes are "unknown single-byte opcodes," which mean that they take no
    operands but their name or functionality hasn't been uncovered yet.
- Unary operators seem to have a `u8` sentinel that is not used nor checked in any way.
- `Iterator` is really weird: `UObject::execIterator` is, as far as I can tell, an empty function,
  which makes zero sense, since `UStruct::SerializeExpr` does some stuff to read an instruction and
  a 16-byte value. But the opcode clearly _does work_, the question is _how????_

### Function parameters

The evaluation of function parameters is a little complicated, because function parameters in
UnrealScript can be optional. Those are declared like so:
```unrealscript
function OptionalParmsExample(optional int x = 1, optional int y = x + 2)
{}
```
Note that the default value for a parameter can be _any expression_. I haven't verified if it lets
you refer to other parameters, but my suspicion given what I've reversed so far is that it _should_
be possible.

Then in function calls you can omit arguments, like so:
```unrealscript
function OptionalArgsExample()
{
    OptionalParmsExample(,);
}
```
The syntax is quite ugly and in my opinion totally unreadable, but it does the job.

Now onto the bytecode! The base parsing rule for function arguments is `fnargs`, which is shared
between all types of functions. As a refresher, here it is in all its glory:
```
endfnargs <- 22  // This is the EndFunctionParms opcode. Which is a misnomer, by the way.
fnargs <- (!endfnargs insn)* endfnargs DebugInfo?
```
Function call instructions are structured like `opcode fnexpr fnargs`. `fnexpr` is specific to
each function call; see the **Operands** column of each `*Function` instruction for reference.

Each function argument can be any expression, but one instruction is used specifically for omitted
arguments - `EmptyParmValue`. This instruction nulls out the current property and sets the Value
Omitted flag to signal that no value was given for the parameter.
Whenever this flag is set, the corresponding parameter's `DefaultParmValue` instruction executes
zero or more instructions to initialize the parameter. If this flag is not set however, the entire
section that initializes the parameter is skipped over.

## Yarnbox extensions

Yarnbox reserves the following otherwise free opcodes for implementing its functionality:

Index | Name | Operands | Description
:-: | --- | --- | ---
4094 | `OutOfBounds` | N/A | Marker emitted by the disassembler in case it reads an out-of-bounds opcode.
4095 | `Unknown` | N/A | Marker emitted by the disassembler when it doesn't know how a certain opcode is encoded.

# Flags

The VM keeps a bunch of flags in its global state, inside the `GRuntimeUCFlags` global variable.
Here's a list of all known flags. The names are not accurate to how they may appear in the Unreal
Engine source code because... well, I don't have the source code.

Bit | Name | Description
:-: | --- | ---
0 | unknown | -
1 | Value Omitted | Unset for each parameter in a function; may be set by arguments that do not provide a value through the `EmptyParmValue` instruction.
2 | unknown | This is set by `StructMember` and only ever used by `HatLogRedirector`. I'm guessing it may be Hat in Time-specific.

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
