set(ABIT_UE_SOURCES
	"AGameMod.cpp"
	"FName.cpp"
	"FString.cpp"
	"UClass.cpp"
	"UFunction.cpp"
	"UObject.cpp"
	"UState.cpp"
	"UStruct.cpp"
	"memory.cpp"
)

add_library(AByteInTime.UnrealEngine SHARED ${ABIT_UE_SOURCES})
target_link_libraries(AByteInTime.UnrealEngine
	AByteInTime.Common
	AByteInTime.Procs
)
