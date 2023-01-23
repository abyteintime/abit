# Getting started with ABiT

Glad to see someone interested in the project!

To get started, head over to the [Releases](https://github.com/abyteintime/abit/releases) page and
download the latest version (`AByteInTime-win64.zip`.) Then unpack the archive somewhere convenient,
and run `AByteInTime.exe`.

If everything works correctly, you should see a console window pop up saying

```text
A Byte in Time Loader version 0.1.0
Config file: C:\Users\liquidex\Repositories\abit\build\bin\Debug\ByteinTime.ini
```

with some timing/log verbosity info before each line, and the game should boot up as usual.

If that doesn't happen, please look at [Troubleshooting](#troubleshooting).

## Next steps

With how early ABiT is in its development, I assume you came here to mod the game with it; if so,
please look at the [patching tutorial](./patching-tutorial.md).

## Troubleshooting

### A Hat in Time doesn't launch

If your Hat installation is in any way custom, you'll need to edit the loader's configuration
file and tell it where the game is located. This file is generated upon first launch next to
`AByteInTime.exe`, and is called `ByteinTime.ini`.

In this file at the very top you'll find these lines:

```ini
[Game]
; If your game is installed in a different directory, change that here.
; Note that the working directory must be the game's install directory, while Executable must point
; to where the (64-bit) .exe of the game is located.
Executable=C:\Program Files (x86)\Steam\steamapps\common\HatinTime\Binaries\Win64\HatinTimeGame.exe
WorkingDirectory=C:\Program Files (x86)\Steam\steamapps\common\HatinTime

; -- snip --
```

You'll need to change `WorkingDirectory` to the path where you installed A Hat in Time, and change
`Executable` to point to `Binaries\Win64\HatinTimeGame.exe` within that directory.
