set(INIH_SOURCES "ini.c")
add_library(inih STATIC ${INIH_SOURCES})

set(SPDLOG_BUILD_SHARED ON)
set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_WCHAR_SUPPORT ON)

set(JSON_Diagnostics ON)

add_subdirectory("fmt")
add_subdirectory("spdlog")
add_subdirectory("minhook")
add_subdirectory("nlohmann_json")
