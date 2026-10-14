$ErrorActionPreference='Stop'
$taskRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$taskConfig=Get-Content (Join-Path $taskRoot 'qpm.json') -Raw | ConvertFrom-Json
$taskArchive=Join-Path $taskRoot ("dist/RhythmSaber-"+$taskConfig.info.version+"-source.zip")
New-Item -ItemType Directory -Force (Join-Path $taskRoot 'dist') | Out-Null
Add-Type -AssemblyName System.IO.Compression.FileSystem
if(Test-Path -LiteralPath $taskArchive) { Remove-Item -LiteralPath $taskArchive }
$taskZip=[IO.Compression.ZipFile]::Open($taskArchive,[IO.Compression.ZipArchiveMode]::Create)
try {
  $taskFiles=@('CMakeLists.txt','qpm.json','qpm.shared.json','mod.template.json','.gitignore','README.md','LICENSE','THIRD_PARTY_NOTICES.md') | ForEach-Object { Get-Item (Join-Path $taskRoot $_) }
  foreach($taskDir in @('include','src','cmake','scripts','tests','docs','shared','third_party')) { $taskFiles+=Get-ChildItem (Join-Path $taskRoot $taskDir) -File -Recurse }
  foreach($taskFile in $taskFiles) {
    $taskRelative=[IO.Path]::GetRelativePath($taskRoot,$taskFile.FullName).Replace('\','/')
    [IO.Compression.ZipFileExtensions]::CreateEntryFromFile($taskZip,$taskFile.FullName,$taskRelative,[IO.Compression.CompressionLevel]::Optimal) | Out-Null
  }
} finally { $taskZip.Dispose() }
Write-Output $taskArchive
