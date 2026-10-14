using System;
using System.Linq;
using System.Reflection;
using System.IO;
using System.Security.Cryptography;
using System.Runtime.CompilerServices;
using HarmonyLib;
using IPA.Loader;

namespace RhythmSaber
{
    internal static class ScoreSubmissionGuard
    {
        // Unknown binary versions are deliberately not trusted just because method names match.
        const string BeatLeaderHash = "D55139BA64625F93C0A9E029B9D288738ECF9F5F04F24B7BAD4E7EEA82DCCA1A";
        const string ScoreSaberHash = "4822E3392A95C6152521C1C90EF09148CFA10D233AC7A30A7C8BCA368F63AA4A";
        static readonly ConditionalWeakTable<LevelCompletionResults, object> modified = new ConditionalWeakTable<LevelCompletionResults, object>();
        static readonly ConditionalWeakTable<object, object> modifiedReplays = new ConditionalWeakTable<object, object>();
        static bool installed;
        public static void Mark(LevelCompletionResults result)
        { if(result != null && Session.Active && !modified.TryGetValue(result, out _)) modified.Add(result, new object()); }
        public static bool IsModified(LevelCompletionResults result) => result != null && modified.TryGetValue(result, out _);
        static string Hash(string path)
        { using(var sha = SHA256.Create()) using(var stream = File.OpenRead(path)) return BitConverter.ToString(sha.ComputeHash(stream)).Replace("-", ""); }
        public static string Check()
        {
            if (!installed) return "Submission guard did not initialize.";
            try
            {
                foreach (var id in new[] { "BeatLeader", "ScoreSaber" })
                {
                    var meta = PluginManager.GetPluginFromId(id);
                    if (meta == null) continue;
                    string expected = id == "BeatLeader" ? BeatLeaderHash : ScoreSaberHash;
                    if (meta.Assembly == null || Hash(meta.File.FullName) != expected)
                        return "Disable incompatible " + id + ": its installed binary has not been audited.";
                }
                return "";
            }
            catch(Exception e) { return "Unable to verify submission safety: " + e.Message; }
        }
        public static void Install(Harmony harmony)
        {
            PatchOptional(harmony, "BeatLeader", "BeatLeader.ReplayRecorder", "FinalizeReplay", nameof(TagBeatLeaderReplay), typeof(LevelCompletionResults));
            var leader = PluginManager.GetPluginFromId("BeatLeader");
            if (leader?.Assembly != null && Hash(leader.File.FullName) == BeatLeaderHash)
            {
                var replayType = leader.Assembly.GetType("BeatLeader.Models.Replay.Replay", true);
                var endType = leader.Assembly.GetType("BeatLeader.Models.PlayEndData", true);
                PatchOptional(harmony, "BeatLeader", "BeatLeader.Utils.ScoreUtil", "ProcessReplay", nameof(BlockBeatLeaderReplay), replayType, endType);
                PatchOptional(harmony, "BeatLeader", "BeatLeader.Utils.ScoreUtil", "UploadReplay", nameof(BlockBeatLeaderReplay), replayType);
                PatchOptional(harmony, "BeatLeader", "BeatLeader.Utils.ScoreUtil", "UploadPlay", nameof(BlockBeatLeaderReplay), replayType, endType);
            }
            // Five is the verified synchronous entry before ScoreSaber's async upload pipeline.
            PatchOptional(harmony, "ScoreSaber", "ScoreSaber.Core.Daemons.UploadDaemon", "Five", nameof(BlockModifiedResult),
                typeof(string), typeof(BeatmapLevel), typeof(BeatmapKey), typeof(LevelCompletionResults), typeof(bool));
            installed = true;
        }
        static void PatchOptional(Harmony harmony, string id, string type, string name, string prefix, params Type[] args)
        {
            var meta = PluginManager.GetPluginFromId(id);
            if (meta == null) return;
            // A mismatched binary will fail Check and never receive timing scores.
            if (meta.Assembly == null || Hash(meta.File.FullName) != (id == "BeatLeader" ? BeatLeaderHash : ScoreSaberHash)) return;
            var method = AccessTools.Method(meta.Assembly.GetType(type), name, args);
            if (method == null) throw new MissingMethodException(type, name);
            harmony.Patch(method, prefix: new HarmonyMethod(typeof(ScoreSubmissionGuard), prefix) { priority = Priority.First });
            Plugin.Log.Info("Audited submission guard: " + id + " " + name);
        }
        static bool BlockModifiedResult(object[] __args)
        {
            var result = __args.OfType<LevelCompletionResults>().FirstOrDefault();
            bool blocked = Session.Active || IsModified(result);
            if (blocked) Plugin.Log.Info("RhythmSaber prevented ranked replay creation / score upload.");
            return !blocked;
        }
        static void TagBeatLeaderReplay(object __instance, LevelCompletionResults __0)
        {
            if (!Session.Active && !IsModified(__0)) return;
            object replay = AccessTools.Field(__instance.GetType(), "_replay").GetValue(__instance);
            if (replay != null && !modifiedReplays.TryGetValue(replay, out _)) modifiedReplays.Add(replay, new object());
            // Allow recorder finalization/cleanup; ProcessReplay owns both saving and uploading.
        }
        static bool BlockBeatLeaderReplay(object __0)
        {
            bool blocked = Session.Active || (__0 != null && modifiedReplays.TryGetValue(__0, out _));
            if (blocked) Plugin.Log.Info("RhythmSaber prevented BeatLeader replay saving / submission / retry.");
            return !blocked;
        }
    }
}
