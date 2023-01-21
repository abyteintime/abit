set(EXAMPLE_MOD_SOURCES
	"main.cpp"
)

add_library(Example.HelloABiT SHARED ${EXAMPLE_MOD_SOURCES})
target_link_libraries(Example.HelloABiT
	AByteInTime.Common
	AByteInTime.Loader.API
)
set_target_properties(Example.HelloABiT PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${ABIT_MODS_DIRECTORY}
)
