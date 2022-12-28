#include "abit/config.hpp"

using namespace abit;

const std::string_view Config::defaultConfigIni = R"config(; A Byte in Time configuration file

[Game]
; If your game is installed in a different directory, change that here.
; Note that the working directory must be the game's install directory, while Executable must point
; to where the (64-bit) .exe of the game is located.
; ABiT does not support 32-bit systems.
Executable=C:\Program Files (x86)\Steam\steamapps\common\HatinTime\Binaries\Win64\HatinTimeGame.exe
WorkingDirectory=C:\Program Files (x86)\Steam\steamapps\common\HatinTime

[Mods]
; By default, all mods are enabled. Each mod can be disabled individually by using +Disable.
+Disable=ExampleMod
+Disable=Example.FourTwenty

[Log]
; A Byte in Time includes a logger for more easily diagnosing what the loader and mods are doing.
; The logger outputs to two locations: a console window, which is opened in the game's process,
; and a log file, located in the WorkingDirectory, named ByteinTime.log.

; Set the maximum level of log messages output to ByteinTime.log inside the WorkingDirectory.
; Available log levels are (from most verbose to least verbose):
;     trace
;     debug
;     info
;     warn
;     err
;     critical
;     off
FileLevel=debug
; Set the maximum level of log messages output to the console window.
; See above for available log levels.
ConsoleLevel=info

[Debug]
; Set this to true if you'd like the ABiT loader to wait for a debugger on startup.
; The loader will wait once it's injected into GuardedMainWrapper, but before any mods are loaded.
; This way you can set a breakpoint inside your mod's ABiT_ModInit function.
WaitForDebugger=false
)config";
