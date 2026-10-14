param(
 [Parameter(Mandatory=$true)][string]$BeatSaberDir,
 [string]$Dotnet = 'dotnet',
 [switch]$SkipHarness
)
$ErrorActionPreference='Stop'
Set-Location $PSScriptRoot
$game=(Resolve-Path -LiteralPath $BeatSaberDir).Path
if((Get-Content -LiteralPath "$game\BeatSaberVersion.txt" -Raw).Trim() -notlike '1.40.8_*') { throw 'PC Beat Saber 1.40.8 is required.' }
New-Item -ItemType Directory -Path logs -Force | Out-Null
function Build([string]$project,[string]$log){
 & $Dotnet build $project -c Release --nologo -v minimal "-p:BeatSaberDir=$game" 2>&1 | Tee-Object -FilePath $log
 if($LASTEXITCODE -ne 0){throw "Build failed: $project"}
}
Build RhythmSaber.csproj logs/build.txt
Build tests/RhythmSaber.Tests.csproj logs/tests-build.txt
& .\tests\bin\Release\net472\RhythmSaber.Tests.exe 2>&1 | Tee-Object -FilePath logs/tests.txt
if($LASTEXITCODE -ne 0){throw 'Math tests failed'}
& .\tools\VerifyAbi.ps1 -GameDir $game 2>&1 | Tee-Object -FilePath logs/abi.txt
if(-not $SkipHarness){
 Build tools/RuntimeHarness/RuntimeHarness.csproj logs/harness-build.txt
 & .\tools\RuntimeHarness\bin\Release\net472\RuntimeHarness.exe $game .\bin\Release\net472\RhythmSaber.dll 2>&1 | Tee-Object -FilePath logs/harness.txt
 if($LASTEXITCODE -ne 0){throw 'Managed scoring harness failed'}
}
& .\tools\Package.ps1
