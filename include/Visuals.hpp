#pragma once
#include "TimingScoring.hpp"
namespace GlobalNamespace { class FlyingScoreEffect; class CutScoreBuffer; class NoteController; class FlyingTextSpawner; struct NoteCutInfo; }
namespace UnityEngine { struct Vector3; }
namespace RhythmSaber {
bool CreateJudgementLine();
void DestroyJudgementLine();
void SetVisualsPaused(bool paused);
void UpdateJudgementLine(double songTime);
double SignedPlaneDistance(UnityEngine::Vector3 position);
bool CreateFeedback();
void DestroyFeedback();
void UpdateFeedback(double songTime);
void ShowBlocked(const char* reason);
void AttachFlyingJudgement(GlobalNamespace::FlyingScoreEffect* effect,GlobalNamespace::CutScoreBuffer* buffer);
void UpdateFlyingJudgement(GlobalNamespace::FlyingScoreEffect* effect,float progress=-1.f);
void RestoreFlyingJudgement(GlobalNamespace::FlyingScoreEffect* effect);
void ShowCutFailure(const GlobalNamespace::NoteCutInfo& cut);
void ShowMissFailure(GlobalNamespace::NoteController* note);
void SetFailureSpawner(GlobalNamespace::FlyingTextSpawner* spawner);
void ClearFailureSpawner(GlobalNamespace::FlyingTextSpawner* spawner);
}
