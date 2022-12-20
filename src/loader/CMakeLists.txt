set(ABIT_LOADER_SOURCES
	"console.cpp"
	"detours.cpp"

	"main.cpp"
)

add_library(AByteInTime.Loader SHARED ${ABIT_LOADER_SOURCES})
target_link_libraries(AByteInTime.Loader
	inih
	MinHook
	AByteInTime.Common
)
