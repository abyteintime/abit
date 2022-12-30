set(YARNBOX_SOURCES
	"ue/FString.cpp"
	"ue/UClass.cpp"
	"ue/UFunction.cpp"
	"ue/UObject.cpp"
	"ue/UStruct.cpp"

	"bytecode/disassembler.cpp"
	"bytecode/opcode.cpp"
	"bytecode/tree.cpp"

	"main.cpp"
)

add_library(Yarnbox SHARED ${YARNBOX_SOURCES})
target_link_libraries(Yarnbox
	AByteInTime.Common
	AByteInTime.Loader.API
)
set_target_properties(Yarnbox PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${ABIT_MODS_DIRECTORY}
)
