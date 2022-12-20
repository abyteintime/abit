cmake_minimum_required(VERSION 3.24)

project(abit
	VERSION 0.1.0
	DESCRIPTION "A Hat in Time hacking toolkit"
	LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory("src/lib")

include_directories("src/abit")
add_subdirectory("src/abit")
add_subdirectory("src/launcher")
add_subdirectory("src/loader")

