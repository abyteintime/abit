set(ABIT_LOADER_SOURCES
	"console.cpp"
	"mod.cpp"
	"patches.cpp"
	"procs.cpp"

	"main.cpp"
)

add_library(AByteInTime.Loader SHARED ${ABIT_LOADER_SOURCES})
target_link_libraries(AByteInTime.Loader
	AByteInTime.Common
	inih
	MinHook
)
target_include_directories(AByteInTime.Loader PUBLIC .)
