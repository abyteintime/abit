if (Test-Path build) {
	Write-Output "Removing existing build directory"
	Remove-Item -Recurse -Path build
}

Write-Output "Setting up new build directory"
New-Item -Path "build" -ItemType Directory
Set-Location build
cmake .. `
	-G Ninja `
	-DCMAKE_C_COMPILER=clang `
	-DCMAKE_CXX_COMPILER=clang++ `
	-DCMAKE_EXPORT_COMPILE_COMMANDS=true `
	$args
Set-Location ..
