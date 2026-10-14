#include "main.hpp"
#include "TimingManager.hpp"
#include "ScoreSubmissionGuard.hpp"
#include "UnityEngine/Application.hpp"
static modloader::ModInfo modInfo{MOD_ID,VERSION,0};
MOD_EXTERN_FUNC void setup(CModInfo* info) {
  info->id=MOD_ID; info->version=VERSION; info->version_long=0;
  getRhythmSaberConfig().Init(modInfo);
  RhythmSaber::MigrateSettings();
}
MOD_EXTERN_FUNC void late_load() {
  il2cpp_functions::Init();
  auto version=static_cast<std::string>(UnityEngine::Application::get_version());
  if(version!="1.40.8" && version!="1.40.8_7379") {
    Logger.error("Unsupported Beat Saber version: {}; scoring hooks not installed",version);
    getRhythmSaberConfig().Enabled.SetValue(false); return;
  }
  RhythmSaber::SubmissionGuard::SetRequested(getRhythmSaberConfig().Enabled.GetValue());
  RhythmSaber::InstallHooks();
  RhythmSaber::RegisterSettings();
  Logger.info("RhythmSaber {} loaded for Quest {}. Experimental; headset validation pending.",VERSION,version);
}
