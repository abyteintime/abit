set(LUA_SOURCES
	"lapi.c"
	"lauxlib.c"
	"lbaselib.c"
	"lcode.c"
	"lcorolib.c"
	"lctype.c"
	"ldblib.c"
	"ldebug.c"
	"ldo.c"
	"ldump.c"
	"lfunc.c"
	"lgc.c"
	"linit.c"
	"liolib.c"
	"llex.c"
	"lmathlib.c"
	"lmem.c"
	"loadlib.c"
	"lobject.c"
	"lopcodes.c"
	"loslib.c"
	"lparser.c"
	"lstate.c"
	"lstring.c"
	"lstrlib.c"
	"ltable.c"
	"ltablib.c"
	"ltm.c"
	"lundump.c"
	"lutf8lib.c"
	"lvm.c"
	"lzio.c"
)

add_library(lua5.4 SHARED ${LUA_SOURCES})
target_compile_definitions(lua5.4 PRIVATE
	LUA_BUILD_AS_DLL
)
target_include_directories(lua5.4 PUBLIC ".")
