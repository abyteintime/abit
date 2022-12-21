#include "abit/launcher/process.hpp"

#include <system_error>

#include "abit/error.hpp"

using namespace abit;

Process::Process(std::wstring_view executablePath, std::wstring_view workingDirectory)
{
	STARTUPINFOW startupInfo = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION processInfo;
	if (!CreateProcessW(
			executablePath.data(),
			nullptr,
			nullptr,
			nullptr,
			true,
			CREATE_SUSPENDED,
			nullptr,
			workingDirectory.data(),
			&startupInfo,
			&processInfo
		)) {
		throw Error::System("could not start the game: ");
	}
	handle = processInfo.hProcess;
	threadHandle = processInfo.hThread;
}

Process::~Process()
{
	TerminateProcess(handle, 0x100B5);
	WaitForExit();
	CloseHandle(handle);
	CloseHandle(threadHandle);
}

void
Process::WaitForExit()
{
	WaitForSingleObject(handle, INFINITE);
}
