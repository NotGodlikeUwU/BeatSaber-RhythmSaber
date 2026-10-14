param([string]$Package)
$ErrorActionPreference='Stop'
$taskRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if(-not $Package){$taskConfig=Get-Content (Join-Path $taskRoot 'qpm.json') -Raw | ConvertFrom-Json;$Package=Join-Path $taskRoot $taskConfig.workspace.qmodOutput}
$taskVerifier=Join-Path $taskRoot 'scripts/verify-package.ps1'
& $taskVerifier -Package $Package
$taskFixtures=Join-Path $taskRoot 'build-tests/package-fixtures'
New-Item -ItemType Directory -Path $taskFixtures -Force | Out-Null
function CheckRejected([string]$case,[string]$expected) {
  $taskOutput=Join-Path $taskFixtures ($case+'.qmod')
  Copy-Item -LiteralPath $Package -Destination $taskOutput -Force
  $taskZip=[IO.Compression.ZipFile]::Open($taskOutput,[IO.Compression.ZipArchiveMode]::Update)
  try {
    $taskEntry=$taskZip.GetEntry('mod.json');$taskReader=[IO.StreamReader]::new($taskEntry.Open())
    try{$taskManifest=$taskReader.ReadToEnd() | ConvertFrom-Json}finally{$taskReader.Dispose()}
    if($case -eq 'partial-version'){$taskManifest.version='0.1'}
    elseif($case -eq 'exact-dependency'){$taskManifest.dependencies[0].version='='+$taskManifest.dependencies[0].version.Substring(1)}
    $taskEntry.Delete();$taskEntry=$taskZip.CreateEntry('mod.json')
    $taskWriter=[IO.StreamWriter]::new($taskEntry.Open(),[Text.UTF8Encoding]::new($false))
    try{$taskWriter.Write(($taskManifest | ConvertTo-Json -Depth 20))}finally{$taskWriter.Dispose()}
  } finally {$taskZip.Dispose()}
  $taskError=$null
  try { & $taskVerifier -Package $taskOutput | Out-Null } catch { $taskError=$_.Exception.Message }
  if(-not $taskError -or $taskError -notmatch $expected){throw "Fixture $case did not fail for the expected reason: $taskError"}
  "PASS: $case rejected: $taskError"
}
CheckRejected 'partial-version' 'QMOD schema validation failed.*(/version|regular expression)'
CheckRejected 'exact-dependency' 'must use a caret SemVer range'
'PASS: 3 package regression checks (valid QMOD, partial SemVer rejection, exact dependency rejection).'
