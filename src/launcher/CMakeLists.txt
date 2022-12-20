set(ABIT_LAUNCHER_SOURCES
	"config_default.cpp"
	"config.cpp"
	"paths.cpp"
	"process.cpp"

	"main.cpp"
)

add_executable(AByteInTime ${ABIT_LAUNCHER_SOURCES})
target_link_libraries(AByteInTime
	inih
	AByteInTime.Common
)
