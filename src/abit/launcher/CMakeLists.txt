set(ABIT_LAUNCHER_SOURCES
	"paths.cpp"
	"process.cpp"

	"main.cpp"
)

add_executable(AByteInTime ${ABIT_LAUNCHER_SOURCES})
target_link_libraries(AByteInTime
	AByteInTime.Common
)
