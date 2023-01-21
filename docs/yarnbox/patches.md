# Patches

Reference documentation for Yarnbox patchfiles.

## Overview

Yarnbox can be instructed that a mod contains patches by putting a file called `Yarnbox.Patches.lua`
next to your mod's `modinfo.ini` file. A `Yarnbox.Patches.lua` file is called a _patchfile_ and
contains instructions for Yarnbox on what sort of patches to apply.

## Syntax

Patchfiles follow the usual [Lua] syntax. Of note is the fact that unlike UnrealScript,
**Lua identifiers are case-sensitive**, so you cannot mix casings freely.

[Lua]: https://lua.org/

### Lua overview

The most important bits of Lua syntax Yarnbox makes use of are tables and function calls.
Tables are the standard data structure in Lua, and allow for specifying sets of key-value mappings.
If you've dealt with hash maps or dictionaries in other languages, tables will be quite familiar.

Tables use the following syntax:

```lua
-- Assign a table to the global variable `myTable`.
myTable = {
    key1 = value1,
    key2 = value2,
}
```

Note that `key = value` pairs must be separated with commas. A trailing comma is allowed after the
last element, and for ease of maintenance I recommend always using it.

Yarnbox uses the convention of table keys being `camelCase`d.

Functions in Lua have three ways of calling them:

```lua
-- Call MyFunction with the arguments 1, 2, 3.
MyFunction(1, 2, 3)

-- Call a function with a single string argument.
-- The space between the function name and string is optional.
-- Strings may use single or double quotes; the syntaxes are equivalent.
-- By convention, single quotes and no space should be used when referring to objects in Yarnbox's
-- registry, such as functions, so as to mimic UnrealScript.
print "Hello!"
Function'Hello'

-- Call a function with a single table argument.
Dance {
    smugly = true,
    bpm = 128,
}
```

If a function returns another function, we can chain the function calls, leading to a pattern known
as _currying_. This pattern is used in Yarnbox for implementing patch specifiers (described later).

```lua
-- injection returns a function that when called with a table adds a new injection patch to
-- the list.
injection "Does some stuff" {
    -- ...
}
```

You can read more about Lua's syntax in [their manual](https://www.lua.org/manual/5.4/manual.html).

### Patch specifiers

Yarnbox uses the syntax `patchtype "comment" {}` to add new patches to the patchfile. Note that
although `patchtype` names are usually spelled with all lowercase letters, aliases exist that also
allow you to spell them with `camelCase` (like `patchType`) and `PascalCase` (like `PatchType`.)

The patch comment is primarily used for error reporting, so that you can easily identify which patch
failed to apply. By convention, the patch comment should be a single sentence describing what the
patch does.

### Object references

Yarnbox also exposes a bunch of functions that let you refer to various objects it collects into
its registry. As mentioned before, the general syntax is `Type'Path'`, to refer to an object of the
given `Type` at the given `Path`.

The `Path` is case-insensitive, so `Type'path'` and `Type'PATH'` refer to the same object. The
`Type` however, being a regular Lua identifier, is not, so `Type'path'` will work, but `type'path'`
will cause an error in your patchfile.

Paths follow one of the following forms:

- `Package.Class` - class path
- `Package.Class.Function` - function path
- `Package.Class.State` - state path
- `Package.Class.State.Function` - state function path

The following types of objects can be referred to:

- `Function` - refers to a function within a class or a state. This only works with function and
  function state paths. Example: `Function'HatinTimeGameContent.Hat_HUDMenu_HatKidDance.OnOpenHUD'`

## Patch types

The following sections describe which types of patches Yarnbox supports.

### Injections

Injections are the bread and butter of patching. By far they're going to be your most commonly used
tool when patching the game.

Injections allow you to modify existing functions in the game with custom behavior. Use cases
include changing otherwise hardcoded values, fixing bugs, and other small tweaks.

To specify a set of injections, of which all must be applied to work correctly, we use
`injectionset`:

```lua
injectionset "My set of injections" {
    {
        into = chunk,
        select = queryOrQueries,
        generate = bytecodeToGenerate,
    },
    {
        into = chunk,
        select = queryOrQueries,
        generate = bytecodeToGenerate,
    },
}
```

An injection set is all-or-nothing: if any one of the injections inside the set fails to apply, no
injections from the set will end up being applied.

To specify a single injection that can be applied independently, we use `injection`:

```lua
injection "My injection" {
    into = chunk,
    select = queryOrQueries,
    generate = bytecodeToGenerate,
}
```

The three properties of an injection are as follows:

- `into` - specifies the chunk of bytecode to inject into.
- `select` - specifies which spans of bytecode inside the chunk Yarnbox should inject into.
- `generate` - specifies what to generate at the selected spans of bytecode.

### Queries

The `select` property is a list of _queries_. Queries are used for selecting spans of bytecode
inside the chunk. Currently the only supported query type is an opcode query, which takes the form:

```lua
Opcode.type{occurrences}.pick
-- or
Opcode.type(AllOccurrences).pick
```

`type` should be replaced with an opcode type, a list of which can be found [here](opcodes.md).

`occurrences` should be replaced with a list of numbers. Each number in the list is an
_occurrence index_, which specifies which occurrence of an opcode to choose. This number must be
either positive or negative, but never zero. Positive numbers search from the start, and
negative numbers search from the end.

Given that, index `1` is the very first opcode of a given type, and index `-1` is the very last
opcode of a given type.

The `AllOccurrences` form selects all occurrences of a given opcode, and is useful for injecting
code before all `Return`s, for example.

`pick` should be replaced with `Start`, `End`, or `Span`. This specifies which part of the
instruction to select. Choosing `Start` will prepend bytecode before the selected span, `End` will
append bytecode after the selected span, and `Span` will replace the selected span with the
generated bytecode.

Examples of valid opcode queries:

- `Opcode.VirtualFunction{3, 4}.Span` - replace the 3rd and 4th virtual function call
- `Opcode.JumpIfNot{-1}.End` - insert after the last `if` statement
- `Opcode.Return(AllOccurrences).Start` - insert before all `return` statements

### Generators

Yarnbox facilitates inserting new bytecode into the existing function using _bytecode generators._
At the moment only one generator is available - `StaticFinalFunctionCall`, or SFFC for short:

```lua
StaticFinalFunctionCall(func)
StaticFinalFunctionCall(func, { options })
```

As the name suggests, it generates a call to a `static final` function. The basic usage is as
follows:

```lua
StaticFinalFunctionCall(Function'ExampleMod.ExMod_Injections.SomeFunction')
```

The function on the UnrealScript side must have no arguments, and when replacing an expression that
produces some result, it must have the same return type as that expression. Note that this is not
validated in any way, so take care.

```unrealscript
// The class we extend does not matter; usually we want it to be Object.
class ExMod_Injections extends Object;

// If we're replacing an expression that normally produces an int, our function also has to return
// an int.
function int SomeFunction()
{
    return 42;
}
```

The more advanced form of `StaticFinalFunctionCall` accepts a table of options. The currently
supported set of options (and their default values) is as follows:

```lua
StaticFinalFunctionCall(func, {
    captureSelf = false,
})
```

- `captureSelf` - captures `self` into the first argument of the function call.
  Instead of generating a plain call like `SomeFunction()`, it'll generate `SomeFunction(self)`.
