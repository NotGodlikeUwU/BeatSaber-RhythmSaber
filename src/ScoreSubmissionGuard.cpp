#include "ScoreSubmissionGuard.hpp"
#include "RhythmSaberConfig.hpp"
#include "main.hpp"
#include "metacore/shared/game.hpp"
#include "GuardPolicy.hpp"

namespace RhythmSaber::SubmissionGuard {
namespace { GuardState state; }
std::string Conflict() {
  auto mods=modloader_get_loaded();
  std::string conflict;
  for(size_t i=0;i<mods.size;++i) {
    auto& mod=mods.array[i];
    std::string id=mod.info.id?mod.info.id:"";
    std::string version=mod.info.version?mod.info.version:"";
    std::string path=mod.path?mod.path:"";
    auto reason=ModConflict(id,version,path);
    if(!reason.empty()) { if(!conflict.empty()) conflict+='\n'; conflict+=reason; }
  }
  modloader_free_results(&mods);
  for(const auto& disabler:MetaCore::Game::GetScoreSubmissionDisablers()) {
    auto reason=PlaybackConflict(disabler);
    if(!reason.empty()) { if(!conflict.empty()) conflict+='\n'; conflict+=reason; }
  }
  return conflict;
}
bool SetRequested(bool enabled) {
  if (enabled) {
    auto conflict=Conflict();
    auto settings=ReadSettings();
    if (!conflict.empty() || !settings.windows.Valid() || !settings.points.Valid()) {
      Logger.warn("Activation refused: {}",conflict.empty()?"Invalid timing windows or points":conflict);
      getRhythmSaberConfig().Enabled.SetValue(false); Sync(); return false;
    }
  }
  getRhythmSaberConfig().Enabled.SetValue(enabled); Sync(); return true;
}
void Sync() {
  // Keep the guard through results/replay processing. A new untouched gameplay
  // session is the release boundary; never release on OnDestroy/scene-ended.
  state.requested=getRhythmSaberConfig().Enabled.GetValue();
  MetaCore::Game::SetScoreSubmission(MOD_ID,!state.Disabled());
}
void HoldForLevel() { state.held=true; Sync(); }
void NewLevel() { state.NewLevel(); Sync(); }
bool Held() { return state.held; }
}
