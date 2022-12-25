set(EXAMPLE_MOD_SOURCES
	"main.cpp"
)

add_library(ExampleMod SHARED ${EXAMPLE_MOD_SOURCES})
target_link_libraries(ExampleMod
	AByteInTime.Common
	AByteInTime.Loader.API
)
set_target_properties(ExampleMod PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${ABIT_MODS_DIRECTORY}
)
