param([string]$Ndk, [string]$Ninja)
$ErrorActionPreference='Stop'
$taskRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if(-not $Ndk) {
  if(Test-Path (Join-Path $taskRoot 'ndkpath.txt')) { $Ndk=(Get-Content (Join-Path $taskRoot 'ndkpath.txt') -Raw).Trim() }
  else { $Ndk=$env:ANDROID_NDK_HOME }
}
if(-not $Ndk -or -not (Test-Path (Join-Path $Ndk 'build/cmake/android.toolchain.cmake'))) { throw 'Specify -Ndk or set ANDROID_NDK_HOME to Android NDK r27d.' }
if(-not $Ninja) { $taskCmd=Get-Command ninja -ErrorAction SilentlyContinue; if($taskCmd) { $Ninja=$taskCmd.Source } }
if(-not $Ninja -and (Test-Path 'H:\BSTheater_Quest\tools\qpm\ninja.exe')) { $Ninja='H:\BSTheater_Quest\tools\qpm\ninja.exe' }
if(-not $Ninja) { throw 'Ninja is required. Run qpm download ninja and add it to PATH.' }
if(-not (Test-Path (Join-Path $taskRoot 'extern.cmake'))) { throw 'Run qpm restore first.' }
& cmake -S $taskRoot -B (Join-Path $taskRoot 'build') -G Ninja "-DCMAKE_ANDROID_NDK=$Ndk" "-DCMAKE_MAKE_PROGRAM=$Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo
if($LASTEXITCODE -ne 0) { throw 'CMake configure failed' }
& cmake --build (Join-Path $taskRoot 'build') -j 4
if($LASTEXITCODE -ne 0) { throw 'Android build failed' }
