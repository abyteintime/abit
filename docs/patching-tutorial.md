# Patching tutorial

This tutorial will guide you through creating your first bytecode injection with ABiT/Yarnbox.
Yarnbox is the ABiT mod that facilitates injecting into UnrealScript bytecode.

## Setup

Before we start, we need to know which function to patch into. I'm sure you already came here with
a piece of code you want to change in mind, but this tutorial is going to patch into the Peace and
Tranquility screen to change the tempo at which Hat Kid dances.

The class that handles the Peace and Tranquility screen is called `Hat_HUDMenu_HatKidDance`, and
is located at `Development\Src\HatinTimeGameContent\Classes\HUD\HUDMenus\Hat_HUDMenu_HatKidDance.uc`.
The tempo is set inside the function `OnOpenHUD`:

```unrealscript
simulated function OnOpenHUD(HUD H, optional string command)
{
    // -- snip --
    DynamicMusicNode.BPM = 128;
    // -- snip --
}
```

Having just the class name and function name is not enough though. Let me explain why.

While the game's loading, Yarnbox takes note of each function loaded and saves it in a registry,
assigning it a _path name_ in the process. For most functions, including ours, the path name takes
the form `Package.Class.Function`. The `Package` can be found by looking at the function's source
file path - `Development\Src\HatinTimeGameContent\...` - the folder within `Src` is the content
package the class belongs to, and that's what we want to use as our `Package`. The `Class.Function`
part is pretty obvious; thus, our function's path name is `HatinTimeGameContent.Hat_HUDMenu_HatKidDance.OnOpenHUD`.

### Where do path names come from?

Yarnbox generates path names based on the outer object chain. Each object in Unreal Engine stores
a reference to its _outer object_ (which can be null.) As far as I'm aware this is used for
the purposes of serialization.

Anyhow, the `Package.Class.Function` structure comes from the fact that `Function`'s outer object
is `Class`, and `Class`'s outer object is `Package` - so we go from the outermost to the innermost
object, giving us `Package.Class.Function`.

## Finding the bytecode

UnrealScript is compiled into bytecode, which is a much more compact representation than the source
text. To facilitate its functionality, Yarnbox disassembles that bytecode into a more structured
representation called the _bytecode tree._ Bytecode trees are still much lower level than actual
UnrealScript source code, but by looking at the tree it's usually easy to correlate the tree
with the source code, since Unreal leaves a lot of metadata behind that Yarnbox can then present to
you. But how do you see the bytecode tree of a function in the first place?

To see the bytecode trees of functions, we need to enable more verbose debug logs. To do that,
edit the `ByteinTime.ini` file located next to `AByteInTime.exe`, and locate the `[Log]` section.

```ini
[Log]
; A Byte in Time includes a logger for more easily diagnosing what the loader and mods are doing.
; The logger outputs to two locations: a console window, which is opened in the game's process,
; and a log file, located in the WorkingDirectory, named ByteinTime.log.

; The log messages that are output to a destination are filtered through a level, which specifies
; how verbose the messages should be.
; Available log levels are (from most verbose to least verbose):
;     trace
;     debug
;     info
;     warn
;     err
;     critical
;     off
;
; Only messages whose verbosity level is less than or equal to the chosen level will be output.

; Set the maximum level of log messages that should be generated at all.
; This can save some performance in case you're not debugging native mods.
Level=debug

; Set the maximum level of log messages output to ByteinTime.log inside the WorkingDirectory.
FileLevel=debug
; Set the maximum level of log messages output to the console window.
; See above for available log levels.
ConsoleLevel=info
```

Here, we want to change `Level` and `FileLevel` to `trace`, which is the highest possible verbosity.
Yarnbox dumps disassemblies of all functions it sees into trace-level logs.

If you're tempted to set the `ConsoleLevel` to `trace` too, beware: the amount of output Yarnbox
produces is _huge_ - around 80MB of text, and the Windows console is very slow at outputting large
amounts of text. Additionally, the console can only scroll back about 9000 lines by default, which
makes it impractical for inspecting bytecode.

With the config setting changed, we can now launch ABiT, let the game load into the title screen,
and close it afterwards.

The ABiT log files can be found inside the game's install directory, under `ByteinTime\Logs`.
This directory stores the log files for the last 11 runs of the game, with the latest one being
`Log.txt`. If `Log.txt` is very big (over 70MB,) then you did everything correctly! If not, please
go through the steps again and check that you haven't missed anything.

Now open the `Log.txt` file in Notepad (or your favorite editor of choice.)
If you're using Notepad remember to disable word wrapping before opening the file, since it slows
down loading _a ton._

At the very top you should see a ton of lines with `[trace]` verbosity, like so:

```log
[2023-01-21 21:39:58.454] [trace] Chunk: Function'CheckIsGameThread'
[2023-01-21 21:39:58.454] [trace]   - outer: 1:Class'Object' (0x7ff42c9e3600)
[2023-01-21 21:39:58.454] [trace]   - bytecode: (2 bytes) [11, 83]
[2023-01-21 21:39:58.454] [trace] Disassembled bytecode into 2 nodes
[2023-01-21 21:39:58.454] [trace] Disassembly:
000..001 @ Nothing (11)
001..002 @ EndOfScript (83)

[2023-01-21 21:39:58.454] [trace] Outer chunk 'Core.Object' needs to be registered before 'Core.Object.CheckIsGameThread'
[2023-01-21 21:39:58.454] [trace] Registered chunk Class'Object' as 'Core.Object'
[2023-01-21 21:39:58.454] [trace] Registered chunk Function'CheckIsGameThread' as 'Core.Object.CheckIsGameThread'
```

This is the set of logs that Yarnbox dumps for every single piece of bytecode (_chunk_) it
disassembles. Let's break them down piece by piece.

```log
[2023-01-21 21:39:58.454] [trace] Chunk: Function'CheckIsGameThread'
[2023-01-21 21:39:58.454] [trace]   - outer: 1:Class'Object' (0x7ff42c9e3600)
```

This part is output for every single `UStruct`-based object, which Yarnbox calls _chunks._
Chunks include not only functions, but also states, structs, and classes, even though the latter two
do not contain any bytecode. For chunks that do contain bytecode though, it also outputs the
following:

```log
[2023-01-21 21:39:58.454] [trace]   - bytecode: (2 bytes) [11, 83]
[2023-01-21 21:39:58.454] [trace] Disassembled bytecode into 2 nodes
```

The first line contains the raw bytecode, as in _the array of bytes_ that makes up the bytecode.
The second line is the status code telling you that the bytecode was disassembled into a tree
successfully.

The message below that is the most interesting part:

```log
[2023-01-21 21:39:58.454] [trace] Disassembly:
000..001 @ Nothing (11)
001..002 @ EndOfScript (83)
```

This part shows you the bytecode tree Yarnbox disassembled from the bytecode. Every line taking the
form `sss..eee @ Opcode (index)` is a single _node_ in the tree. The `sss..eee` part shows you which
space in the bytecode is occupied by the node, in hexadecimal. This is a half-inclusive range, so
`000..001` means the 0st byte until the 1nd byte, where the latter is not included in the range.

The `Opcode` tells the UnrealScript virtual machine (VM) what sort of _instruction_ to perform.
The number in parentheses is the opcode's numeric index.

In this chunk, we have the instructions `Nothing` followed by `EndOfScript`. `Nothing` literally
does nothing (returns `None`) and `EndOfScript` tells the VM that the chunk ends at that byte.
Note that **this opcode must be present.** Otherwise the VM will not know where the bytecode ends,
and will end up reading into garbage, most likely crashing the game. Keep this in mind while
designing your patches.

Let's look at a more complicated function:

```unrealscript
function bool Example()
{
    return true;
}
```

And here's its disassembly:

```text
000..002 @ Return (4)
  001..002 @ True (39)
002..00c @ Return (4)
  003..00c @ ReturnNothing (58)
    004..00c i(obj) 146791:BoolProperty'ReturnValue' (0x7ff40f25a140)
00c..00d @ EndOfScript (83)
```

This function demonstrates a few important things.

- Instructions can nest. For example the first `Return` here nests a `True` instruction within.
- Instructions can nest other data than just instructions, as indicated by span `004..00c`, which
  holds a reference to a `UObject`.
  - This "other data" can range from simple integers, to names, objects, and offsets to other parts
    of the bytecode. What kind of data is stored varies depending on the instruction.
  - You may also note that this "other data" is marked with `i(obj)` instead of `@`; here's the
    full list of prefixes that Yarnbox can emit:
    - `i` - an unsigned integer
    - `i(s8)`, `i(s16)`, `i(s32)`, `i(s64)` - signed integers of various sizes. In practice only
      `i(s32)` is used.
    - `i(f32)` - a `float`
    - `i(ptr)` - a pointer to some unknown data; in practice this is unused.
    - `i(obj)` - an object reference
    - `i(name)` - a name
    - `i(oabs)` - an offset to some absolute location within the bytecode - that is, relative to the
      bytecode's start
    - `i(orel+n)` - an offset to some relative location within the bytecode - that is, relative to
      where the `i(orel+n)` is, plus `n` bytes
    - `s` - a string. The quoted part may be prefixed with `L` if the string has Unicode characters.
      The entire game only contains a single Unicode string (and printing Unicode strings is
      currently broken so it appears as garbage.)
    - `@` - an instruction
    - `@+` - a list of instructions. This is used by function calls.
- If you're curious as to what the `ReturnNothing` opcode does - it's emitted at the end of each
  function that has a return type, and whenever this opcode is evaluated Unreal will emit a
  warning that a `return` statement was never entered.

Phew, that was a lot! Anyways, going back to the logs for a brief moment.

```log
[2023-01-21 21:39:58.454] [trace] Outer chunk 'Core.Object' needs to be registered before 'Core.Object.CheckIsGameThread'
[2023-01-21 21:39:58.454] [trace] Registered chunk Class'Object' as 'Core.Object'
[2023-01-21 21:39:58.454] [trace] Registered chunk Function'CheckIsGameThread' as 'Core.Object.CheckIsGameThread'
```

These three logs are emitted by the Yarnbox registry whenever a new chunk is registered. They tell
you which path name the chunk was registered under, which is useful if you ever run into problems
where the path name maybe isn't exactly what you expect it to be, or as will be our case now,
when you need to find a function's bytecode by its path name.

## Preparing the injection

So! With all that bytecode 101 out of the way, we are now ready to prepare an injection. First we
need to find the bytecode of the function we'll be patching into; so, let's press Ctrl+F and
find `HatinTimeGameContent.Hat_HUDMenu_HatKidDance.OnOpenHUD` in the file.

```log
[2023-01-21 21:40:13.844] [trace] Chunk: Function'OnOpenHUD'
[2023-01-21 21:40:13.844] [trace]   - outer: 79242:Class'Hat_HUDMenu_HatKidDance' (0x7ff428b4f600)
[2023-01-21 21:40:13.845] [trace]   - bytecode: (3603 bytes) [-- snip --]
[2023-01-21 21:40:13.850] [trace] Disassembled bytecode into 542 nodes
[2023-01-21 21:40:13.850] [trace] Disassembly:
-- snip --
[2023-01-21 21:40:13.850] [trace] Outer chunk 'hatintimegamecontent.Hat_HUDMenu_HatKidDance' needs to be registered before 'hatintimegamecontent.Hat_HUDMenu_HatKidDance.OnOpenHUD'
[2023-01-21 21:40:13.850] [trace] Registered chunk Class'Hat_HUDMenu_HatKidDance' as 'hatintimegamecontent.Hat_HUDMenu_HatKidDance'
[2023-01-21 21:40:13.850] [trace] Registered chunk Function'OnOpenHUD' as 'hatintimegamecontent.Hat_HUDMenu_HatKidDance.OnOpenHUD'
```

Do note that the disassembly is located _above_ our search result.

This function is _massive_, counting in at a whopping 542 instruction nodes disassembled into the
bytecode tree. Let's search for that `BPM` property from the start of the tree.

```text
2b3..2d7 @ Let (15)
  2b4..2d2 @ Context (25)
    2b5..2be @ InstanceVariable (1)
      2b6..2be i(obj) 79995:ObjectProperty'DynamicMusicNode' (0x7ff428b35540)
    2be..2c0 i(orel+12) +0x9 --> 2d3
    2c0..2c8 i(obj) 49804:FloatProperty'BPM' (0x7ff3656c2740)
    2c8..2c9 i 0
    2c9..2d2 @ InstanceVariable (1)
      2ca..2d2 i(obj) 49804:FloatProperty'BPM' (0x7ff3656c2740)  👈 there it is!
  2d2..2d7 @ FloatConst (30)
    2d3..2d7 i(f32) 128
```

This is the bytecode for setting a variable. It's a `Let` instruction, which contains two other
instructions. The first one is the left-hand side of the `=`, and the second one is the right-hand
side of the `=`. In our case we're interested in replacing the right-hand side - `FloatConst`, since
we want to change the tempo from 128 to something else.

Now we're in for a counting game. We need to find which occurrence of `FloatConst` we want to patch
into. Counting from the top of the function, this is the 4th occurrence of `FloatConst` in the
function.

Since we'll be replacing a `FloatConst`, let's prepare a function to replace it with.
Assume we already have a mod `BPMMod` and we're only adding a class `BPM_Injections` to it:

```unrealscript
class BPM_Injections extends Object;

static final function float GetSmugDanceBPM()
{
    // I've been really into The Flashbulb lately.
    // https://www.youtube.com/watch?v=Yx7_Z1WKRJc
    return 189;
}
```

Having a target function, an instruction we want to modify, and a function we'd like to inject,
we're ready to write the _patchfile._

Create a file called `Yarnbox.Patches.lua` next to your mod's `modinfo.ini`. Having this file tells
Yarnbox that your mod would like to patch into the game's bytecode.

```lua
injection "Change the BPM at which Hat Kid smugly dances" {
    -- Choose the function we inject into. Note that the path name is case-insensitive.
    into = Function'HatinTimeGameContent.Hat_HUDMenu_HatKidDance.OnOpenHUD',
    -- Select the entire span of bytecode the 4th occurrence of FloatConst occupies.
    select = Opcode.FloatConst{4}.Span,
    -- Replace it with a call to our function.
    generate = StaticFinalFunctionCall(Function'BPMMod.BPM_Injections.GetSmugDanceBPM'),
}
```

Now it's time to compile your mod, cook it, and launch ABiT. If you open the Peace and Tranquility
screen now, you'll notice Hat Kid dancing much faster than usual! In fact, her dance should sync up
with The Flashbulb's _Lucid Bass I._ Which is a pretty good drum'n'bass track. Can recommend.

## Next steps

With all that, you're equipped to conquer Hat with bytecode patches. The next places to visit would
be the [detailed patchfile documentation](yarnbox/patches.md), as well as the
[table of opcodes](yarnbox/opcodes.md), both of which will be invaluable along your patching
journey.

Have fun!
