#include "main.hpp"
#include "TimingManager.hpp"
#include "Visuals.hpp"
#include "ScoreSubmissionGuard.hpp"
#include "GlobalNamespace/GameplayCoreInstaller.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/StandardGameplayInstaller.hpp"
#include "GlobalNamespace/MultiplayerCoreInstaller.hpp"
#include "GlobalNamespace/MissionGameplayInstaller.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/GoodCutScoringElement.hpp"
#include "GlobalNamespace/LevelCompletionResultsHelper.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/PartyFreePlayFlowCoordinator.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/NoteSpawnData.hpp"
#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/FlyingScoreSpawner.hpp"
#include "GlobalNamespace/FlyingObjectEffect.hpp"
#include "GlobalNamespace/FlyingTextSpawner.hpp"
#include "GlobalNamespace/EffectPoolsManualInstaller.hpp"
#include "GlobalNamespace/IReadonlyCutScoreBuffer.hpp"
#include "Zenject/DiContainer.hpp"
#include "metacore/shared/unity.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/writer.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/stringbuffer.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

using namespace GlobalNamespace;
using namespace RhythmSaber;
namespace {
GameplayCoreSceneSetupData* pending{};
bool solo{};
AudioTimeSyncController* owner{};
bool Replace(CutScoreBuffer* buffer) {
  return Current().active && buffer && buffer->_noteCutInfo.noteData && TimingScoredType(static_cast<int>(buffer->_noteCutInfo.noteData->get_scoringType()));
}
int TimingScore(CutScoreBuffer* buffer) {
  auto judgement=FindJudgement(buffer->_noteCutInfo.noteData);
  return judgement ? judgement->score : 0;
}
}
MAKE_HOOK_MATCH(CoreInstall,&GameplayCoreInstaller::InstallBindings,void,GameplayCoreInstaller* self) {
  CoreInstall(self);
  pending=self->_sceneSetupData;
  Current().active=false;
}
MAKE_HOOK_MATCH(ClockStart,&AudioTimeSyncController::Start,void,AudioTimeSyncController* self) {
  // StandardGameplayInstaller exists only in solo/party, not multiplayer/mission.
  solo=false;
  auto installers=UnityEngine::Resources::FindObjectsOfTypeAll<StandardGameplayInstaller*>();
  for(auto installer:installers) if(installer && installer->get_gameObject()->get_activeInHierarchy() && installer->get_gameObject()->get_scene().get_isLoaded()) solo=true;
  pending=nullptr;
  auto cores=UnityEngine::Resources::FindObjectsOfTypeAll<GameplayCoreInstaller*>();
  for(auto core:cores) if(core && core->get_gameObject()->get_activeInHierarchy() && core->get_gameObject()->get_scene().get_isLoaded()) pending=core->_sceneSetupData;
  owner=self;
  if(solo) BeginLevel(pending,self);
  else { EndVisuals(); Current()={}; SubmissionGuard::NewLevel(); pending=nullptr; }
  if(getRhythmSaberConfig().Enabled.GetValue() && !Current().active) ShowBlocked(Current().reason.empty()?"Unsupported game mode.":Current().reason.c_str());
  ClockStart(self);
}
MAKE_HOOK_MATCH(ClockUpdate,&AudioTimeSyncController::Update,void,AudioTimeSyncController* self) {
  ClockUpdate(self);
  if(self==owner) TickFeedback(self);
}
MAKE_HOOK_MATCH(ClockPause,&AudioTimeSyncController::Pause,void,AudioTimeSyncController* self) {
  ClockPause(self); if(self==owner) { Current().paused=true; SetVisualsPaused(true); }
}
MAKE_HOOK_MATCH(ClockResume,&AudioTimeSyncController::Resume,void,AudioTimeSyncController* self) {
  ClockResume(self); if(self==owner) { Current().paused=false; SetVisualsPaused(false); }
}
MAKE_HOOK_MATCH(ClockDestroy,&AudioTimeSyncController::OnDestroy,void,AudioTimeSyncController* self) {
  if(self==owner) { EndVisuals(); owner=nullptr; pending=nullptr; }
  ClockDestroy(self);
}
MAKE_HOOK_MATCH(ScoreCut,&ScoreController::HandleNoteWasCut,void,ScoreController* self,NoteController* note,ByRef<NoteCutInfo> cut) {
  CaptureCut(note,cut.heldRef);
  ScoreCut(self,note,cut);
}
MAKE_HOOK_MATCH(ScoreMiss,&ScoreController::HandleNoteWasMissed,void,ScoreController* self,NoteController* note) {
  CaptureMiss(note); ScoreMiss(self,note);
}
MAKE_HOOK_MATCH(NoteInit,&NoteController::Init,void,NoteController* self,NoteData* note,ByRef<NoteSpawnData> spawn,float endRotation,float scale,bool rotateTowardsPlayer,bool randomRotation) {
  ResetNoteMotion(self); NoteInit(self,note,spawn,endRotation,scale,rotateTowardsPlayer,randomRotation);
}
MAKE_HOOK_MATCH(NoteUpdate,&NoteController::ManualUpdate,void,NoteController* self) {
  NoteUpdate(self); ObserveNote(self);
}
MAKE_HOOK_MATCH(BufferRefresh,&CutScoreBuffer::RefreshScores,void,CutScoreBuffer* self) {
  // 1.40.8 disassembly confirms RefreshScores only computes swing fields; it
  // does not dispatch events. Replace it before the caller notifies listeners.
  if(Replace(self)) { self->_beforeCutScore=TimingScore(self); self->_afterCutScore=0; self->_centerDistanceCutScore=0; }
  else BufferRefresh(self);
}
MAKE_HOOK_MATCH(BufferTotal,&CutScoreBuffer::get_cutScore,int,CutScoreBuffer* self) {
  return Replace(self)?self->_beforeCutScore:BufferTotal(self);
}
MAKE_HOOK_MATCH(GoodScore,&GoodCutScoringElement::get_cutScore,int,GoodCutScoringElement* self) {
  return Replace(self->_cutScoreBuffer)?self->_cutScoreBuffer->_beforeCutScore:GoodScore(self);
}
MAKE_HOOK_MATCH(DespawnScore,&ScoreController::DespawnScoringElement,void,ScoreController* self,ScoringElement* element) {
  auto note=element->get_noteData(); DespawnScore(self,element); ForgetJudgement(note);
}
MAKE_HOOK_MATCH(FlyingInit,&FlyingScoreEffect::InitAndPresent,void,FlyingScoreEffect* self,IReadonlyCutScoreBuffer* buffer,float duration,UnityEngine::Vector3 target,UnityEngine::Color color) {
  RestoreFlyingJudgement(self);
  if(Current().active && Current().settings.showJudgements) duration=std::max(duration,.9f);
  FlyingInit(self,buffer,duration,target,color);
  auto concrete=il2cpp_utils::try_cast<CutScoreBuffer>(buffer);
  if(concrete) AttachFlyingJudgement(self,*concrete);
}
MAKE_HOOK_MATCH(FlyingRefresh,&FlyingScoreEffect::RefreshScore,void,FlyingScoreEffect* self,int score,int maxScore) {
  FlyingRefresh(self,score,maxScore); UpdateFlyingJudgement(self);
}
MAKE_HOOK_MATCH(FlyingUpdate,&FlyingScoreEffect::ManualUpdate,void,FlyingScoreEffect* self,float t) {
  FlyingUpdate(self,t); UpdateFlyingJudgement(self,t);
}
MAKE_HOOK_MATCH(FlyingFinish,&FlyingScoreSpawner::HandleFlyingObjectEffectDidFinish,void,FlyingScoreSpawner* self,FlyingObjectEffect* effect) {
  auto score=il2cpp_utils::try_cast<FlyingScoreEffect>(effect); if(score) RestoreFlyingJudgement(*score);
  FlyingFinish(self,effect);
}
MAKE_HOOK_MATCH(EffectPools,&EffectPoolsManualInstaller::ManualInstallBindings,void,EffectPoolsManualInstaller* self,Zenject::DiContainer* container,bool shortBeatEffect) {
  EffectPools(self,container,shortBeatEffect);
  if(!getRhythmSaberConfig().Enabled.GetValue()) return;
  auto spawner=container->InstantiateComponentOnNewGameObject<FlyingTextSpawner*>("RhythmSaberFailureTextPool");
  spawner->_duration=.9f; spawner->_xSpread=.2f; spawner->_targetYPos=1.45f; spawner->_targetZPos=2.8f;
  spawner->_fontSize=Defaults::FailureFontSize(ReadSettings().judgementScale); spawner->_shake=false;
  SetFailureSpawner(spawner);
  MetaCore::Engine::SetOnDestroy(spawner,[spawner]{ ClearFailureSpawner(spawner); });
}
// Independent result gate: never write timing results to original personal bests
// or official leaderboards, including when a user disabled the menu toggle later.
MAKE_HOOK_MATCH(ProcessResult,&LevelCompletionResultsHelper::ProcessScore,void,ByRef<BeatmapKey> key,PlayerData* player,PlayerLevelStatsData* stats,
  LevelCompletionResults* results,IReadonlyBeatmapData* beatmap,PlatformLeaderboardsModel* leaderboards) {
  if(SubmissionGuard::Held() || getRhythmSaberConfig().Enabled.GetValue()) { Logger.info("Suppressed original score persistence/submission"); return; }
  ProcessResult(key,player,stats,results,beatmap,leaderboards);
}
MAKE_HOOK_MATCH(PartyLeaderboard,&PartyFreePlayFlowCoordinator::WillScoreGoToLeaderboard,bool,PartyFreePlayFlowCoordinator* self,LevelCompletionResults* results,StringW id,bool practice) {
  if(SubmissionGuard::Held() || getRhythmSaberConfig().Enabled.GetValue()) return false;
  return PartyLeaderboard(self,results,id,practice);
}
MAKE_HOOK_MATCH(FinishLevel,&StandardLevelScenesTransitionSetupDataSO::Finish,void,StandardLevelScenesTransitionSetupDataSO* self,LevelCompletionResults* results) {
  if(Current().active && results) {
    // Separate local record; no original PB or remote submission is touched.
    rapidjson::StringBuffer buffer; rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject(); writer.Key("schema"); writer.Int(2); writer.Key("modVersion"); writer.String(VERSION);
    writer.Key("scorePolicy"); writer.String("configurable-1");
    writer.Key("points"); writer.StartArray(); for(int points:Current().settings.points.points) writer.Int(points); writer.EndArray();
    auto key=self->get_beatmapKey(); auto level=static_cast<std::string>(key.levelId);
    writer.Key("levelId"); writer.String(level.c_str()); writer.Key("difficulty"); writer.Int(static_cast<int>(key.difficulty));
    writer.Key("score"); writer.Int(results->multipliedScore); writer.Key("modifiedScore"); writer.Int(results->modifiedScore);
    writer.Key("maxScore"); writer.Int(Current().maxScore);
    writer.Key("fullCombo"); writer.Bool(results->fullCombo); writer.Key("goodCuts"); writer.Int(results->goodCutsCount);
    writer.Key("badCuts"); writer.Int(results->badCutsCount); writer.Key("misses"); writer.Int(results->missedCount);
    writer.Key("endState"); writer.Int(static_cast<int>(results->levelEndStateType)); writer.Key("endAction"); writer.Int(static_cast<int>(results->levelEndAction));
    writer.Key("lineDistance"); writer.Double(Current().settings.lineDistance); writer.Key("calibrationMs"); writer.Double(Current().settings.offset);
    writer.Key("lineHeight"); writer.Double(Current().settings.lineHeight);
    writer.Key("lineWidth"); writer.Double(Current().settings.lineWidth);
    writer.Key("observedMotion"); writer.Bool(Current().observedMap);
    writer.Key("estimatedCuts"); writer.Uint64(Current().statistics.estimatedCuts);
    writer.Key("unmeasurableCuts"); writer.Uint64(Current().statistics.unmeasurableCuts);
    writer.Key("windowsMs");writer.StartArray();for(double window:Current().settings.windows.ms)writer.Double(window);writer.EndArray();
    writer.Key("counts");writer.StartArray();for(auto count:Current().statistics.counts)writer.Uint64(count);writer.EndArray();
    writer.Key("meanMs"); writer.Double(Current().statistics.Mean()); writer.Key("meanAbsMs"); writer.Double(Current().statistics.MeanAbsolute()); writer.EndObject();
    try {
      const std::filesystem::path directory("/sdcard/ModData/com.beatgames.beatsaber/Mods/RhythmSaber");
      std::filesystem::create_directories(directory);
      std::ofstream output(directory/"results.jsonl",std::ios::app);
      output << buffer.GetString() << '\n';
      if(!output) Logger.error("Unable to save local RhythmSaber result");
    } catch(const std::exception& e) { Logger.error("Local result save failed: {}",e.what()); }
  }
  FinishLevel(self,results);
}
namespace RhythmSaber {
void InstallHooks() {
  INSTALL_HOOK(Logger,CoreInstall); INSTALL_HOOK(Logger,ClockStart); INSTALL_HOOK(Logger,ClockUpdate);
  INSTALL_HOOK(Logger,ClockPause); INSTALL_HOOK(Logger,ClockResume); INSTALL_HOOK(Logger,ClockDestroy);
  INSTALL_HOOK(Logger,ScoreCut); INSTALL_HOOK(Logger,ScoreMiss);
  INSTALL_HOOK(Logger,NoteInit); INSTALL_HOOK(Logger,NoteUpdate);
  INSTALL_HOOK(Logger,BufferRefresh); INSTALL_HOOK(Logger,BufferTotal);
  INSTALL_HOOK(Logger,GoodScore); INSTALL_HOOK(Logger,DespawnScore);
  INSTALL_HOOK(Logger,FlyingInit); INSTALL_HOOK(Logger,FlyingRefresh); INSTALL_HOOK(Logger,FlyingUpdate); INSTALL_HOOK(Logger,FlyingFinish); INSTALL_HOOK(Logger,EffectPools);
  INSTALL_HOOK(Logger,ProcessResult); INSTALL_HOOK(Logger,PartyLeaderboard); INSTALL_HOOK(Logger,FinishLevel);
}
}
