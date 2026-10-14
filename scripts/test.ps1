param([string]$VcVars)
$ErrorActionPreference='Stop'
$taskRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if(-not $VcVars) {
  $taskVsWhere='C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
  $taskVs=& $taskVsWhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
  if(-not $taskVs) { throw 'Install Visual Studio C++ Build Tools for portable tests.' }
  $VcVars=Join-Path $taskVs 'VC/Auxiliary/Build/vcvars64.bat'
}
$taskBuild=Join-Path $taskRoot 'build-tests'
New-Item -ItemType Directory -Force $taskBuild | Out-Null
$taskScript=Join-Path $taskBuild 'run.cmd'
@"
@echo off
call "$VcVars" >nul
cl /nologo /std:c++20 /EHsc /W4 /I "$taskRoot\include" /I "$taskRoot\extern\includes" "$taskRoot\src\TimingScoring.cpp" "$taskRoot\src\GuardPolicy.cpp" "$taskRoot\src\MapCompatibility.cpp" "$taskRoot\src\TrajectoryTiming.cpp" "$taskRoot\tests\timing_tests.cpp" /Fe:"$taskBuild\timing_tests.exe" /Fo:"$taskBuild\\"
if errorlevel 1 exit /b %errorlevel%
"$taskBuild\timing_tests.exe"
exit /b %errorlevel%
"@ | Set-Content -LiteralPath $taskScript
& cmd /c $taskScript
if($LASTEXITCODE -ne 0) { throw 'Timing tests failed' }
