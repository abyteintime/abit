# A Byte in Time

A Hat in Time tweaking toolkit.

## IMPORTANT NOTE

A Byte in Time (the toolkit) no longer works on current versions of the game. Gears for Breakfast
removed HatinTimeGame.pdb from the game's directory, which this loader was relying on for finding out
function locations.

If you have a backup of a version of the game made before February 2023, you might be able to compile
ABiT for it yourself. If you have a backup of the version released directly after the Christmas event
update, the release binary will work on it out of the box.

This renders the modding toolkit effectively useless for the vast majority of modders, but stay tuned!
Follow the [ABiT organization](https://github.com/abyteintime) for more cool modding tools in the future.

## Why

So you may be saying this to yourself at this point: "but uncle liquidex, the hat game already has
modding support!!!11" And I know that!

But the thing with A Hat in Time Modding As The Developers Intended is that it's very *limited.*
You're confined to the limits of what adding new classes in UnrealScript can provide, and cannot
modify any native code, nor tweak the existing UnrealScript in any way.

Which is where ABiT comes in.

## What

As mentioned already, ABiT is a tweaking toolkit for A Hat in Time. Its purpose is to expose the
internals of the game such that **You** can play around with them in ways the developers never even
considered possible. Want to make the in-game speedrun timer display 4:20 at all times?
[Got you covered](src/mods/example_fourtwenty/). Wanna have Peace and Tranquility loop forever?
[Take a look](src/gamemods/PeacefulAndTranquilForever/).

As a user (be it a player or a mod developer,) the most important components of ABiT are:

- **The Loader,** which facilitates injecting custom behavior into the base game's C++ code,
- **Yarnbox,** which facilitates injecting custom behavior into the game's UnrealScript code.
  It is implemented as a native mod on top of the Loader.

ABiT should be considered a tool for tweaks and bug fixes. If you use it to cheat in any competitive
setting I'll be very sad and disappointed.

## Where

You can download the latest release of ABiT [here](https://github.com/abyteintime/abit/releases).

### A word of warning

At this stage ABiT should be considered an alpha piece of software. Expect major breakthroughs and
breaking changes.

Being in alpha, it's ready for poking at, but expect crashes, rough edges, and lacks in
documentation along the way.

You have been warned.

## How

### Prerequisites

- Clang >= 15
  - Other compilers (such as MSVC) are not tested. Beware.
- CMake >= 3.24
- Ninja >= 1.11
- Rust >= 1.66
  - Can be disabled by passing `-DABIT_GENERATE_PROCS=OFF -DABIT_GENERATE_MAPPINGS=OFF` to CMake,
    but the build may not work for your version of the game and function name mappings will not be
    available.
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

ABiT and Yarnbox ship with a few example mods that demonstrate what they can do.

#### Native mods

Native example mods are bundled together with the loader, but disabled in the config. You can
reenable them by removing these `+Disable` lines from `ByteinTime.ini`:

```ini
[Mods]
; By default, all mods are enabled. Each mod can be disabled individually by using +Disable.
+Disable=Example.HelloABiT
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
PS> .\scripts\buildAndInstallGameMods.ps1 -Mod PeacefulAndTranquilForever
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
