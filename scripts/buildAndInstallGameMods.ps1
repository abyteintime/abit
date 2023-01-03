param (
	[string] $Mod = "*",
	[string] $GameInstallDirectory = "C:\Program Files (x86)\Steam\steamapps\common\HatinTime"
)

$repoModDirectory = "src\gamemods"
$gameModDirectory = $GameInstallDirectory + "\HatinTimeGame\Mods"
$editorExe = $GameInstallDirectory + "\Binaries\Win64\HatinTimeEditor.exe"

$makeLogFile = $GameInstallDirectory + "\HatinTimeGame\Logs\make.txt"
$cookLogFile = $GameInstallDirectory + "\HatinTimeGame\Logs\cook.txt"

function Get-ErrorLines {
	param (
		[string] $File
	)

	$log = Get-Content $File
	$errorLines = Write-Output $log | Select-String "^\[.*?\] (Warning|Error):" -List
	return $errorLines
}

Get-ChildItem -Path $repoModDirectory -Filter $Mod | ForEach-Object {
	$modName = $_.BaseName
	$modDirectoryInGame = ($gameModDirectory + "\" + $modName)

	if (Test-Path $modDirectoryInGame) {
		Write-Output "Removing existing ${modName}"
		Remove-Item -Path $modDirectoryInGame -Recurse
	}

	Write-Output "Copying ${modName}"
	Copy-Item `
		-Path ($repoModDirectory + "\" + $modName) `
		-Destination ($gameModDirectory) `
		-Recurse

	Write-Output "Compiling scripts"
	Start-Process $editorExe -ArgumentList "Make -Full -ShortPaths -ModsOnly=$modName -unattended -silent -Log=make.txt" -Wait
	$errorLines = Get-ErrorLines $makeLogFile
	Write-Output $errorLines
	if ($errorLines.Matches.Length -gt 0) {
		Write-Output ""
		Write-Output "--------------------"
		Write-Output "Compiling scripts failed!"
		Write-Output ""
		return
	}

	Write-Output "Cooking mod"
	Start-Process $editorExe `
		-ArgumentList "CookPackages -Platform=PC -Full -FastCook -MultiLanguageCook=INT -ModsOnly=$modName -NoGADWarning -unattended -silent -Log=cook.txt" `
		-Wait
	$errorLines = Get-ErrorLines $cookLogFile
	Write-Output $errorLines
	if ($errorLines.Matches.Length -gt 0) {
		Write-Output ""
		Write-Output "--------------------"
		Write-Output "Cooking failed!"
		Write-Output ""
		return
	}
}

