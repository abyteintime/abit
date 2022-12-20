set(ABIT_VERSION ${PROJECT_VERSION})
configure_file("version.cpp.in" "version.cpp")

set(ABIT_COMMON_SOURCES
	"error.cpp"
	"string.cpp"
	"version.cpp"
)

add_library(AByteInTime.Common SHARED ${ABIT_COMMON_SOURCES})
