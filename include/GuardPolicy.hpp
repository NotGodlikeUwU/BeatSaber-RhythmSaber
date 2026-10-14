#pragma once
#include <string>
#include <string_view>
namespace RhythmSaber {
// Kept independent of IL2CPP so the conflict matrix can be tested on the host.
std::string_view ModConflict(std::string_view id,std::string_view version,std::string_view path);
std::string_view PlaybackConflict(std::string_view scoreDisabler);
struct GuardState {
  bool requested{},held{};
  bool Disabled() const { return requested||held; }
  void NewLevel() { held=false; }
};
}
