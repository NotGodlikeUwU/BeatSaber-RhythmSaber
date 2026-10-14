#pragma once
#include "RhythmSaberConfig.hpp"
#include <optional>
#include <string>
namespace GlobalNamespace { class NoteController; class NoteData; class GameplayCoreSceneSetupData; struct NoteCutInfo; class AudioTimeSyncController; }
namespace UnityEngine { class Transform; }
namespace RhythmSaber {
struct Session {
  bool active{},paused{},touched{},observedMap{};
  GlobalNamespace::AudioTimeSyncController* clock{};
  UnityEngine::Transform* playerSpace{};
  int maxScore{};
  Settings settings;
  Statistics statistics;
  std::string reason;
};
Session& Current();
void BeginLevel(GlobalNamespace::GameplayCoreSceneSetupData* data,GlobalNamespace::AudioTimeSyncController* clock);
void ObserveNote(GlobalNamespace::NoteController* note);
void ResetNoteMotion(GlobalNamespace::NoteController* note);
void EndVisuals();
void CaptureCut(GlobalNamespace::NoteController* note, const GlobalNamespace::NoteCutInfo& cut);
void CaptureMiss(GlobalNamespace::NoteController* note);
std::optional<Judgement> FindJudgement(GlobalNamespace::NoteData* note);
void ForgetJudgement(GlobalNamespace::NoteData* note);
void AbortScoring(const char* reason);
void TickFeedback(GlobalNamespace::AudioTimeSyncController* clock);
void InstallHooks();
}
