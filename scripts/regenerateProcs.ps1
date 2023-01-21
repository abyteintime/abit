$hatPath = "C:\Program Files (x86)\Steam\steamapps\common\HatinTime"
$pdbPath = $hatPath + "\Binaries\Win64\HatinTimeGame.pdb"
$exePath = $hatPath + "\Binaries\Win64\HatinTimeGame.exe"

cargo run `
	--manifest-path ".\abit-procs-generator\Cargo.toml" `
	--release `
	-- `
	--pdb "$pdbPath" `
	addresses `
	--output-source ".\procs\src\procs.cpp" `
	--output-headers ".\procs\include\abit\procs"

(Get-FileHash $exePath -Algorithm SHA256).Hash > ".\procs\GameVersion.txt"
