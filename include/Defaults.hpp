#pragma once
#include <algorithm>
#include <cmath>
namespace RhythmSaber::Defaults {
inline constexpr bool Enabled=true,ShowStatistics=false;
inline constexpr float Distance=1.35f,Height=.80f,Width=2.8f,TextSize=1.6f,Thickness=.015f,Brightness=1.f,Opacity=.75f;
inline float ReplaceOldDefault(float value,float oldValue,float newValue) {
  return std::isfinite(value) && std::abs(value-oldValue)<.0001f ? newValue : value;
}
inline float TextScale(float size) {
  return .6f*(std::isfinite(size)?std::clamp(size,.25f,3.f):TextSize);
}
inline float FailureFontSize(float size) { return 2.25f*TextScale(size); }
// Hold the whole judgement opaque; fade only at the end of its lifetime.
inline float JudgementOpacity(float progress) {
  return std::isfinite(progress)?std::clamp((1.f-progress)*4.f,0.f,1.f):1.f;
}
}
