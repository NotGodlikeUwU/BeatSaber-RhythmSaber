param([string]$Assembly='Main',[string]$Type='',[string]$Method='',[switch]$IL,[switch]$Plugins)
$g='H:\SteamLibrary\steamapps\common\Beat Saber\BSManager\BSInstances\1.40.8'
Add-Type -LiteralPath "$g\Libs\Mono.Cecil.dll"
$dir = if($Plugins){'Plugins'}else{'Beat Saber_Data\Managed'}
$m=[Mono.Cecil.ModuleDefinition]::ReadModule("$g\$dir\$Assembly.dll")
function Dump($t) {
 if($t.FullName -match $Type){
  "TYPE $($t.FullName)"
  if(-not $Method){$t.Fields | ForEach-Object {" FIELD $($_.FullName)"}}
  foreach($fn in $t.Methods){if($fn.Name -match $Method){" METHOD $($fn.FullName) PUBLIC=$($fn.IsPublic)"; if($IL -and $fn.HasBody){$fn.Body.Instructions|ForEach-Object{"  $_"}}}}
 }
 foreach($n in $t.NestedTypes){Dump $n}
}
foreach($t in $m.Types){Dump $t}
$m.Dispose()
