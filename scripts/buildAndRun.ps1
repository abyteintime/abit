param (
	[switch] $Release = $false
)

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
Write-Output "RUN"
Write-Output ""
if ($Release) {
	.\build\bin\Release\AByteInTime.exe
}
else {
	.\build\bin\Debug\AByteInTime.exe
}
