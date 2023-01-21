set(ABIT_VERSION ${PROJECT_VERSION})
configure_file("version.hpp.in" "version.hpp")

set(ABIT_COMMON_SOURCES
	"config.cpp"
	"config_default.cpp"
	"error.cpp"
	"io.cpp"
	"map.cpp"
	"paths.cpp"
	"string.cpp"
)

add_library(AByteInTime.Common SHARED ${ABIT_COMMON_SOURCES})
target_include_directories(AByteInTime.Common PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)
target_link_libraries(AByteInTime.Common
	inih
	fmt::fmt
)

add_subdirectory("launcher")
add_subdirectory("loader")
add_subdirectory("procs")
add_subdirectory("ue")
