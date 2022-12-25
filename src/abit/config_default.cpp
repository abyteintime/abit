#include "abit/config.hpp"

using namespace abit;

const std::string_view Config::defaultConfigIni = R"config(; A Byte in Time configuration file

[Game]
; If your game is installed in a different directory, change that here.
Executable=C:\\Program Files (x86)\\Steam\\steamapps\\common\\HatinTime\\Binaries\\Win64\\HatinTimeGame.exe
WorkingDirectory=C:\\Program Files (x86)\\Steam\\steamapps\\common\\HatinTime

[Debug]
; Set this to true if you'd like the ABiT loader to wait for a debugger on startu
; The loader will wait once it's injected into GuardedMainWrapper, but before any mods are loaded.
; This way you can set a breakpoint inside your mod's ABiT_ModInit function.
WaitForDebugger=false

[Mods]
; By default, all mods are enabled. Each mod can be disabled individually by using +Disable.
+Disable=ExampleMod
+Disable=Example.FourTwenty
)config";
