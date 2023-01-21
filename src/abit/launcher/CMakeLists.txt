set(ABIT_LAUNCHER_SOURCES
	"paths.cpp"
	"process.cpp"

	"main.cpp"
)

add_executable(AByteInTime ${ABIT_LAUNCHER_SOURCES})
target_link_libraries(AByteInTime
	AByteInTime.Common
)

# In release mode we don't want to show the console window. The loader already spawns its own one
# and it has stuff way more useful to users than the launcher's console output.
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	set_target_properties(AByteInTime PROPERTIES
		WIN32_EXECUTABLE ON
	)
endif()
