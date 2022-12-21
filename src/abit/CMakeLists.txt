set(ABIT_VERSION ${PROJECT_VERSION})
configure_file("version.hpp.in" "version.hpp")

set(ABIT_COMMON_SOURCES
	"error.cpp"
	"paths.cpp"
	"string.cpp"
)

add_library(AByteInTime.Common SHARED ${ABIT_COMMON_SOURCES})
target_include_directories(AByteInTime.Common PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)

add_subdirectory("launcher")
add_subdirectory("loader")
