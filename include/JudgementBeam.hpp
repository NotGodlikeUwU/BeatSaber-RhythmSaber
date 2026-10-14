#pragma once
#include <array>
#include <cmath>
namespace RhythmSaber {
struct BeamPoint { float x,y,alpha; };
inline BeamPoint BeamVertex(int column,int row,float width,float height,float thickness,double time) {
  constexpr std::array<float,7> alpha{0,.025f,.22f,1,.22f,.025f,0};
  const float x=width*(column/40.f-.5f);
  const float wave=static_cast<float>(thickness*(.12f*std::sin(x*18-time*6)+.06f*std::sin(x*31+time*7)));
  return {x,height+(row-3)*thickness+wave,alpha[row]};
}
inline float MigrateLineDistance(float distance,int layoutVersion) {
  return layoutVersion<1 && std::isfinite(distance) && std::abs(distance-.5f)<.0001f ? 1.5f : distance;
}
}
