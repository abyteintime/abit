set(INIH_SOURCES "ini.c")
add_library(inih STATIC ${INIH_SOURCES})

add_subdirectory("minhook")
