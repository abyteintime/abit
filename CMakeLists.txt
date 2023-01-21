cmake_minimum_required(VERSION 3.24)

# Project settings

project(abit
	VERSION 0.1.0
	DESCRIPTION "A Hat in Time tweaking toolkit"
	LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Options

set(ABIT_HATINTIME_PATH "C:/Program Files (x86)/Steam/steamapps/common/HatinTime" CACHE PATH
	"A Hat in Time installation directory. Required to point to a valid AHiT installation for ABIT_GENERATE_PROCS and ABIT_GENERATE_MAPPINGS to work.")

set(ABIT_GENERATE_PROCS ON CACHE BOOL
	"When enabled, ABiT will generate the sources for Procs instead of using the ones committed to the repository. Requires a valid installation directory to work.")

set(ABIT_GENERATE_MAPPINGS ON CACHE BOOL
	"When enabled, ABiT will generate a AByteInTime.Mappings.txt file inside the build directory, containing mappings from hashed proc names to demangled proc names from HatinTimeGame.pdb. Requires a valid installation directory to work.")

set(ABIT_BUILD_EXAMPLE_MODS ON CACHE BOOL
	"When enabled, example mods will be built.")

set(ABIT_CI_BUILD OFF CACHE BOOL
	"When enabled, the build will behave as if we were building on CI (procs/mapping generation will be disabled.)")

if(${ABIT_CI_BUILD})
	message(STATUS "CI build - procs and mapping generation will be disabled.")
	set(ABIT_GENERATE_PROCS OFF)
	set(ABIT_GENERATE_MAPPINGS OFF)
endif()

# Checks

if (${ABIT_GENERATE_PROCS} OR ${ABIT_GENERATE_MAPPINGS})
	message(STATUS "A Hat in Time install directory: " ${ABIT_HATINTIME_PATH})

	if(NOT EXISTS ${ABIT_HATINTIME_PATH}/Binaries/Win64/HatinTimeGame.exe)
		message(SEND_ERROR "The specified Hat in Time installation directory is not valid. Please specify a path using -DABIT_HATINTIME_PATH=...")
	endif()

	set(ABIT_HATINTIME_GAME_PDB ${ABIT_HATINTIME_PATH}/Binaries/Win64/HatinTimeGame.pdb)
	if(NOT EXISTS ${ABIT_HATINTIME_GAME_PDB})
		message(SEND_ERROR
			"It appears that HatinTimeGame.pdb does not exist inside the game's install directory "
			"(looked at " ${ABIT_HATINTIME_GAME_PDB} ")."
			"It's possible that you're using a release of the game that does not include it. "
			"Whatever it is, ABiT will not be able to run with this installation."
		)
	endif()
	message(STATUS "Using HatinTimeGame.pdb file: " ${ABIT_HATINTIME_GAME_PDB})
endif()

# Targets

add_subdirectory("src/lib")

include_directories("src")
add_subdirectory("src/abit")
add_subdirectory("src/mods")

