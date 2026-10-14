#include "RhythmSaberConfig.hpp"
#include "JudgementBeam.hpp"
#include <algorithm>
#include <cmath>
namespace RhythmSaber {
Settings ReadSettings() {
  auto& c=getRhythmSaberConfig();
  auto safe=[](float v,float lo,float hi,float def) { return std::isfinite(v)?std::clamp(v,lo,hi):def; };
  return {c.ShowLine.GetValue(),c.ShowJudgements.GetValue(),c.ShowOffset.GetValue(),c.ShowStatistics.GetValue(),c.Diagnostics.GetValue(),
    safe(c.LineDistance.GetValue(),0.f,2.5f,Defaults::Distance),safe(c.LineHeight.GetValue(),.5f,2.f,Defaults::Height),safe(c.LineWidth.GetValue(),1.f,20.f,Defaults::Width),safe(c.LineThickness.GetValue(),.005f,.1f,Defaults::Thickness),safe(c.LineBrightness.GetValue(),0.f,3.f,Defaults::Brightness),
    safe(c.LineRed.GetValue(),0.f,1.f,1.f),safe(c.LineGreen.GetValue(),0.f,1.f,1.f),safe(c.LineBlue.GetValue(),0.f,1.f,1.f),safe(c.LineAlpha.GetValue(),0.f,1.f,Defaults::Opacity),safe(c.JudgementScale.GetValue(),.25f,3.f,Defaults::TextSize),
    safe(c.TimingOffset.GetValue(),-200.f,200.f,0.f),{{c.PerfectWindow.GetValue(),c.GreatWindow.GetValue(),c.GoodWindow.GetValue(),c.BadWindow.GetValue(),c.PoorWindow.GetValue()}},
    {{c.PerfectPoints.GetValue(),c.GreatPoints.GetValue(),c.GoodPoints.GetValue(),c.BadPoints.GetValue(),c.PoorPoints.GetValue()}}};
}
void MigrateSettings() {
  auto& c=getRhythmSaberConfig();
  if(c.LineLayoutVersion.GetValue()<1) {
    c.LineDistance.SetValue(MigrateLineDistance(c.LineDistance.GetValue(),c.LineLayoutVersion.GetValue()));
    c.LineLayoutVersion.SetValue(1);
  }
  if(c.LineLayoutVersion.GetValue()<2) {
    c.LineAlpha.SetValue(.75f); c.LineRed.SetValue(1); c.LineGreen.SetValue(1); c.LineBlue.SetValue(1);
    c.LineLayoutVersion.SetValue(2);
  }
  if(c.LineLayoutVersion.GetValue()<3) {
    c.LineDistance.SetValue(Defaults::ReplaceOldDefault(c.LineDistance.GetValue(),1.5f,Defaults::Distance));
    c.LineHeight.SetValue(Defaults::ReplaceOldDefault(c.LineHeight.GetValue(),1.35f,Defaults::Height));
    c.LineThickness.SetValue(Defaults::ReplaceOldDefault(c.LineThickness.GetValue(),.025f,Defaults::Thickness));
    c.ShowStatistics.SetValue(Defaults::ShowStatistics);
    c.LineLayoutVersion.SetValue(3);
  }
  if(c.ScorePolicyVersion.GetValue()<1) {
    if(c.PerfectWindow.GetValue()==20 && c.GreatWindow.GetValue()==40 && c.GoodWindow.GetValue()==70 && c.BadWindow.GetValue()==110 && c.PoorWindow.GetValue()==150) {
      c.PerfectWindow.SetValue(15); c.GreatWindow.SetValue(30); c.GoodWindow.SetValue(50); c.BadWindow.SetValue(75); c.PoorWindow.SetValue(100);
    }
    c.ScorePolicyVersion.SetValue(1);
  }
}
void ResetSettings() {
  auto& c=getRhythmSaberConfig();
  c.Enabled.SetValue(Defaults::Enabled); c.ShowLine.SetValue(true); c.ShowJudgements.SetValue(true); c.ShowOffset.SetValue(true); c.ShowStatistics.SetValue(Defaults::ShowStatistics);
  c.LineDistance.SetValue(Defaults::Distance); c.LineHeight.SetValue(Defaults::Height); c.LineWidth.SetValue(Defaults::Width); c.LineLayoutVersion.SetValue(3); c.LineThickness.SetValue(Defaults::Thickness); c.LineBrightness.SetValue(Defaults::Brightness); c.LineRed.SetValue(1); c.LineGreen.SetValue(1); c.LineBlue.SetValue(1); c.LineAlpha.SetValue(Defaults::Opacity); c.JudgementScale.SetValue(Defaults::TextSize);
  c.PerfectPoints.SetValue(115); c.GreatPoints.SetValue(100); c.GoodPoints.SetValue(70); c.BadPoints.SetValue(10); c.PoorPoints.SetValue(0);
  c.TimingOffset.SetValue(0); c.PerfectWindow.SetValue(15); c.GreatWindow.SetValue(30); c.GoodWindow.SetValue(50); c.BadWindow.SetValue(75); c.PoorWindow.SetValue(100); c.ScorePolicyVersion.SetValue(1); c.Diagnostics.SetValue(false);
}
}
