#include "MapCompatibility.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include <cstring>
#include <cmath>
namespace RhythmSaber {
namespace {
bool DecorativeAnimation(const rapidjson::Value& animation) {
  if(!animation.IsObject()) return false;
  // Chroma color animation does not alter a note's crossing of the Z plane.
  for(auto member=animation.MemberBegin();member!=animation.MemberEnd();++member) {
    const std::string_view name(member->name.GetString(),member->name.GetStringLength());
    if(name!="color" && name!="_color") return false;
  }
  return true;
}
bool CustomNoteMotion(const rapidjson::Value& note) {
  if(!note.IsObject()) return false;
  for(const char* container:{"_customData","customData"}) {
    auto found=note.FindMember(container);
    if(found==note.MemberEnd() || !found->value.IsObject()) continue;
    const auto& data=found->value;
    // Noodle/Tracks v2 and v3 properties, not decorative Chroma color data.
    for(const char* key:{"_position","coordinates","_rotation","worldRotation","_localRotation","localRotation","_scale","scale",
        "_track","track","_definitePosition","definitePosition","offsetPosition","offsetWorldRotation","_flip","flip"})
      if(data.HasMember(key)) return true;
    // Constant per-note speed/offset changes still use linear NoteJump Z. Read
    // that note's actual endpoints and durations at the cut, never the map NJS.
    for(const char* key:{"_noteJumpMovementSpeed","noteJumpMovementSpeed","_noteJumpStartBeatOffset","noteJumpStartBeatOffset"}) {
      auto value=data.FindMember(key);
      if(value==data.MemberEnd() || value->value.IsNull()) continue;
      if(!value->value.IsNumber() || !std::isfinite(value->value.GetDouble())) return true;
      if((std::strcmp(key,"_noteJumpMovementSpeed")==0 || std::strcmp(key,"noteJumpMovementSpeed")==0) && value->value.GetDouble()<=0) return true;
    }
    for(const char* key:{"_animation","animation"}) {
      auto animation=data.FindMember(key);
      if(animation!=data.MemberEnd() && !DecorativeAnimation(animation->value)) return true;
    }
    for(const char* key:{"_fake","fake","_disableNoteGravity","disableNoteGravity","uninteractable","disableBadCutDirection","disableBadCutSaberType","disableBadCutSpeed"}) {
      auto flag=data.FindMember(key);
      if(flag!=data.MemberEnd() && (!flag->value.IsBool() || flag->value.GetBool())) return true;
    }
    for(const char* key:{"_interactable","interactable"}) {
      auto flag=data.FindMember(key);
      if(flag!=data.MemberEnd() && (!flag->value.IsBool() || !flag->value.GetBool())) return true;
    }
  }
  return false;
}
bool ChangesPlayerSpace(const rapidjson::Value& root) {
  for(const char* container:{"_customData","customData"}) {
    auto custom=root.FindMember(container);
    if(custom==root.MemberEnd() || !custom->value.IsObject()) continue;
    for(const char* list:{"_customEvents","customEvents"}) {
      auto events=custom->value.FindMember(list);
      if(events==custom->value.MemberEnd() || !events->value.IsArray()) continue;
      for(const auto& event:events->value.GetArray()) {
        if(!event.IsObject()) continue;
        for(const char* key:{"_type","type","t"}) {
          auto type=event.FindMember(key);
          if(type!=event.MemberEnd() && type->value.IsString() && std::strcmp(type->value.GetString(),"AssignPlayerToTrack")==0) return true;
        }
      }
    }
  }
  return false;
}
}
bool Parse(std::string_view json,rapidjson::Document& document) {
  if(json.empty() || json.size()>64u*1024u*1024u) return false;
  // Some editor exports use UTF-8 BOM, comments or trailing commas. Newtonsoft's
  // map reader accepts these; do not reject an otherwise ordinary downloaded map.
  if(json.starts_with("\xEF\xBB\xBF")) json.remove_prefix(3);
  document.Parse<rapidjson::kParseCommentsFlag|rapidjson::kParseTrailingCommasFlag>(json.data(),json.size());
  return !document.HasParseError() && document.IsObject();
}
std::string_view MapConflict(std::string_view json) {
  rapidjson::Document document;
  if(!Parse(json,document)) return "Custom map JSON could not be inspected; observing live movement instead.";
  return {};
}
bool MapNeedsObservedTiming(std::string_view json) {
  rapidjson::Document document;
  if(!Parse(json,document)) return true;
  for(const char* list:{"_notes","colorNotes","colorNotesData","bombNotes","bombNotesData"}) {
    auto notes=document.FindMember(list);
    if(notes==document.MemberEnd()) continue;
    if(!notes->value.IsArray()) return true;
    for(const auto& note:notes->value.GetArray()) if(CustomNoteMotion(note))
      return true;
  }
  return ChangesPlayerSpace(document);
}
}
