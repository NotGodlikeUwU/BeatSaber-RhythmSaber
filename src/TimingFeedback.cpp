#include "Visuals.hpp"
#include "TimingManager.hpp"
#include "main.hpp"
#include "JudgementText.hpp"
#include "GlobalNamespace/FlyingScoreEffect.hpp"
#include "GlobalNamespace/FlyingTextSpawner.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "UnityEngine/SpriteRenderer.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/LayerMask.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TMP_FontAsset.hpp"
#include "TMPro/TextAlignmentOptions.hpp"
#include "TMPro/TextOverflowModes.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/getters.hpp"
#include <array>
#include <cstdio>

namespace RhythmSaber {
namespace {
UnityEngine::GameObject* root{};
TMPro::TextMeshPro* stats{};
TMPro::TextMeshPro* banner{};
double now{},lastStats{};
GlobalNamespace::FlyingTextSpawner* failureSpawner{};
struct FlyingStyle {
  GlobalNamespace::FlyingScoreEffect* effect{};
  GlobalNamespace::NoteData* note{};
  float fontSize{};
  bool rich{},wrap{},indicator{};
  TMPro::TextOverflowModes overflow{};
  TMPro::TextAlignmentOptions alignment{};
  float progress{},alphaMultiplier{};
  UnityEngine::Color color{};
};
std::array<FlyingStyle,256> flying{};
constexpr UnityEngine::Color gradeColors[]{ {1,.88f,.25f,1},{.3f,1,.4f,1},{.25f,.85f,1,1},{1,.55f,.15f,1},{1,.25f,.25f,1},{1,.12f,.12f,1} };
TMPro::TextMeshPro* Text(const char* name, UnityEngine::Vector3 pos, float size) {
  auto go=UnityEngine::GameObject::New_ctor(name);
  go->get_transform()->SetParent(root->get_transform(),false);
  go->get_transform()->set_localPosition(pos);
  auto text=go->AddComponent<TMPro::TextMeshPro*>();
  text->set_font(BSML::Helpers::GetMainTextFont());
  text->set_fontSize(size); text->set_alignment(TMPro::TextAlignmentOptions::Center); text->set_enableWordWrapping(false);
  text->set_richText(false); text->set_overflowMode(TMPro::TextOverflowModes::Overflow);
  text->get_rectTransform()->set_sizeDelta({25,5});
  text->set_text("");
  return text;
}
}
bool CreateFeedback() {
  DestroyFeedback();
  if(!BSML::Helpers::GetMainTextFont()) { Logger.error("Main TMP font unavailable"); return false; }
  root=UnityEngine::GameObject::New_ctor("RhythmSaberFeedback");
  int layer=UnityEngine::LayerMask::NameToLayer("UI"); if(layer>=0) root->set_layer(layer);
  if(Current().playerSpace) root->get_transform()->SetParent(Current().playerSpace,false);
  root->get_transform()->set_localPosition({0,0,2.8f});
  root->get_transform()->set_localRotation(UnityEngine::Quaternion::get_identity());
  root->get_transform()->set_localScale({.1f,.1f,.1f});
  stats=Text("TimingStatistics",{0,10.2f,0},3.5f);
  banner=Text("SubmissionStatus",{0,8.5f,0},2.8f);
  banner->set_text("RhythmSaber Active — Online Score Submission Disabled");
  banner->set_color({1,.85f,.25f,1});
  now=0; lastStats=-1;
  return true;
}
void DestroyFeedback() {
  if(root && UnityEngine::Object::op_Implicit_bool(root)) UnityEngine::Object::Destroy(root);
  root=nullptr; stats=nullptr; banner=nullptr; flying={};
}
void AttachFlyingJudgement(GlobalNamespace::FlyingScoreEffect* effect,GlobalNamespace::CutScoreBuffer* buffer) {
  if(!Current().active || !Current().settings.showJudgements || !effect || !effect->_text || !buffer || !buffer->_noteCutInfo.noteData) return;
  if(!TimingScoredType(static_cast<int>(buffer->_noteCutInfo.noteData->get_scoringType()))) return;
  for(auto& style:flying) if(!style.effect) {
    auto text=effect->_text;
    style={effect,buffer->_noteCutInfo.noteData,text->get_fontSize(),text->get_richText(),text->get_enableWordWrapping(),
      effect->_maxCutDistanceScoreIndicator?effect->_maxCutDistanceScoreIndicator->get_enabled():false,text->get_overflowMode(),text->get_alignment(),
      0.f,effect->_colorAMultiplier,effect->_color};
    UpdateFlyingJudgement(effect); return;
  }
  Logger.warn("Flying judgement pool tracking capacity exceeded");
}
void UpdateFlyingJudgement(GlobalNamespace::FlyingScoreEffect* effect,float progress) {
  if(!Current().active || !effect || !effect->_text) return;
  for(auto& style:flying) if(style.effect==effect) {
    auto judgement=FindJudgement(style.note); if(!judgement) return;
    char formatted[256]; FormatJudgement(formatted,sizeof(formatted),*judgement,Current().settings.showOffset);
    auto text=effect->_text;
    // Compare UTF-16 to ASCII without allocating strings on every update.
    const std::u16string_view current=static_cast<std::u16string_view>(text->get_text());
    const std::string_view wanted(formatted);
    bool same=current.size()==wanted.size();
    if(same) for(size_t i=0;i<wanted.size();++i) if(current[i]!=static_cast<unsigned char>(wanted[i])) { same=false; break; }
    if(!same) text->set_text(formatted);
    text->set_fontSize(style.fontSize*Defaults::TextScale(Current().settings.judgementScale)*(judgement->grade==Grade::Miss?.5f:1.f));
    text->set_richText(true); text->set_enableWordWrapping(false); text->set_overflowMode(TMPro::TextOverflowModes::Overflow); text->set_alignment(TMPro::TextAlignmentOptions::Center);
    if(progress>=0) style.progress=progress;
    auto color=gradeColors[static_cast<unsigned>(judgement->grade)];
    effect->_colorAMultiplier=1.f;
    effect->_color=color;
    color.a=Defaults::JudgementOpacity(style.progress); text->set_color(color);
    if(effect->_maxCutDistanceScoreIndicator) effect->_maxCutDistanceScoreIndicator->set_enabled(false);
    return;
  }
}
void RestoreFlyingJudgement(GlobalNamespace::FlyingScoreEffect* effect) {
  for(auto& style:flying) if(style.effect==effect) {
    if(effect && effect->_text) {
      auto text=effect->_text; text->set_fontSize(style.fontSize); text->set_richText(style.rich); text->set_enableWordWrapping(style.wrap);
      text->set_overflowMode(style.overflow); text->set_alignment(style.alignment);
      if(effect->_maxCutDistanceScoreIndicator) effect->_maxCutDistanceScoreIndicator->set_enabled(style.indicator);
      effect->_colorAMultiplier=style.alphaMultiplier; effect->_color=style.color;
    }
    style={}; return;
  }
}
void SetFailureSpawner(GlobalNamespace::FlyingTextSpawner* spawner) { failureSpawner=spawner; }
void ClearFailureSpawner(GlobalNamespace::FlyingTextSpawner* spawner) { if(failureSpawner==spawner) failureSpawner=nullptr; }
void ShowCutFailure(const GlobalNamespace::NoteCutInfo& cut) {
  if(!failureSpawner || !Current().settings.showJudgements) return;
  failureSpawner->_color=gradeColors[5];
  failureSpawner->SpawnText(cut.cutPoint,cut.worldRotation,cut.inverseWorldRotation,"BAD CUT");
}
void ShowMissFailure(GlobalNamespace::NoteController* note) {
  if(!failureSpawner || !Current().settings.showJudgements || !note) return;
  failureSpawner->_color=gradeColors[5];
  auto pos=UnityEngine::Quaternion::op_Multiply(note->get_inverseWorldRotation(),note->get_transform()->get_position());
  pos.z=Current().settings.lineDistance;
  failureSpawner->SpawnText(pos,note->get_worldRotation(),note->get_inverseWorldRotation(),"MISS");
}
void UpdateFeedback(double songTime) {
  if(!root) return;
  if(songTime<now-.01) lastStats=-1;
  now=songTime;
  if(lastStats>=0 && now-lastStats<.2) return;
  lastStats=now;
  const auto& s=Current();
  if(!s.settings.showStatistics) { stats->set_text(""); return; }
  char text[256]; const auto& c=s.statistics.counts;
  std::snprintf(text,sizeof(text),"P %llu  GR %llu  G %llu  B %llu  PO %llu  M %llu\nMean %+.1fms | Abs %.1fms | Perfect %.1f%%",
    static_cast<unsigned long long>(c[0]),static_cast<unsigned long long>(c[1]),static_cast<unsigned long long>(c[2]),static_cast<unsigned long long>(c[3]),static_cast<unsigned long long>(c[4]),static_cast<unsigned long long>(c[5]),
    s.statistics.Mean(),s.statistics.MeanAbsolute(),s.statistics.PerfectPercent());
  stats->set_text(text);
}
void ShowBlocked(const char* reason) {
  if(!root && !CreateFeedback()) return;
  banner->set_text("RhythmSaber scoring inactive — original scoring");
  stats->set_text(reason);
}
}
