#pragma once
#include "config-utils/shared/config-utils.hpp"
#include "TimingScoring.hpp"
#include "Defaults.hpp"
DECLARE_CONFIG(RhythmSaberConfig) {
  CONFIG_VALUE(Enabled, bool, "Enable RhythmSaber", RhythmSaber::Defaults::Enabled);
  CONFIG_VALUE(ShowLine, bool, "Show Judgement Line", true);
  CONFIG_VALUE(ShowJudgements, bool, "Show Timing Judgements", true);
  CONFIG_VALUE(ShowOffset, bool, "Show Timing Offset", true);
  CONFIG_VALUE(ShowStatistics, bool, "Show Timing Statistics", RhythmSaber::Defaults::ShowStatistics);
  CONFIG_VALUE(LineDistance, float, "Line Distance", RhythmSaber::Defaults::Distance);
  CONFIG_VALUE(LineHeight, float, "Line Height", RhythmSaber::Defaults::Height);
  CONFIG_VALUE(LineWidth, float, "Line Width", 2.8f);
  CONFIG_VALUE(LineLayoutVersion, int, "Line Layout Version", 0);
  CONFIG_VALUE(LineThickness, float, "Line Thickness", RhythmSaber::Defaults::Thickness);
  CONFIG_VALUE(LineBrightness, float, "Line Brightness", 1.0f);
  CONFIG_VALUE(LineRed, float, "Line Red", 1.0f);
  CONFIG_VALUE(LineGreen, float, "Line Green", 1.0f);
  CONFIG_VALUE(LineBlue, float, "Line Blue", 1.0f);
  CONFIG_VALUE(LineAlpha, float, "Line Alpha", 0.75f);
  CONFIG_VALUE(JudgementScale, float, "Judgement Scale", 1.6f);
  CONFIG_VALUE(ScorePolicyVersion, int, "Score Policy Version", 0);
  CONFIG_VALUE(PerfectPoints, int, "PERFECT points", 115);
  CONFIG_VALUE(GreatPoints, int, "GREAT points", 100);
  CONFIG_VALUE(GoodPoints, int, "GOOD points", 70);
  CONFIG_VALUE(BadPoints, int, "BAD points", 10);
  CONFIG_VALUE(PoorPoints, int, "POOR points", 0);
  CONFIG_VALUE(TimingOffset, float, "Timing Offset ms", 0.0f);
  CONFIG_VALUE(PerfectWindow, float, "PERFECT ms", 15.0f);
  CONFIG_VALUE(GreatWindow, float, "GREAT ms", 30.0f);
  CONFIG_VALUE(GoodWindow, float, "GOOD ms", 50.0f);
  CONFIG_VALUE(BadWindow, float, "BAD ms", 75.0f);
  CONFIG_VALUE(PoorWindow, float, "POOR ms", 100.0f);
  CONFIG_VALUE(Diagnostics, bool, "Timing Diagnostics", false);
};
namespace RhythmSaber {
struct Settings {
  bool showLine{},showJudgements{},showOffset{},showStatistics{},diagnostics{};
  float lineDistance{},lineHeight{},lineWidth{},lineThickness{},brightness{},red{},green{},blue{},alpha{},judgementScale{},offset{};
  Windows windows;
  PointTable points;
};
Settings ReadSettings();
void MigrateSettings();
void ResetSettings();
void RegisterSettings();
}
