#include "TimingManager.hpp"
#include "Visuals.hpp"
#include "ScoreSubmissionGuard.hpp"
#include "MapCompatibility.hpp"
#include "TrajectoryTiming.hpp"
#include "main.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/IBeatmapLevelData.hpp"
#include "GlobalNamespace/FileSystemBeatmapLevelData.hpp"
#include "GlobalNamespace/FileDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/INoteMovementProvider.hpp"
#include "GlobalNamespace/PlayerTransforms.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "GlobalNamespace/NoteMovement.hpp"
#include "GlobalNamespace/NoteJump.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/SliderData.hpp"
#include "GlobalNamespace/NoteJumpSpeedEventData.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/IAudioTimeSource.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"
#include "metacore/shared/game.hpp"
#include <array>
#include <algorithm>
#include <stdexcept>
#include <cmath>

namespace RhythmSaber {
namespace {
Session session;
struct Record { GlobalNamespace::NoteData* note{}; Judgement judgement{}; bool scoring{}; };
std::array<Record,2048> records{};
struct Motion { GlobalNamespace::NoteController* controller{}; GlobalNamespace::NoteData* note{}; TrajectoryTiming timing; };
std::array<Motion,2048> motion{};
size_t next{};
Motion* MotionFor(GlobalNamespace::NoteController* controller) {
  const size_t first=(reinterpret_cast<uintptr_t>(controller)>>4)%motion.size();
  for(size_t i=0;i<motion.size();++i) {
    auto& m=motion[(first+i)%motion.size()];
    if(!m.controller || m.controller==controller) {
      auto note=controller->get_noteData();
      if(m.controller!=controller || m.note!=note) m={controller,note,{}};
      return &m;
    }
  }
  return nullptr;
}
}
Session& Current() { return session; }
void BeginLevel(GlobalNamespace::GameplayCoreSceneSetupData* setup,GlobalNamespace::AudioTimeSyncController* clock) {
  EndVisuals(); session={}; records={}; motion={}; next=0; session.clock=clock;
  SubmissionGuard::NewLevel();
  session.settings=ReadSettings();
  if (!getRhythmSaberConfig().Enabled.GetValue()) return;
  SubmissionGuard::Sync();
  if(!MetaCore::Game::GetScoreSubmissionDisablers().contains(MOD_ID)) { session.reason="Score submission guard could not be confirmed."; return; }
  session.reason=SubmissionGuard::Conflict();
  if (!session.reason.empty()) return;
  if (!session.settings.windows.Valid()) { session.reason="Invalid timing windows: use increasing positive limits."; return; }
  if (!session.settings.points.Valid()) { session.reason="Invalid points: use 0-115 and PERFECT >= GREAT >= GOOD >= BAD >= POOR."; return; }
  if (!setup || !setup->get_transformedBeatmapData()) { session.reason="No transformed beatmap data."; return; }
  auto players=UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::PlayerTransforms*>();
  for(auto player:players) if(player && player->get_gameObject()->get_activeInHierarchy() && player->get_gameObject()->get_scene().get_isLoaded())
    session.playerSpace=player->_originParentTransform;
  // SongCore 1.1.26 creates FileSystemBeatmapLevelData for downloaded v2/v3/v4
  // songs. Recognize that provider too, rather than depending on an ID prefix.
  const auto levelId=static_cast<std::string>(setup->beatmapKey.levelId);
  auto levelData=setup->get_beatmapLevelData();
  auto files=levelData?il2cpp_utils::try_cast<GlobalNamespace::FileSystemBeatmapLevelData>(levelData):std::optional<GlobalNamespace::FileSystemBeatmapLevelData*>{};
  if(levelId.starts_with("custom_level_") || files.has_value()) {
    try {
      if(!levelData) throw std::runtime_error("Custom map data unavailable");
      auto key=setup->beatmapKey;
      // Resolve the selected difficulty through the native file provider. No
      // guessed folder names, hash->path assumptions or extra SongCore linkage.
      auto difficulty=files?(*files)->GetDifficultyBeatmap(byref(key)):nullptr;
      auto json=difficulty?difficulty->GetBeatmapString():levelData->GetBeatmapString(byref(key));
      if(!json) throw std::runtime_error("Serialized custom map data unavailable");
      auto raw=static_cast<std::string>(json);
      session.observedMap=MapNeedsObservedTiming(raw);
      Logger.info("Custom song inspected: {} difficulty={} serializedBytes={}",levelId,static_cast<int>(key.difficulty),raw.size());
    } catch(const std::exception& error) {
      session.observedMap=true;
      Logger.warn("Custom map metadata unavailable; using observed world movement: {}",error.what());
    }
  }
  auto characteristic=setup->beatmapKey.beatmapCharacteristic;
  if(characteristic && (characteristic->get_containsRotationEvents() || characteristic->get_serializedName()!=std::string("Standard"))) session.observedMap=true;
  auto data=setup->get_transformedBeatmapData();
  auto normalDefinition=GlobalNamespace::ScoreModel::GetNoteScoreDefinition(GlobalNamespace::NoteData::ScoringType::Normal);
  if(!normalDefinition || normalDefinition->get_maxCutScore()!=115 || normalDefinition->fixedCutScore!=0) { session.reason="Another mod changed the normal-note score definition."; return; }
  auto items=data->get_allBeatmapDataItems();
  if(!items) { session.reason="Beatmap could not be inspected."; return; }
  for(auto node=items->get_First();node;node=node->get_Next()) {
    auto item=node->get_Value();
    if (il2cpp_utils::try_cast<GlobalNamespace::NoteJumpSpeedEventData>(item).has_value()) session.observedMap=true;
    auto note=il2cpp_utils::try_cast<GlobalNamespace::NoteData>(item);
    if(note && (*note)->get_gameplayType()!=GlobalNamespace::NoteData::GameplayType::Bomb) {
      if((*note)->get_rotation()!=0) session.observedMap=true;
    }
  }
  session.maxScore=GlobalNamespace::ScoreModel::ComputeMaxMultipliedScoreForBeatmap(data);
  session.active=true;
  SubmissionGuard::HoldForLevel();
  if (!CreateJudgementLine() || !CreateFeedback()) { AbortScoring("Required visual resources unavailable."); return; }
  Logger.info("Level armed: beam z={} m height={} m, observedMotion={}, calibration={} ms; online submission disabled",session.settings.lineDistance,session.settings.lineHeight,session.observedMap,session.settings.offset);
}
void EndVisuals() { DestroyJudgementLine(); DestroyFeedback(); }
void AbortScoring(const char* reason) {
  // After any changed cut, reverting mid-song would mix scoring systems. Keep
  // timing scoring active but award zero for unavailable measurements.
  session.reason=reason;
  if(!session.touched) session.active=false;
  Logger.error("RhythmSaber: {}",reason);
}
void ResetNoteMotion(GlobalNamespace::NoteController* controller) {
  if(controller) if(auto m=MotionFor(controller)) m->timing={};
}
void ObserveNote(GlobalNamespace::NoteController* controller) {
  if(!session.active || session.paused || !session.clock || !controller || !controller->get_noteData()) return;
  if(!TimingScoredType(static_cast<int>(controller->get_noteData()->get_scoringType()))) return;
  if(auto m=MotionFor(controller)) m->timing.Sample(session.clock->get_songTime(),SignedPlaneDistance(controller->get_transform()->get_position()));
}
void CaptureCut(GlobalNamespace::NoteController* controller,const GlobalNamespace::NoteCutInfo& cut) {
  if(!session.active || session.paused || !cut.noteData) return;
  if(!TimingScoredType(static_cast<int>(cut.noteData->get_scoringType()))) return;
  if(!const_cast<GlobalNamespace::NoteCutInfo&>(cut).get_allIsOK()) {
    if(session.settings.diagnostics) Logger.info("native bad cut: note={} speedOK={} directionOK={} saberTypeOK={} tooSoon={}",cut.noteData->get_time(),cut.speedOK,cut.directionOK,cut.saberTypeOK,cut.wasCutTooSoon);
    session.statistics.AddMiss(); ShowCutFailure(cut); return;
  }
  SubmissionGuard::HoldForLevel(); session.touched=true;
  Judgement judgement{Grade::Miss,0,NAN};
  auto provider=il2cpp_utils::try_cast<GlobalNamespace::INoteMovementProvider>(controller);
  if(provider && session.clock) {
    auto movement=(*provider)->get_noteMovement();
    auto jump=movement?movement->_jump:nullptr;
    auto clock=session.clock;
    if (jump && clock) {
      const double speed=clock->get_timeScale();
      const double actual=clock->get_songTime();
      const double distance=SignedPlaneDistance(controller->get_transform()->get_position());
      std::optional<double> expected;
      bool estimated=false;
      // Analytic timing is exact for the original straight trajectory. Animated
      // or rotated maps use real world positions, not nominal note timestamps.
      const double linearZ=jump->_startPos.z+(jump->_endPos.z-jump->_startPos.z)*(actual-jump->_noteTime+jump->_halfJumpDuration)/jump->_jumpDuration;
      if(!session.observedMap && cut.noteData->get_rotation()==0 && std::abs(distance-(movement->get_localPosition().z-session.settings.lineDistance))<.02 && std::abs(linearZ-movement->get_localPosition().z)<.02)
        expected=PlaneCrossing(jump->_noteTime,jump->_halfJumpDuration,jump->_jumpDuration,jump->_startPos.z,jump->_endPos.z,session.settings.lineDistance);
      if(!expected) if(auto m=MotionFor(controller)) {
        m->timing.Sample(actual,distance);
        auto crossing=m->timing.Expected(actual,std::max(.5,session.settings.windows.ms.back()/1000.0+.1)*speed);
        if(crossing) { expected=crossing->time; estimated=crossing->estimated; }
      }
      if(expected) {
        auto offset=OffsetMs(actual,*expected,speed,session.settings.offset);
        auto definition=GlobalNamespace::ScoreModel::GetNoteScoreDefinition(cut.noteData->get_scoringType());
        if(offset && definition) judgement=Judge(*offset,session.settings.windows,definition->get_maxCutScore(),session.settings.points);
        judgement.estimated=estimated;
        if(session.settings.diagnostics) Logger.info("cut note={} type={} actual={} expected={} speed={} planeDistance={} delta={} score={} estimated={} frameSongDelta={} nativeCutSongTime={}",
          jump->_noteTime,static_cast<int>(cut.noteData->get_scoringType()),actual,*expected,speed,distance,judgement.deltaMs,judgement.score,estimated,clock->get_lastFrameDeltaSongTime(),cut.noteData->get_time()-cut.timeDeviation);
      } else if(session.settings.diagnostics) Logger.warn("No measurable beam crossing for this cut; awarding 0 without changing native cut rules.");
    }
  }
  // A fixed buffer covers concurrent swing completions without a per-hit allocation.
  size_t slot=next++%records.size();
  if(records[slot].scoring) Logger.error("Timing record capacity exceeded; old measurement will yield zero.");
  records[slot]={cut.noteData,judgement,true};
  session.statistics.Add(judgement);
}
void CaptureMiss(GlobalNamespace::NoteController* controller) {
  if(!session.active || !controller || !TimingScoredType(static_cast<int>(controller->get_noteData()->get_scoringType()))) return;
  session.statistics.AddMiss(); ShowMissFailure(controller);
}
std::optional<Judgement> FindJudgement(GlobalNamespace::NoteData* note) {
  for(auto& record:records) if(record.note==note && note) return record.judgement;
  return {};
}
// Retain the immutable display measurement after the scoring element finishes.
// Chain-link buffers can finish before their FlyingScoreEffect's first update.
void ForgetJudgement(GlobalNamespace::NoteData* note) { for(auto& r:records) if(r.note==note) r.scoring=false; }
void TickFeedback(GlobalNamespace::AudioTimeSyncController* clock) {
  if(session.active && !session.paused) { UpdateFeedback(clock->get_songTime()); UpdateJudgementLine(clock->get_songTime()); }
}
}
