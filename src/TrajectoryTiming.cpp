#include "TrajectoryTiming.hpp"
#include <cmath>
namespace RhythmSaber {
void TrajectoryTiming::Sample(double t,double d) {
  if(!std::isfinite(t)||!std::isfinite(d)) return;
  if(sampled && t<previousTime-1e-5) *this={};
  if(sampled && t>previousTime+1e-6) {
    const double dt=t-previousTime;
    if(dt>.25) { moving=false; crossed=false; }
    else {
      velocity=(d-previousDistance)/dt;
      moving=std::abs(velocity)>.01;
      // Large frame-to-frame jumps cannot prove a continuous crossing.
      if(std::abs(d-previousDistance)<=2 && ((previousDistance>0 && d<=0)||(previousDistance<0 && d>=0))) {
        crossing=previousTime+dt*(-previousDistance)/(d-previousDistance); crossed=true;
      }
    }
  }
  // Duplicate clock samples have no usable dt; retain the last real velocity.
  previousTime=t; previousDistance=d; sampled=true;
}
std::optional<CrossingEstimate> TrajectoryTiming::Expected(double actual,double maxPrediction) const {
  if(!sampled || !std::isfinite(actual) || !std::isfinite(maxPrediction) || maxPrediction<=0) return {};
  std::optional<CrossingEstimate> result;
  if(crossed && actual>=crossing-1e-5 && actual-crossing<=maxPrediction) result=CrossingEstimate{crossing,false};
  if(!moving || std::abs(actual-previousTime)>.05) return result;
  const double dt=-previousDistance/velocity;
  // Predict only a future approach, not a fictional past crossing after a teleport.
  if(dt<0 || dt>maxPrediction) return result;
  if(result && std::abs(actual-result->time)<=std::abs(previousTime+dt-actual)) return result;
  return CrossingEstimate{previousTime+dt,true};
}
}
