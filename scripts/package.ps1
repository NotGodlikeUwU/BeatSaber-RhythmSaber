param([switch]$SkipBuild,[string]$Qpm='qpm')
$ErrorActionPreference='Stop'
$taskRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Push-Location $taskRoot
try {
  if(-not $SkipBuild) { & (Join-Path $PSScriptRoot 'build.ps1') }
  if(-not (Get-Command $Qpm -ErrorAction SilentlyContinue) -and (Test-Path 'H:\BSTheater_Quest\tools\qpm\qpm.exe')) { $Qpm='H:\BSTheater_Quest\tools\qpm\qpm.exe' }
  New-Item -ItemType Directory -Force dist | Out-Null
  & $Qpm qmod manifest --offline
  if($LASTEXITCODE -ne 0) { throw 'QMOD manifest generation failed' }
  $taskManifestJson=Get-Content mod.json -Raw
  if(-not ($taskManifestJson | Test-Json -SchemaFile (Join-Path $taskRoot 'schemas/qmod.schema.json') -ErrorAction Stop)) { throw 'Generated QMOD manifest schema validation failed' }
  & $Qpm qmod zip --skip_build --offline
  if($LASTEXITCODE -ne 0) { throw 'QMOD packaging failed' }
  & (Join-Path $PSScriptRoot 'verify-package.ps1')
} finally { Pop-Location }
