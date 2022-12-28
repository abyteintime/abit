set(ABIT_LOADER_API_SOURCES
	"logging_init.cpp"
	"mod.cpp"
	"patches.cpp"
)

add_library(AByteInTime.Loader.API SHARED ${ABIT_LOADER_API_SOURCES})
target_link_libraries(AByteInTime.Loader.API
	AByteInTime.Common
	AByteInTime.Procs
	MinHook
	spdlog::spdlog
)
target_include_directories(AByteInTime.Loader.API PUBLIC .)

set(ABIT_LOADER_SOURCES
	"main.cpp"
)

add_library(AByteInTime.Loader SHARED ${ABIT_LOADER_SOURCES})
target_link_libraries(AByteInTime.Loader
	AByteInTime.Loader.API
)
