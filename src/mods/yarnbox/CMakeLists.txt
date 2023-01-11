set(YARNBOX_SOURCES
	"bytecode/disassembler.cpp"
	"bytecode/dumper.cpp"
	"bytecode/encoding.cpp"
	"bytecode/jumps.cpp"
	"bytecode/opcode.cpp"
	"bytecode/tree.cpp"

	"vm/tracing.cpp"

	"config.cpp"
	"registry.cpp"
	"patcher.cpp"

	"main.cpp"
)

add_library(Yarnbox SHARED ${YARNBOX_SOURCES})
target_link_libraries(Yarnbox
	AByteInTime.Common
	AByteInTime.Loader.API
	AByteInTime.UnrealEngine
	nlohmann_json::nlohmann_json
)
set_target_properties(Yarnbox PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${ABIT_MODS_DIRECTORY}
)
