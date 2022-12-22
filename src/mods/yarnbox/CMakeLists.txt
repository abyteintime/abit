set(YARNBOX_SOURCES
	"main.cpp"
)

add_library(Yarnbox SHARED ${YARNBOX_SOURCES})
target_link_libraries(Yarnbox
	AByteInTime.Common
	AByteInTime.Loader
)
set_target_properties(Yarnbox PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${ABIT_MODS_DIRECTORY}
)
