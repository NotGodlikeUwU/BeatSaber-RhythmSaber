$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$dist=Join-Path $root 'dist'
$version=(Get-Content "$root\manifest.json" -Raw | ConvertFrom-Json).version
$stage=Join-Path $dist "RhythmSaber-PC-$version"
New-Item -ItemType Directory -Path "$stage\Plugins" -Force | Out-Null
Copy-Item -LiteralPath "$root\bin\Release\net472\RhythmSaber.dll" -Destination "$stage\Plugins\RhythmSaber.dll"
Copy-Item -LiteralPath "$root\bin\Release\net472\RhythmSaber.pdb" -Destination "$stage\Plugins\RhythmSaber.pdb"
Copy-Item -LiteralPath "$root\README.md" -Destination "$stage\README.md"
$license=Join-Path $root 'LICENSE'
if(-not (Test-Path -LiteralPath $license)){$license=Join-Path $root '..\LICENSE'}
Copy-Item -LiteralPath $license -Destination "$stage\LICENSE"
Copy-Item -LiteralPath "$root\docs\TESTING.md" -Destination "$stage\TESTING.md"
New-Item -ItemType Directory -Path "$stage\docs" -Force | Out-Null
Copy-Item -LiteralPath "$root\docs\TESTING.md","$root\docs\BEATMODS.md","$root\docs\RELEASE-0.2.4.md" -Destination "$stage\docs"
Compress-Archive -Path "$stage\*" -DestinationPath "$dist\RhythmSaber-PC-$version.zip" -Force
$source=Join-Path $dist "RhythmSaber-PC-$version-source"
New-Item -ItemType Directory -Path $source -Force | Out-Null
foreach($name in @('src','UI','docs','tests','tools','publication')){
 $inputDir=Join-Path $root $name
 foreach($file in Get-ChildItem -LiteralPath $inputDir -Recurse -File){
  $relative=$file.FullName.Substring($root.Length+1)
  if($relative -match '(^|\\)(bin|obj)(\\|$)'){continue}
  $target=Join-Path $source $relative
  New-Item -ItemType Directory -Path (Split-Path $target -Parent) -Force | Out-Null
  Copy-Item -LiteralPath $file.FullName -Destination $target
 }
}
foreach($name in @('RhythmSaber.csproj','manifest.json','README.md','build.ps1')){Copy-Item -LiteralPath "$root\$name" -Destination "$source\$name"}
Copy-Item -LiteralPath $license -Destination "$source\LICENSE"
Compress-Archive -Path "$source\*" -DestinationPath "$dist\RhythmSaber-PC-$version-source.zip" -Force
Get-FileHash "$dist\RhythmSaber-PC-$version.zip","$dist\RhythmSaber-PC-$version-source.zip","$stage\Plugins\RhythmSaber.dll" -Algorithm SHA256 |
 ForEach-Object { "$($_.Hash)  $([IO.Path]::GetFileName($_.Path))" } | Set-Content "$dist\SHA256SUMS.txt"
Get-Content "$dist\SHA256SUMS.txt"
& "$PSScriptRoot\PrepareBeatMods.ps1"
