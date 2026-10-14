#pragma once
#include <optional>
namespace RhythmSaber {
struct CrossingEstimate { double time; bool estimated; };
// Samples are signed distances from the visible plane in current player space.
// Confirmed crossings interpolate between observed frames. Early cuts can only
// extrapolate the latest velocity; arbitrary future animation is not observable.
class TrajectoryTiming {
  double previousTime{},previousDistance{},velocity{},crossing{};
  bool sampled{},moving{},crossed{};
public:
  void Sample(double songTime,double signedDistance);
  std::optional<CrossingEstimate> Expected(double actual,double maxPrediction) const;
};
}
