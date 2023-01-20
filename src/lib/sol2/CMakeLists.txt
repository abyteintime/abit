add_subdirectory("lua")

add_library(sol2 INTERFACE)
target_link_libraries(sol2 INTERFACE
	lua5.4
)
target_include_directories(sol2 INTERFACE "./include")
