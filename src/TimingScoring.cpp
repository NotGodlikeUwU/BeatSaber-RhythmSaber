#include "TimingScoring.hpp"
#include <cmath>

namespace RhythmSaber {
bool Windows::Valid() const {
  double previous=0;
  for (double value : ms) { if (!std::isfinite(value) || value<=previous || value>500) return false; previous=value; }
  return true;
}
bool TimingScoredType(int type) { return type>=1 && type<=8; }
bool PointTable::Valid() const {
  int previous=115;
  for(int value:points) { if(value<0 || value>previous) return false; previous=value; }
  return true;
}
Judgement Judge(double deltaMs, const Windows& windows,int maxScore,const PointTable& table) {
  if (!windows.Valid() || !table.Valid() || !std::isfinite(deltaMs) || maxScore<1 || maxScore>115) return {Grade::Miss,0,deltaMs};
  for (unsigned i=0;i<5;++i) if (std::abs(deltaMs)<=windows.ms[i]) return {static_cast<Grade>(i),static_cast<int>(std::lround(table.points[i]*maxScore/115.0)),deltaMs};
  return {Grade::Miss,0,deltaMs};
}
std::string_view GradeName(Grade grade) {
  constexpr std::string_view names[]{"PERFECT","GREAT","GOOD","BAD","POOR","MISS"};
  return names[static_cast<unsigned>(grade)];
}
std::optional<double> PlaneCrossing(double noteTime,double halfJump,double duration,double startZ,double endZ,double lineZ) {
  if (!std::isfinite(noteTime)||!std::isfinite(halfJump)||!std::isfinite(duration)||!std::isfinite(startZ)||!std::isfinite(endZ)||!std::isfinite(lineZ) || duration<=0 || halfJump<=0 || startZ<=endZ) return {};
  const double fraction=(lineZ-startZ)/(endZ-startZ);
  if (fraction<0 || fraction>1) return {};
  return noteTime-halfJump+fraction*duration;
}
std::optional<double> OffsetMs(double actual,double expected,double speed,double calibration) {
  if (!std::isfinite(actual)||!std::isfinite(expected)||!std::isfinite(speed)||!std::isfinite(calibration)||speed<=0) return {};
  return (actual-expected)*1000.0/speed-calibration;
}
void Statistics::Add(const Judgement& j) { ++counts[static_cast<unsigned>(j.grade)]; if(j.estimated) ++estimatedCuts; if (std::isfinite(j.deltaMs)) { ++timedCuts; deltaSum+=j.deltaMs; absoluteSum+=std::abs(j.deltaMs); } else ++unmeasurableCuts; }
void Statistics::AddMiss() { ++counts[5]; }
double Statistics::Mean() const { return timedCuts ? deltaSum/timedCuts : 0; }
double Statistics::MeanAbsolute() const { return timedCuts ? absoluteSum/timedCuts : 0; }
double Statistics::PerfectPercent() const { std::uint64_t total=0; for(auto count:counts) total+=count; return total ? counts[0]*100.0/total : 0; }
}
