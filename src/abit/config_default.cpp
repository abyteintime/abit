#include "abit/config.hpp"

using namespace abit;

// clang-format off

const std::string_view Config::defaultConfigIni =
"[Game]\n"
"; If your game is installed in a different directory, change that here.\n"
"Executable=C:\\Program Files (x86)\\Steam\\steamapps\\common\\HatinTime\\Binaries\\Win64\\HatinTimeGame.exe\n"
"WorkingDirectory=C:\\Program Files (x86)\\Steam\\steamapps\\common\\HatinTime\n"
"\n"
"[Mods]\n"
"; By default, all mods are enabled. Each mod can be disabled individually by using +Disable.\n"
"+Disable=ExampleMod\n"
"+Disable=Example.FourTwenty\n"
;

// clang-format on
