#include "GuardPolicy.hpp"
#include <algorithm>
#include <cctype>
namespace RhythmSaber {
std::string_view ModConflict(std::string_view modId,std::string_view version,std::string_view modPath) {
  std::string id(modId),path(modPath);
  auto lower=[](unsigned char c) { return static_cast<char>(std::tolower(c)); };
  std::transform(id.begin(),id.end(),id.begin(),lower);std::transform(path.begin(),path.end(),path.begin(),lower);
  if(id.find("beatleader")!=std::string::npos || path.find("libbl.so")!=std::string::npos)
    return "Disable BeatLeader before enabling RhythmSaber (unprotected replay retry).";
  if(id.find("scoresaber")!=std::string::npos || path.find("scoresaber")!=std::string::npos)
    return "Disable ScoreSaber before enabling RhythmSaber (unguarded live replay streaming).";
  if(id=="metacore" && version!="1.6.2" && version!="1.6.3")
    return "RhythmSaber requires MetaCore 1.6.2 or 1.6.3 for Quest 1.40.8.";
  return {};
}
std::string_view PlaybackConflict(std::string_view scoreDisabler) {
  std::string id(scoreDisabler);
  std::transform(id.begin(),id.end(),id.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
  // Replay 0.5.2 sets its own MetaCore disabler in StartReplay and removes it
  // in StopReplay. Merely loading the plugin is not playback.
  return id=="replay" ? "Replay playback is active; timing scoring stays off for this replay." : std::string_view{};
}
}
