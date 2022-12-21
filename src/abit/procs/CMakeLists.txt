add_custom_command(
	OUTPUT procs.cpp
	COMMAND python
		${CMAKE_CURRENT_SOURCE_DIR}/generator/__main__.py
		--input ${CMAKE_CURRENT_SOURCE_DIR}/procs.csv
		--output-source procs.cpp
		--output-headers include/abit/procs
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/procs.csv ${CMAKE_CURRENT_SOURCE_DIR}/generator
	VERBATIM
)

add_library(AByteInTime.Procs SHARED "procs.cpp")
target_include_directories(AByteInTime.Procs PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
