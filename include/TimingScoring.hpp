#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

namespace RhythmSaber {
enum class Grade : std::uint8_t { Perfect, Great, Good, Bad, Poor, Miss };
struct Windows { std::array<double, 5> ms{15,30,50,75,100}; bool Valid() const; };
struct PointTable { std::array<int,5> points{115,100,70,10,0}; bool Valid() const; };
struct Judgement { Grade grade; int score; double deltaMs; bool estimated{}; };
Judgement Judge(double deltaMs, const Windows& windows,int maxScore=115,const PointTable& table=PointTable{});
bool TimingScoredType(int scoringType);
std::string_view GradeName(Grade grade);
// z follows the game's linear jump segment; timestamps are song seconds.
std::optional<double> PlaneCrossing(double noteTime, double halfJump, double duration,
                                    double startZ, double endZ, double lineZ);
std::optional<double> OffsetMs(double actualSongTime, double expectedSongTime, double songSpeed, double calibrationMs);
struct Statistics {
  std::array<std::uint64_t,6> counts{};
  std::uint64_t timedCuts{};
  std::uint64_t estimatedCuts{},unmeasurableCuts{};
  double deltaSum{}, absoluteSum{};
  void Add(const Judgement& judgement);
  void AddMiss();
  double Mean() const;
  double MeanAbsolute() const;
  double PerfectPercent() const;
};
}
