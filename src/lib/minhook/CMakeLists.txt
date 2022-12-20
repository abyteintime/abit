set(MINHOOK_INCLUDE "include")

set(MINHOOK_SOURCES
	"src/hde/hde32.c"
	"src/hde/hde64.c"
	"src/buffer.c"
	"src/hook.c"
	"src/trampoline.c"
)

add_compile_options(-dM)
add_library(MinHook STATIC ${MINHOOK_SOURCES})
target_include_directories(MinHook PUBLIC ${MINHOOK_INCLUDE})
