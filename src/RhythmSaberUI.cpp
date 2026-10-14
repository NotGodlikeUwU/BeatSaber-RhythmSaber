#include "RhythmSaberConfig.hpp"
#include "ScoreSubmissionGuard.hpp"
#include "TimingManager.hpp"
#include "bsml/shared/BSML.hpp"
#include "HMUI/ViewController.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Color.hpp"
#include "custom-types/shared/register.hpp"
#include <memory>
#include <cmath>

namespace RhythmSaber {
void RegisterSettings() {
  custom_types::Register::AutoRegister(); BSML::Init();
  BSML::Register::RegisterSettingsMenu("RhythmSaber",[](HMUI::ViewController* self,bool first,bool,bool) {
    // Rebuild on each activation so reset, conflicts and persisted values agree.
    auto transform=self->get_transform();
    if(!first) { for(int i=transform->get_childCount()-1;i>=0;--i) UnityEngine::Object::Destroy(transform->GetChild(i)->get_gameObject()); }
    auto parent=BSML::Lite::CreateScrollableSettingsContainer(transform)->get_transform();
    auto status=BSML::Lite::CreateText(parent,"",2.6f);
    auto refresh=[status] {
      auto conflict=SubmissionGuard::Conflict();
      if(conflict.empty() && !ReadSettings().points.Valid()) conflict="Points must be 0-115: PERFECT >= GREAT >= GOOD >= BAD >= POOR.";
      status->set_text(!conflict.empty()?conflict:(getRhythmSaberConfig().Enabled.GetValue()?"RhythmSaber Active — Online Score Submission Disabled":"RhythmSaber off — original scoring"));
    };
    refresh();
    auto enable=std::make_shared<BSML::ToggleSetting*>(nullptr);
    *enable=BSML::Lite::CreateToggle(parent,"Enable RhythmSaber",getRhythmSaberConfig().Enabled.GetValue(),[refresh,enable](bool v){
      if(!SubmissionGuard::SetRequested(v) && *enable) (*enable)->set_Value(false);
      refresh();
    });
    BSML::Lite::CreateText(parent,"OST/custom songs, Arc/Chain and mapping mods. Changes apply next song.",2.5f);
#define TOGGLE(field,label) BSML::Lite::CreateToggle(parent,label,getRhythmSaberConfig().field.GetValue(),[](bool v){getRhythmSaberConfig().field.SetValue(v);})
    TOGGLE(ShowLine,"Show Judgement Line"); TOGGLE(ShowJudgements,"Show Timing Judgements"); TOGGLE(ShowOffset,"Show milliseconds (off: EARLY/LATE)"); TOGGLE(ShowStatistics,"Show Timing Statistics");
#undef TOGGLE
#define NUMBER(field,label,precision,step,lo,hi) BSML::Lite::CreateIncrementSetting(parent,label,precision,step,getRhythmSaberConfig().field.GetValue(),lo,hi,[](float v){getRhythmSaberConfig().field.SetValue(v);})
    NUMBER(LineDistance,"Beam distance (m)",2,.05f,0.f,2.5f);
    NUMBER(LineHeight,"Beam height (m)",2,.05f,.5f,2.f);
    NUMBER(LineWidth,"Beam width (m)",1,.1f,1.f,20.f);
    NUMBER(JudgementScale,"Scoring text size",2,.1f,.25f,3.f);
    NUMBER(LineThickness,"Line thickness (m)",3,.005f,.005f,.1f);
    NUMBER(LineBrightness,"Line brightness",1,.1f,0.f,3.f);
    NUMBER(LineAlpha,"Line opacity",2,.05f,0.f,1.f);
    auto s=ReadSettings();
    BSML::Lite::CreateColorPicker(parent,"Line color",{s.red,s.green,s.blue,1},[](UnityEngine::Color color){ auto& c=getRhythmSaberConfig(); c.LineRed.SetValue(color.r);c.LineGreen.SetValue(color.g);c.LineBlue.SetValue(color.b); });
    NUMBER(TimingOffset,"Calibration offset (ms)",0,1.f,-200.f,200.f);
    BSML::Lite::CreateText(parent,"Positive calibration subtracts lateness. Windows must increase.",2.3f);
    BSML::Lite::CreateText(parent,"~ means predicted animated crossing. Points: 0-115, descending by grade.",2.3f);
    NUMBER(PerfectWindow,"PERFECT window (ms)",0,1.f,1.f,500.f); NUMBER(GreatWindow,"GREAT window (ms)",0,1.f,2.f,500.f);
    NUMBER(GoodWindow,"GOOD window (ms)",0,1.f,3.f,500.f); NUMBER(BadWindow,"BAD window (ms)",0,1.f,4.f,500.f); NUMBER(PoorWindow,"POOR window (ms)",0,1.f,5.f,500.f);
#undef NUMBER
#define POINTS(field,label) BSML::Lite::CreateIncrementSetting(parent,label,0,1.f,static_cast<float>(getRhythmSaberConfig().field.GetValue()),0.f,115.f,[refresh](float v){getRhythmSaberConfig().field.SetValue(static_cast<int>(std::lround(v)));refresh();})
    POINTS(PerfectPoints,"PERFECT points"); POINTS(GreatPoints,"GREAT points"); POINTS(GoodPoints,"GOOD points"); POINTS(BadPoints,"BAD points"); POINTS(PoorPoints,"POOR points");
#undef POINTS
    BSML::Lite::CreateToggle(parent,"Timing diagnostics (logcat)",getRhythmSaberConfig().Diagnostics.GetValue(),[](bool v){getRhythmSaberConfig().Diagnostics.SetValue(v);});
    BSML::Lite::CreateUIButton(parent,"Reset to defaults",[refresh]{ ResetSettings(); SubmissionGuard::SetRequested(getRhythmSaberConfig().Enabled.GetValue()); refresh(); });
    BSML::Lite::CreateText(parent,"After Reset, reopen this page to refresh controls.",2.3f);
    if(!Current().reason.empty()) BSML::Lite::CreateText(parent,Current().reason,2.3f);
  },false);
}
}
