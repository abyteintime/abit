if(${ABIT_GENERATE_PROCS})
	message(STATUS "Using generated Procs sources")

	set(ABIT_PROC_ADDRESSES_FILE ${CMAKE_CURRENT_BINARY_DIR}/procs.cpp)
	set(ABIT_PROCS_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)

	add_custom_command(
		OUTPUT ${ABIT_PROC_ADDRESSES_FILE}
		COMMAND
			cargo run
			--release
			--manifest-path ${CMAKE_SOURCE_DIR}/abit-procs-generator/Cargo.toml
			--
			--pdb ${ABIT_HATINTIME_GAME_PDB}
			addresses
			--output-source ${ABIT_PROC_ADDRESSES_FILE}
			--output-headers ${ABIT_PROCS_INCLUDE_DIR}/abit/procs
		DEPENDS
			${ABIT_HATINTIME_GAME_PDB}
			${CMAKE_SOURCE_DIR}/abit-procs-generator
		VERBATIM
	)
else()
	message(STATUS "Using included Procs sources")

	set(ABIT_PROC_ADDRESSES_FILE ${CMAKE_SOURCE_DIR}/procs/src/procs.cpp)
	set(ABIT_PROCS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/procs/include)
endif()

if(${ABIT_GENERATE_MAPPINGS})
	message(STATUS "AByteInTime.Mappings.txt will be generated")

	set(ABIT_PROC_MAPPINGS_FILE ${CMAKE_BINARY_DIR}/AByteInTime.Mappings.txt)
	add_custom_command(
		OUTPUT ${ABIT_PROC_MAPPINGS_FILE}
		COMMAND
			cargo run
			--release
			--manifest-path ${CMAKE_SOURCE_DIR}/abit-procs-generator/Cargo.toml
			--
			--pdb ${ABIT_HATINTIME_GAME_PDB}
			mappings ${ABIT_PROC_MAPPINGS_FILE}
		DEPENDS
			${ABIT_HATINTIME_GAME_PDB}
		VERBATIM
	)
	add_custom_target(AByteInTime.Procs.Mappings DEPENDS ${ABIT_PROC_MAPPINGS_FILE})
else()
	add_custom_target(AByteInTime.Procs.Mappings)
endif()

set(ABIT_PROCS_SOURCES
	"init.cpp"
	${ABIT_PROC_ADDRESSES_FILE}
)

add_library(AByteInTime.Procs SHARED ${ABIT_PROCS_SOURCES})
add_dependencies(AByteInTime.Procs
	AByteInTime.Procs.Mappings
)
target_include_directories(AByteInTime.Procs PUBLIC ${ABIT_PROCS_INCLUDE_DIR})
set_target_properties(AByteInTime.Procs PROPERTIES
	# Procs does its own export/import annotations and CMake seems to be confused by the sheer
	# amount of symbols this DLL has.
	WINDOWS_EXPORT_ALL_SYMBOLS OFF
)
