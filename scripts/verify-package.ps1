param([string]$Package)
$ErrorActionPreference='Stop'
$taskRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$taskConfig=Get-Content (Join-Path $taskRoot 'qpm.json') -Raw | ConvertFrom-Json
if(-not $Package) { $Package=Join-Path $taskRoot $taskConfig.workspace.qmodOutput }
Add-Type -AssemblyName System.IO.Compression.FileSystem
$taskZip=[IO.Compression.ZipFile]::OpenRead($Package)
try {
  $taskManifestEntry=$taskZip.GetEntry('mod.json')
  if(-not $taskManifestEntry) { throw 'Missing mod.json' }
  $taskReader=[IO.StreamReader]::new($taskManifestEntry.Open())
  try { $taskManifestJson=$taskReader.ReadToEnd() } finally { $taskReader.Dispose() }
  try {
    if(-not ($taskManifestJson | Test-Json -SchemaFile (Join-Path $taskRoot 'schemas/qmod.schema.json') -ErrorAction Stop)) { throw 'Manifest does not match schema' }
  } catch { throw "QMOD schema validation failed: $($_.Exception.Message)" }
  $taskManifest=$taskManifestJson | ConvertFrom-Json
  if(@($taskZip.Entries | Group-Object FullName | Where-Object Count -GT 1).Count) { throw 'Duplicate QMOD entries' }
  if($taskManifest.id -ne 'rhythmsaber' -or $taskManifest.packageVersion -ne '1.40.8_7379' -or $taskManifest.modloader -ne 'Scotland2') { throw 'Wrong target metadata' }
  if(@($taskManifest.lateModFiles).Count -ne 1 -or $taskManifest.lateModFiles[0] -ne 'librhythmsaber.so') { throw 'Wrong load phase or binary' }
  if(@($taskManifest.modFiles).Count -or @($taskManifest.libraryFiles).Count) { throw 'Unexpected early mods/bundled libraries' }
  $taskIds=@($taskManifest.dependencies | ForEach-Object { $_.id })
  foreach($taskId in @('beatsaber-hook','custom-types','paper2_scotland2','bsml','metacore')) { if($taskId -notin $taskIds) { throw "Missing runtime dependency: $taskId" } }
  if($taskManifest.version -ne $taskConfig.info.version) { throw 'Manifest mod version differs from qpm.json' }
  $taskTemplate=Get-Content (Join-Path $taskRoot 'mod.template.json') -Raw | ConvertFrom-Json
  if(@($taskManifest.dependencies | Group-Object id | Where-Object Count -GT 1).Count) { throw 'Duplicate runtime dependencies' }
  foreach($taskDependency in $taskManifest.dependencies) {
    $taskDeclared=@($taskTemplate.dependencies | Where-Object id -EQ $taskDependency.id)
    if($taskDependency.version -notmatch '^\^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)$') { throw "Runtime dependency $($taskDependency.id) must use a caret SemVer range, not an exact pin" }
    if($taskDeclared.Count -ne 1 -or $taskDependency.version -ne $taskDeclared[0].version -or $taskDependency.downloadIfMissing -ne $taskDeclared[0].downloadIfMissing) { throw "Runtime dependency $($taskDependency.id) differs from installer template" }
    $taskBuildDependency=@($taskConfig.dependencies | Where-Object id -EQ $taskDependency.id)
    if($taskBuildDependency.Count -ne 1 -or $taskBuildDependency[0].versionRange -ne ('='+$taskDependency.version.Substring(1))) { throw "Runtime minimum for $($taskDependency.id) differs from the audited build version" }
  }
  $taskMetaCore=@($taskManifest.dependencies | Where-Object id -EQ 'metacore')
  if($taskMetaCore.Count -ne 1 -or $taskMetaCore[0].version -ne '^1.6.3' -or $taskMetaCore[0].downloadIfMissing -ne 'https://github.com/Metalit/MetaCore/releases/download/v1.6.3/MetaCore.qmod') { throw 'MetaCore dependency must allow compatible updates from 1.6.3' }
  $taskEntry=$taskZip.GetEntry('librhythmsaber.so')
  if(-not $taskEntry) { throw 'Missing mod binary' }
  $taskStream=$taskEntry.Open()
  try {
    $taskBytes=[byte[]]::new(64)
    if($taskStream.Read($taskBytes,0,64) -ne 64) { throw 'Truncated ELF' }
    if($taskBytes[0] -ne 127 -or $taskBytes[1] -ne 69 -or $taskBytes[2] -ne 76 -or $taskBytes[3] -ne 70 -or $taskBytes[4] -ne 2 -or [BitConverter]::ToUInt16($taskBytes,18) -ne 183) { throw 'Not an ARM64 ELF' }
  } finally { $taskStream.Dispose() }
  $taskStream=$taskEntry.Open()
  $taskHash=[Security.Cryptography.SHA256]::Create()
  try { $taskPackagedHash=[Convert]::ToHexString($taskHash.ComputeHash($taskStream)) } finally { $taskStream.Dispose(); $taskHash.Dispose() }
  $taskBuiltHash=(Get-FileHash (Join-Path $taskRoot 'build/librhythmsaber.so') -Algorithm SHA256).Hash
  if($taskPackagedHash -ne $taskBuiltHash) { throw 'Packaged binary differs from the built binary' }
  Write-Output "PASS: MBF QMOD schema, full SemVer, caret runtime dependencies, ARM64 ELF, packaged/build SHA256 $taskBuiltHash"
  Get-FileHash $Package -Algorithm SHA256
} finally { $taskZip.Dispose() }
