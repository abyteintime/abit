set(EXAMPLE_FOURTWENTY_SOURCES
	"main.cpp"
)

add_library(Example.FourTwenty SHARED ${EXAMPLE_FOURTWENTY_SOURCES})
target_link_libraries(Example.FourTwenty
	AByteInTime.Common
	AByteInTime.Loader
)
set_target_properties(Example.FourTwenty PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${ABIT_MODS_DIRECTORY}
)
