#pragma once
#include "TimingScoring.hpp"
#include <cstdio>
#include <cmath>
namespace RhythmSaber {
inline void FormatJudgement(char* text,size_t capacity,const Judgement& j,bool showOffset) {
  const auto name=GradeName(j.grade);
  char timing[48]{};
  if(std::isfinite(j.deltaMs)) {
    if(showOffset) std::snprintf(timing,sizeof(timing),"%s%+.0f ms",j.estimated?"~":"",j.deltaMs);
    else std::snprintf(timing,sizeof(timing),"%s%s",j.estimated?"~":"",j.deltaMs<-.5?"EARLY":(j.deltaMs>.5?"LATE":"ON TIME"));
  }
  std::snprintf(text,capacity,"<b><size=110%%>%.*s</size></b>\n<size=85%%>%d</size>\n<size=55%%>%s</size>",static_cast<int>(name.size()),name.data(),j.score,timing);
}
}
