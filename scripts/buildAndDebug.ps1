Write-Output ""
Write-Output "------------------------------"
Write-Output "BUILD"
Write-Output ""

.\scripts\build.ps1
if ($LastExitCode -ne 0) {
	Write-Output ""
	Write-Output "Compilation failed, the launcher will not run. (exit code $LastExitCode)"
	Exit
}

Write-Output ""
Write-Output "------------------------------"
Write-Output "DEBUG"
Write-Output ""
lldb .\build\AByteInTime.exe