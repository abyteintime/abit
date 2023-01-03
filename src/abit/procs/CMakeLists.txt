add_custom_command(
	OUTPUT procs.cpp
	COMMAND
		cargo run
		--release
		--manifest-path ${CMAKE_SOURCE_DIR}/abit-procs-generator/Cargo.toml
		--
		--pdb ${ABIT_HATINTIME_GAME_PDB}
		--output-source procs.cpp
		--output-headers include/abit/procs
	DEPENDS
		${ABIT_HATINTIME_GAME_PDB}
		${CMAKE_SOURCE_DIR}/abit-procs-generator
	VERBATIM
)

set(ABIT_PROCS_SOURCES
	"init.cpp"
	"procs.cpp"
)

add_library(AByteInTime.Procs SHARED ${ABIT_PROCS_SOURCES})
target_include_directories(AByteInTime.Procs PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
