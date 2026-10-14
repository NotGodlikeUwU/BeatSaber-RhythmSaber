using System;
using System.Linq;
using Newtonsoft.Json.Linq;

namespace RhythmSaber
{
    internal static class MapCompatibility
    {
        static readonly string[] motionKeys = {
            "animation", "_animation", "track", "_track", "position", "_position", "localRotation", "_localRotation",
            "rotation", "_rotation", "worldRotation", "_worldRotation", "offsetPosition", "offsetWorldRotation",
            "interactable", "_interactable", "fake", "_fake", "disableNoteGravity", "_disableNoteGravity" };
        public static bool NeedsObservedTiming(GameplayCoreSceneSetupData setup)
        {
            try
            {
                // Metadata is used only to select timing mode; no requirement is rejected.
                if (setup.beatmapKey.beatmapCharacteristic.serializedName != "Standard" &&
                    setup.beatmapKey.beatmapCharacteristic.serializedName != "OneSaber" &&
                    setup.beatmapKey.beatmapCharacteristic.serializedName != "NoArrows") return true;
                string raw = setup.beatmapLevelData.GetBeatmapString(in setup.beatmapKey);
                if (string.IsNullOrEmpty(raw)) return true;
                var root = JObject.Parse(raw);
                foreach (var prop in root.Descendants().OfType<JProperty>())
                {
                    if (motionKeys.Contains(prop.Name)) return true;
                    if ((prop.Name == "type" || prop.Name == "_type") && prop.Value.Type == JTokenType.String &&
                        new[] { "AssignPlayerToTrack", "AssignTrackParent", "AnimateTrack", "AssignPathAnimation" }.Contains((string)prop.Value)) return true;
                    if (prop.Name == "njsEvents" && prop.Value.HasValues) return true;
                }
                return false;
            }
            catch(Exception e) { Plugin.Log.Warn("Using observed motion: " + e.Message); return true; }
        }
    }
}
