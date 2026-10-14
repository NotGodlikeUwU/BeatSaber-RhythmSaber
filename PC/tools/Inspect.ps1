param([string]$GameDir = 'H:\SteamLibrary\steamapps\common\Beat Saber\BSManager\BSInstances\1.40.8', [string]$Mode = 'game')
Add-Type -LiteralPath "$GameDir\Libs\Mono.Cecil.dll"
$names = @('ScoreController','ScoreModel','GoodCutScoringElement','NoteCutInfo','NoteJump','PlayerTransforms','StandardLevelScenesTransitionSetupDataSO','StandardLevelSceneSetupData','LevelCompletionResults','ResultsViewController','FlyingScoreSpawner','FlyingScoreEffect','CutScoreBuffer','GameNoteController','BurstSliderGameNoteController','NoteController','PlatformLeaderboardsModel','PlayerDataModel','PartyLeaderboardViewController')
$paths = @('Main','GameplayCore','DataModels','BeatmapCore') | ForEach-Object { "$GameDir\Beat Saber_Data\Managed\$_.dll" }
if ($Mode -eq 'plugins') { $paths = @('BS_Utils','BSML','BeatLeader','ScoreSaber') | ForEach-Object { "$GameDir\Plugins\$_.dll" } }
foreach($path in $paths) {
 $mod=[Mono.Cecil.ModuleDefinition]::ReadModule($path)
 "ASSEMBLY $path"
 foreach($res in $mod.Resources) { if($res.Name -match 'manifest') { "MANIFEST $($res.Name)"; [Text.Encoding]::UTF8.GetString($res.GetResourceData()) } }
 foreach($type in $mod.Types) {
  $match = $names -contains $type.Name
  if($Mode -eq 'plugins') { $match = $type.FullName -match 'ScoreSubmission|Upload|ReplayManager|BSMLSettings|GameplayModifiers|ScoreSaberAPI' }
  if(-not $match) { continue }
  "TYPE $($type.FullName)"
  $type.Fields | ForEach-Object { " FIELD $($_.FullName)" }
  foreach($m in $type.Methods) {
   " METHOD $($m.FullName)"
   if($m.Name -match 'RefreshScores|get_cutScore|HandleNoteWasCut|InitAndPresent|ProcessScore|Upload|Submit|Finish|DisableSubmission|EnableSubmission|WasDisabled|StartSong') {
    $m.Body.Instructions | ForEach-Object { "  $_" }
   }
  }
 }
 $mod.Dispose()
}
