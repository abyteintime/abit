set(YARNBOX_SOURCES
	"bytecode/codegen.cpp"
	"bytecode/disassembler.cpp"
	"bytecode/dumper.cpp"
	"bytecode/encoding.cpp"
	"bytecode/index.cpp"
	"bytecode/jumps.cpp"
	"bytecode/opcode.cpp"
	"bytecode/tree.cpp"

	"vm/tracing.cpp"

	"config/config.cpp"
	"config/bootstrap.cpp"
	"config/opcode.cpp"

	"registry.cpp"
	"patcher.cpp"

	"main.cpp"
)

add_library(Yarnbox SHARED ${YARNBOX_SOURCES})
target_link_libraries(Yarnbox
	AByteInTime.Common
	AByteInTime.Loader.API
	AByteInTime.UnrealEngine
	sol2
)
set_target_properties(Yarnbox PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${ABIT_MODS_DIRECTORY}
)
