# A Byte in Time

A Hat in Time hacking toolkit.

## A word of warning

At this stage ABiT is still only a proof of concept. Expect major breakthroughs and major
breaking changes.

You have been warned.

## Why

So you may be saying this to yourself at this point: "but uncle liquidex, the hat game already has
modding support!!!11" And I know that!

But the thing with A Hat in Time Modding As The Developers Intended is that it's very *limited.*
You're confined to the limits of what adding new classes in UnrealScript can provide, and although
the game is mostly written in UnrealScript anyways, doing more advanced things that intend to modify
the base game is quite a pain. Which is why you only ever see mods that *add* content on the
Workshop.

Because, say you want to modify an existing piece of GUI? Ha ha ha, ***good luck!***

Which is where ABiT comes in.

## What

As mentioned already, ABiT is a hacking toolkit for A Hat in Time. Its purpose is to expose the
internals of the game such that **You** can play around with them in ways the developers never even
thought of. Want to make the in-game speedrun timer display 4:20 at all times?
[Got you covered](src/mods/example_fourtwenty/).

ABiT is split into a few modules, each one fulfilling a single purpose:
- **The Launcher,** which handles starting the game up, and injecting into it a tiny lil' DLL called
- **The Loader.** This piece of code is responsible for handling the basic logic of patching into
  the game's code, and exposing that functionality to mod DLLs, which it loads before the game is
  even ready to *think* about initializing anything.
- **The Procs.** All of that is nice and dandy, but we actually need to know which functions to
  patch into - as in, *where the heck they actually are in memory.* Unfortunately this task is made
  quite difficult by the fact that HatinTimeGame.exe is... well, an .exe, and as such does not
  export any symbols, so good ol' `GetProcAddress` is not going to cut it.
  So, it's time to bring out The Big Guns.™ We use the innocuous little .pdb file that
  ships next to the game's .exe, and bada bing, bada boom, *your symbols are now in my room.*
  Using a Rust program, we read the .pdb to figure out where all the public symbols are; we demangle
  them, and save their addresses into a .dll.
  - Using a .dll allows us to have at least *some* level of cross-version compatibility.
    The launcher, loader, and mods can potentially stay at the same version for many updates to
    A Hat in Time, and you'll only need to replace `AByteInTime.Procs.dll` with each update.
    Of course ABI compatibility is another thing, because the .dll does not contain any information
    about function signatures, so if any of them changes, your mod crashes and burns (and the game
    together with it.)
- **The Yarnbox,** which is still in its infancy.
  The Yarnbox is going to be a native mod which enables UnrealScript bytecode patching
  functionality, very much akin to what's done in [Minecraft modding][mcmods] with [Mixin].
  - When this will come out, I have no idea; it'll probably take me a while until I have a PoC
    for this because I need to reverse engineer how the UnrealScript VM actually works under the
    hood.

  [mcmods]: https://fabricmc.net/
  [Mixin]: https://github.com/SpongePowered/Mixin

## How

### Prerequisites

- Clang >= 15
  - Other compilers (such as MSVC) are not tested. Beware.
- CMake >= 3.24
- Ninja >= 1.11
- A Hat in Time
  - Also the Modding Tools, if you wish to build Yarnbox mods.

Older versions may work but haven't been tested.

### Building the toolkit

With all the prerequisites gathered, it's time to build:
```powershell
# Before building:
PS> .\scripts\setup.ps1
# To (re-)build:
PS> .\scripts\build.ps1
# To build and run the launcher:
PS> .\scripts\buildAndRun.ps1
```

### Game install paths

ABiT relies on having access to the game installation, both at build time and at runtime.
By default, the default Steam installation directory is used (`C:\Program Files (x86)\Steam\steamapps\common\HatinTime`.)

#### At build time

CMake will complain about not being able to find a valid Hat in Time installation if you installed
the game elsewhere or from Humble.

It's possible to change the directory by passing the argument `-DABIT_HATINTIME_PATH=...` when
running `.\scripts\setup.ps1`:
```powershell
PS> .\scripts\setup.ps1 -DABIT_HATINTIME_PATH="C:\Path\To\HatinTime"
```
The path you pass to CMake should be the directory containing the folders `Binaries`,
`HatinTimeGame`, and `Engine`, such that `Binaries/HatinTimeGame.pdb` can be reached from it.

#### At runtime

During runtime, the ABiT launcher needs to know where to find the game executable to launch.
This can be configured inside `ByteinTime.ini`, which is located next to the `AByteInTime.exe`
executable, and generated upon first launch:
```ini
[Game]
; If your game is installed in a different directory, change that here.
; Note that the working directory must be the game's install directory, while Executable must point
; to where the (64-bit) .exe of the game is located.
Executable=C:\Program Files (x86)\Steam\steamapps\common\HatinTime\Binaries\Win64\HatinTimeGame.exe
WorkingDirectory=C:\Program Files (x86)\Steam\steamapps\common\HatinTime

; -- snip --
```

### Building example mods

ABiT and Yarnbox ships with a few example mods that demonstrate what they can do.

#### Native mods

Native (ABiT) mods are built together with the loader, but disabled in the config. You can reenable
them by removing these `+Disable` lines from `ByteinTime.ini`:
```ini
[Mods]
; By default, all mods are enabled. Each mod can be disabled individually by using +Disable.
+Disable=ExampleMod
+Disable=Example.FourTwenty
```
Not that these mods do anything particularly interesting.

#### Yarnbox mods

Compiling the included Yarnbox example mods involves a little extra work. You will need the
Hat in Time modding tools installed for this.

To kickstart the compilation of all mods, use the following PowerShell script:
```powershell
PS> .\scripts\buildAndInstallGameMods.ps1
```

If you're working on one of the mods and would like to recompile only a specific one, use the `-Mod`
argument:
```powershell
PS> .\scripts\buildAndInstallGameMods.ps1 -Mod YarnboxTesting
```

If your game is installed in a different directory than the Steam default, you can use
`-GameInstallDirectory` to change it:
```powershell
PS> .\scripts\buildAndInstallGameMods.ps1 -GameInstallDirectory "C:\Path\To\HatinTime"
```

Note that the script is not required to create mods that use Yarnbox. It's only included in this
repository for the convenience of not having to copy the mod's source folder to
`HatinTime\HatinTimeGame\Mods` every time something is changed, and not having to click through
annoying console windows every time you wanna rebuild it.
