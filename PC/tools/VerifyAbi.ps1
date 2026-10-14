param([string]$GameDir = 'H:\SteamLibrary\steamapps\common\Beat Saber\BSManager\BSInstances\1.40.8')
$ErrorActionPreference='Stop'
Add-Type -LiteralPath "$GameDir\Libs\Mono.Cecil.dll"
$root=Split-Path $PSScriptRoot -Parent
$resolver=[Mono.Cecil.DefaultAssemblyResolver]::new()
foreach($dir in @('Beat Saber_Data\Managed','Plugins','Libs')){$resolver.AddSearchDirectory("$GameDir\$dir")}
$rp=[Mono.Cecil.ReaderParameters]::new();$rp.AssemblyResolver=$resolver
$types=@{}; $modules=[Collections.Generic.List[object]]::new()
function Index($t) {$types[$t.FullName]=$t;foreach($n in $t.NestedTypes){Index $n}}
foreach($name in @('Main','GameplayCore','DataModels','BeatmapCore')){
 $m=[Mono.Cecil.ModuleDefinition]::ReadModule("$GameDir\Beat Saber_Data\Managed\$name.dll",$rp);$modules.Add($m);foreach($t in $m.Types){Index $t}
}
foreach($name in @('BSML','SiraUtil','BS_Utils','BeatLeader','ScoreSaber')){
 $file="$GameDir\Plugins\$name.dll";if(Test-Path $file){$m=[Mono.Cecil.ModuleDefinition]::ReadModule($file,$rp);$modules.Add($m);foreach($t in $m.Types){Index $t}}
}
$checks=0
function Require($ok,$message){if(-not $ok){throw $message};$script:checks++}
function PlainType($t) {while($t -is [Mono.Cecil.TypeSpecification]){$t=$t.ElementType};return $t.FullName}
$plugin=[Mono.Cecil.ModuleDefinition]::ReadModule("$root\bin\Release\net472\RhythmSaber.dll",$rp)
foreach($hook in $plugin.Types){
 foreach($attr in $hook.CustomAttributes){
  if($attr.AttributeType.FullName -ne 'HarmonyLib.HarmonyPatch' -or $attr.ConstructorArguments.Count -ne 2){continue}
  $typeName=$attr.ConstructorArguments[0].Value.FullName;$methodName=$attr.ConstructorArguments[1].Value
  Require ($types.ContainsKey($typeName)) "Missing target type $typeName"
  $methods=@($types[$typeName].Methods|Where-Object Name -eq $methodName)
  Require ($methods.Count -eq 1) "Ambiguous/missing target $typeName.$methodName"
  $target=$methods[0]
  foreach($patch in $hook.Methods|Where-Object {$_.Name -in @('Prefix','Postfix')}){
   foreach($p in $patch.Parameters){
    if($p.Name -match '^__(\d+)$'){
     $index=[int]$Matches[1];Require ($index -lt $target.Parameters.Count) "$($hook.Name): invalid argument index"
     Require ((PlainType $p.ParameterType) -eq (PlainType $target.Parameters[$index].ParameterType)) "$($hook.Name): argument type mismatch"
    } elseif($p.Name -match '^___(.+)$'){
     $field=$Matches[1];$f=$types[$typeName].Fields|Where-Object Name -eq $field
     Require ($null -ne $f) "$($hook.Name): missing injected field $field"
     Require ((PlainType $p.ParameterType) -eq $f.FieldType.FullName) "$($hook.Name): field type mismatch $field"
    } elseif($p.Name -eq '__result'){
     Require ((PlainType $p.ParameterType) -eq $target.ReturnType.FullName) "$($hook.Name): result type mismatch"
    } elseif($p.Name -eq '__instance'){
     Require ((PlainType $p.ParameterType) -eq $typeName) "$($hook.Name): instance mismatch"
    } else {throw "Unchecked patch parameter $($hook.Name).$($p.Name)"}
   }
  }
 }
}
$sources=Get-ChildItem "$root\src" -Filter *.cs
foreach($src in $sources){
 $code=Get-Content $src.FullName -Raw
 foreach($match in [regex]::Matches($code,'FieldRefAccess<([^,]+),\s*([^>]+)>\("([^"]+)"\)')){
  $type=$match.Groups[1].Value.Trim();$field=$match.Groups[3].Value
  Require ($types.ContainsKey($type)) "Missing FieldRef type $type"
  Require ($null -ne ($types[$type].Fields|Where-Object Name -eq $field)) "Missing FieldRef $type.$field"
 }
}
foreach($field in @('_duration','_xSpread','_targetYPos','_targetZPos','_color','_fontSize','_shake')){
 Require ($null -ne ($types['FlyingTextSpawner'].Fields|Where-Object Name -eq $field)) "Missing failure spawner field $field"
}
if($types.ContainsKey('BeatLeader.ReplayRecorder')) { Require ($null -ne ($types['BeatLeader.ReplayRecorder'].Fields|Where-Object Name -eq '_replay')) 'Missing BeatLeader replay field' }
foreach($spec in @(@('BeatLeader.ReplayRecorder','FinalizeReplay',1),@('BeatLeader.Utils.ScoreUtil','ProcessReplay',2),@('BeatLeader.Utils.ScoreUtil','UploadReplay',1),@('BeatLeader.Utils.ScoreUtil','UploadPlay',2),@('ScoreSaber.Core.Daemons.UploadDaemon','Five',5))){
 if($types.ContainsKey($spec[0])) { Require ($null -ne ($types[$spec[0]].Methods | Where-Object {$_.Name -eq $spec[1] -and $_.Parameters.Count -eq $spec[2]})) "Missing audited guard target $($spec[0]).$($spec[1])" }
}
# Prove that every actual BeatLeader call to saving a new replay passes through ProcessReplay.
$saveCallers=[Collections.Generic.List[string]]::new()
function FindSave($t){
 foreach($fn in $t.Methods){if($fn.HasBody){foreach($i in $fn.Body.Instructions){
  if($i.OpCode.Code -in @([Mono.Cecil.Cil.Code]::Call,[Mono.Cecil.Cil.Code]::Callvirt) -and $i.Operand.FullName -match 'BeatLeader.Utils.ReplayManager::SaveReplayAsync\('){$saveCallers.Add($fn.FullName)}
 }}}
 foreach($n in $t.NestedTypes){FindSave $n}
}
foreach($t in ($modules|Where-Object Name -eq 'BeatLeader.dll').Types){FindSave $t}
if($types.ContainsKey('BeatLeader.ReplayRecorder')) { Require ($saveCallers.Count -eq 1 -and $saveCallers[0] -match 'ScoreUtil::<ProcessReplay>') 'BeatLeader saving bypasses audited ProcessReplay guard' }
foreach($value in 1..8){Require ($null -ne ($types['NoteData/ScoringType'].Fields|Where-Object {$_.HasConstant -and $_.Constant -eq $value})) "Missing scoring type $value"}
[xml]$bsml=Get-Content "$root\UI\settings.bsml" -Raw
Require ($bsml.DocumentElement.Name -eq 'bg') 'Settings root must not collapse tabs through a VerticalLayoutGroup'
$manifest=$plugin.Resources|Where-Object Name -eq 'RhythmSaber.manifest.json'
Require ($null -ne $manifest) 'Manifest not embedded'
$json=[Text.Encoding]::UTF8.GetString($manifest.GetResourceData())|ConvertFrom-Json
Require ($json.gameVersion -eq '1.40.8') 'Wrong target game version'
Require ($null -ne ($plugin.Resources|Where-Object Name -eq 'RhythmSaber.UI.settings.bsml')) 'BSML not embedded'
"PASS: $checks ABI/hook/guard/resource checks against PC 1.40.8."
foreach($m in $modules){$m.Dispose()};$plugin.Dispose();$resolver.Dispose()
