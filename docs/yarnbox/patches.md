# Patches

The high-level design of Yarnbox, describing the sort of patching functionality it is meant to
support in the end.

## Configuration

To actually apply patches, we need to have UnrealScript bytecode ready. This process is handled
while cooking the mod using the official Hat in Time modding tools (via the Compile Scripts button.)

Yarnbox needs to know which functions to apply patches to, and that is done using the
`Yarnbox.Patches.json` file inside the mod's directory.

The overall file structure is as follows:

```json
{
  "version": 1,
  "patches": []
}
```

The version number will be incremented if any breaking changes are made. Yarnbox will do its best
to maintain backwards compatibility of this file but warnings may be emitted during mod loading if
a mod built for an older version of Yarnbox is loaded into the game. If certain features change in
a backwards-incompatible way, Yarnbox will not load the mod's patches and will emit an error.

The paragraphs below refer solely to the `patches` array; read the JSON examples as if they describe
only that array.

## Chunks

Before we get started with the dark magic of patching, we first need to understand what it is that
we're patching in the first place.

Unreal Engine uses a rather peculiar class hierarchy in its reflection system. The base class of
all things executable is `UStruct`, and there are two types of code chunks: `UFunction` and
`UState`. Technically speaking there's also `UClass`, but classes never contain bytecode on their
own.

A `UFunction` represents a function, declared like so in UnrealScript:

```unrealscript
function Example()
{
  // function code goes here
}
```

This class is also used for `event`s, since they're the same as functions. The difference is that
events can be called by C++ code.

A `UState` represents a state, declared as follows in UnrealScript:

```unrealscript
state Walking
{
  // state code goes here
}
```

You can refer to the [UnrealScript reference](https://docs.unrealengine.com/udk/Three/UnrealScriptReference.html)
for more details on how these work.

Each `UStruct` that is detected by Yarnbox is saved into a registry, and can be later recalled using
its **path name**. The path name is determined by walking the outer object chain. In reality, this
means that path names can be one of four things:

- `Package.Class`
- `Package.Class.Function`
- `Package.Class.Function.State`
- `Package.Class.Function.State.StateFunction`

For example, the path name for the player pawn class is `HatinTimeGameContent.Hat_Player`.
The path name of the player pawn class's Tick function is `HatinTimeGameContent.Hat_Player.Tick`.

Each piece of bytecode collected into the registry like this is called a **chunk**. Each chunk
contains a reference to its corresponding `UStruct` instance, as well as bytecode disassembly and
analysis data. This data is produced on-demand for injection patches.

## Replacements

Replacements are the simplest type of patch. They replace a function's bytecode completely
with the bytecode of a different, but compatible function. Replacements are specified using
`Replacement`-type entries inside `Yarnbox.Patches.json`, and look like so:

```json
[{
  "type": "Replacement",
  "comment": "Let the player move in the Peace and Tranquility menu",
  "chunk": "MyMod.MyMod_PnTReplacements.DisablesMovement",
}]
```

The specified `chunk` must be a function in a class that extends the base class the function is
being replaced in. For example, for `MyMod_PnTReplacements` to contain replacements for
`Hat_HUDMenu_HatKidDance`, we make it extend that class:

```unrealscript
class MyMod_PnTReplacements extends Hat_HUDMenu_HatKidDance;
```

The name of the replaced function should be the same inside the class containing the replacement.

```unrealscript
function bool DisablesMovement(HUD H)
{
  // 😈
  return false;
}
```

The signatures of both functions must be the same - their return type and parameters must match.

## Injections

Injections are a more complex type of patch; instead of replacing a function completely, they
insert bytecode into it.

Injections use `Injection`-type entries, like so:

```json
[{
  "type": "Injection",
  "comment": "Injects into some game code",
  "inject": [...]
}]
```

The `inject` array specifies a list of injections to apply.
Each injection is an object with the following fields:

```json
{
  "into": "Chunk.To.Inject.Into",
  "select": [...],
  "action": "Insert",
  "place": {...}
}
```

Injections are applied in sequence from top to bottom.

### Queries

Each element in the `select` array is a query. Each query must be an _opcode query_ object:

```json
{
  "opcode": "OpcodeToSearchFor",
  "which": [0, 1, 2],
  "pick": "Span"
}
```

Every query produces zero or more `start..end` spans that are then replaced with something else.
A span can be _zero-sized_, which means that its start is the same as its end, and it effectively
points to a single point in the bytecode. Thus injecting into a zero-sized span inserts bytecode
instead of replacing it.

The `"head"` query produces a zero-sized span which points to the beginning of the chunk, right
after the function arguments.

The opcode query searches for occurrences of the given opcode within the chunk.

- `opcode` defines which opcode should be searched for.
- `which` defines which occurrences should be targeted, and can be `"all"` to target all
  occurrences. The numbers in the array can be negative, which means that occurrences will be
  counted from the end.
- `pick` defines which part of the span should be worked on. `"Span"` selects the full span
  `start..end`, `"Start"` picks the zero-sized span `start..start`, and `"End"` picks the
  zero-sized span `end..end`.

If an injection produces zero queries, a warning is emitted to signal that the patch effectively
did not get applied to anything.

### Bytecode generation

Last but not least, there's the `place` field. This field specifies what kind of bytecode to
place at the selected spans.

#### `StaticFinalFunctionCall`

`StaticFinalFunctionCall` generates, as the name suggests, a call to a static final function.

```json
{
  "type": "StaticFinalFunctionCall",
  "function": "Your.Function.To.Call"
}
```

### Injections are transactions

The golden rule of all injections is that an `Injection`-type patch is like a transaction - it's a
single atomic unit that must be applied in its entirety. If one injection of a single
`Injection`-type patch fails to apply, then _no_ injections from the same patch will be applied.
This can (and should) be used to make sure that functionality does not stay broken if some injection
fails.

### Safety first

Injections are not checked for correctness besides a few basic checks at the moment. Extra
validation may get added in the future, but in the meantime it is largely the modder's job to ensure
the generated bytecode is type-safe.

While Yarnbox will refuse to inject bytecode that doesn't make sense at least _structurally_,
it does not check the _semantics_ of the injections it produces. For example, it will not prevent
you from creating an injection that replaces an expression producing a `bool` with a call to a
function that produces a `string`. These kinds of type-unsafe patches in fact trigger undefined
behavior within the game's C++ code, so be ready for crashes if you perform this kind of
tomfoolery.

### Example injection

For a complete example of how injections can be used to modify the game, please take a look at the
the PeacefulAndTranquilForever mod, which patches the Peace and Tranquility screen to play the
"No one is around to help" animation forever. It's a softlock, but at least it's a fun one!
