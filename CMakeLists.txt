cmake_minimum_required(VERSION 3.24)

project(abit
	VERSION 0.1.0
	DESCRIPTION "A Hat in Time hacking toolkit"
	LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)

add_subdirectory("src/lib")

if(NOT ABIT_HATINTIME_PATH)
	set(ABIT_HATINTIME_PATH "C:/Program Files (x86)/Steam/steamapps/common/HatinTime")
endif()
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

include_directories("src")
add_subdirectory("src/abit")
add_subdirectory("src/mods")

