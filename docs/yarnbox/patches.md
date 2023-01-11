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

## Replacements

Replacements are the simplest type of patch. They replace a function's bytecode completely
with the bytecode of a different, but compatible function. Replacements are specified using
`Replacement`-type entries inside `Yarnbox.Patches.json`, and look like so:
```json
[
  {
    "type": "Replacement",
    "comment": "Let the player move in the Peace and Tranquility menu",
    "class": "MyMod_PnTReplacements",
    "function": "DisablesMovement"
  }
]
```

The specified `class` must extend the class it wishes to replace functions in. For example, to
modify the Peace and Tranquility menu, we make the class extend `Hat_HUDMenu_HatKidDance`:
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
[
  {
    "type": "Injection",
    "comment": "Makes a ligma joke with your hats",

    "injections": [{
      "in": "HatinTimeGame.Hat_HUDMenu.RenderBorderedText",
      "select": [{
        "opcode": "VirtualFunction",
        "occurrences": "all",
        "filter": [{ "name": "RenderBorderedText" }]
      }],
      "operation": {
        "type": "Replace",
        "with": "FinalFunction",
        "": ""
      }
    }]
  }
]
```

The injection above will replace all occurrences of the function call to
`Hat_HUDMenu.RenderBorderedText` with our evil `MyMod_Ligma.RenderBorderedText`, which subjects the
player to infinite ligma jokes:
```unrealscript
class MyMod_Ligma extends Object;

static function RenderBorderedText(
  HUD H,
  Hat_HUDElement el,
  string msg,
  float posx,
  float posy,
  float textscale,
  optional TextAlign Alignment,
  optional float border_size = 2,
  optional Color BorderColor,
  optional float verticalscale = -1,
  optional float ShadowAlpha = 1,
  optional float BorderQuality = 1
)
{
  class'Hat_HUDMenu'.static.RenderBorderedText(
    H,
    el,
    "Ligma Hat",
    posx,
    posy,
    textscale,
    Alignment,
    border_size,
    BorderColor,
    verticalscale,
    ShadowAlpha,
    BorderQuality
  );
}
```

The exact details of what injections will be available are to be determined. This is just a sample
of what Yarnbox plans to allow in terms of injection capabilities. For example, more targets will
be available, such as injecting at the beginning or before all `return`s in a function, etc.

Injections are inherently dependent on Yarnbox's disassembler and will not be available for
functions that the disassembler cannot disassemble completely. This is because inserting bytecode
in the middle of a function may screw up jump offsets, thus we need to know the locations of _all_
jumps without exceptions to apply injections properly, but with how UnrealScript is implemented
with its nested variable-length instructions this is impossible to do without disassembling
everything accurately.

What I'm saying is: Expect bugs.
