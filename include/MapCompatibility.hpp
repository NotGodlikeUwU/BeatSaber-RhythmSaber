#pragma once
#include <string_view>
namespace RhythmSaber {
// Inspect serialized note/event data; installed mapping plugins alone are allowed.
std::string_view MapConflict(std::string_view beatmapJson);
bool MapNeedsObservedTiming(std::string_view beatmapJson);
}
