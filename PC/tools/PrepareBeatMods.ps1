$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$manifest=Get-Content -LiteralPath "$root\manifest.json" -Raw | ConvertFrom-Json
$version=$manifest.version
$summary=(Get-Content -LiteralPath "$root\publication\summary.txt" -Raw).Trim()
$description=Get-Content -LiteralPath "$root\publication\description.md" -Raw
if($summary.Length -lt 10 -or $summary.Length -gt 100){throw 'Catalog summary must be 10–100 characters.'}
if($description.Length -lt 3 -or $description.Length -gt 4096){throw 'Catalog description must be 3–4096 characters.'}
$dist=Join-Path $root 'dist'
$kit=Join-Path $dist "RhythmSaber-PC-$version-BeatMods"
New-Item -ItemType Directory -Path $kit -Force | Out-Null
foreach($name in @('summary.txt','description.md','icon.svg','icon.png')){
 Copy-Item -LiteralPath "$root\publication\$name" -Destination "$kit\$name"
}
Copy-Item -LiteralPath "$root\docs\BEATMODS.md" -Destination "$kit\README.md"
Copy-Item -LiteralPath "$root\README.md" -Destination "$kit\MOD-README.md"
Copy-Item -LiteralPath "$root\docs\TESTING.md" -Destination "$kit\TESTING.md"
$license=Join-Path $root 'LICENSE'
if(-not (Test-Path -LiteralPath $license)){$license=Join-Path $root '..\LICENSE'}
Copy-Item -LiteralPath $license -Destination "$kit\LICENSE"
$files=@("RhythmSaber-PC-$version.zip","RhythmSaber-PC-$version-source.zip")
foreach($name in $files){Copy-Item -LiteralPath "$dist\$name" -Destination "$kit\$name"}
$dll="$dist\RhythmSaber-PC-$version\Plugins\RhythmSaber.dll"
$record=[ordered]@{
 purpose='Local publication preparation only; not a BeatMods API payload'
 publicationStatus='not-submitted'
 name=$manifest.name; modId=$manifest.id; version=$version
 gameName='BeatSaber'; supportedGameVersions=@($manifest.gameVersion)
 platform='universalpc'; category='gameplay'; summary=$summary
 sourceRepository=$manifest.links.'project-source'
 actionRequired='Complete live validation and submit through BeatMods with your author account before catalog verification.'
 dependencies=$manifest.dependsOn
 dll=[ordered]@{path='Plugins/RhythmSaber.dll';sha256=(Get-FileHash -LiteralPath $dll -Algorithm SHA256).Hash;md5=(Get-FileHash -LiteralPath $dll -Algorithm MD5).Hash}
 installZip=[ordered]@{path=$files[0];sha256=(Get-FileHash -LiteralPath "$kit\$($files[0])" -Algorithm SHA256).Hash}
}
$record | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath "$kit\publication.json" -Encoding UTF8
$stage=Join-Path $dist "RhythmSaber-PC-$version"
$content=@(Get-ChildItem -LiteralPath $stage -Recurse -File | Sort-Object FullName | ForEach-Object {
 [ordered]@{path=$_.FullName.Substring($stage.Length+1).Replace('\','/');hash=(Get-FileHash -LiteralPath $_.FullName -Algorithm MD5).Hash.ToLowerInvariant()}
})
ConvertTo-Json -InputObject $content -Depth 4 | Set-Content -LiteralPath "$kit\CONTENT-HASHES.json" -Encoding UTF8
Get-ChildItem -LiteralPath $kit -File | Where-Object Name -ne 'SHA256SUMS.txt' | Sort-Object Name | ForEach-Object {
 "$((Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash)  $($_.Name)"
} | Set-Content -LiteralPath "$kit\SHA256SUMS.txt" -Encoding UTF8
$archive=Join-Path $dist "RhythmSaber-PC-$version-BeatMods.zip"
Compress-Archive -Path "$kit\*" -DestinationPath $archive -Force
Write-Output "Prepared only; nothing uploaded: $archive"
Get-FileHash -LiteralPath $archive -Algorithm SHA256
