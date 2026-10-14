param([string]$Assembly='BeatLeader',[string]$Pattern='ScoreSubmission|UploadReplay|ReplayManager::SaveReplay', [switch]$Game)
$g='H:\SteamLibrary\steamapps\common\Beat Saber\BSManager\BSInstances\1.40.8'
Add-Type -LiteralPath "$g\Libs\Mono.Cecil.dll"
$dir=if($Game){'Beat Saber_Data\Managed'}else{'Plugins'}
$m=[Mono.Cecil.ModuleDefinition]::ReadModule("$g\$dir\$Assembly.dll")
function Scan($t){foreach($fn in $t.Methods){if($fn.HasBody){foreach($i in $fn.Body.Instructions){if($i.Operand -and $i.Operand.ToString() -match $Pattern){"$($fn.FullName) -> $i"}}}};foreach($n in $t.NestedTypes){Scan $n}}
foreach($t in $m.Types){Scan $t};$m.Dispose()
