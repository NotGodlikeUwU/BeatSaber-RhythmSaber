using HarmonyLib;
using System;
using System.Collections.Generic;
using System.Reflection;

namespace RhythmSaber
{
    [HarmonyPatch(typeof(ScoreController), "HandleNoteWasCut")]
    static class CutHook
    {
        [HarmonyPrefix, HarmonyPriority(Priority.First)]
        static void Prefix(NoteController __0, in NoteCutInfo __1) => Session.Cut(__0, in __1);
    }
    [HarmonyPatch(typeof(ScoreController), "HandleNoteWasMissed")]
    static class MissHook { static void Prefix(NoteController __0) => Session.Miss(__0); }
    [HarmonyPatch(typeof(NoteController), "ManualUpdate")]
    static class ObserveHook
    {
        [HarmonyPostfix, HarmonyPriority(Priority.Last)]
        static void Postfix(NoteController __instance) => Session.Observe(__instance);
    }
    [HarmonyPatch(typeof(NoteController), "Init")]
    static class InitNoteHook { static void Prefix(NoteController __instance) => Session.Reset(__instance); }
    [HarmonyPatch(typeof(CutScoreBuffer), "RefreshScores")]
    static class CutScoreHook
    {
        [HarmonyPrefix, HarmonyPriority(Priority.First)]
        static bool Prefix(CutScoreBuffer __instance, ref int ____beforeCutScore, ref int ____afterCutScore, ref int ____centerDistanceCutScore)
        {
            if (!Session.Active || !Session.Scored(__instance.noteCutInfo.noteData)) return true;
            Session.Find(__instance.noteCutInfo.noteData, out var judgement);
            ____beforeCutScore = judgement.Score; ____afterCutScore = 0; ____centerDistanceCutScore = 0;
            return false;
        }
    }
    [HarmonyPatch(typeof(CutScoreBuffer), "get_cutScore")]
    static class TotalCutScoreHook
    {
        [HarmonyPrefix, HarmonyPriority(Priority.First)]
        static bool Prefix(CutScoreBuffer __instance, ref int __result)
        {
            if (!Session.Active || !Session.Scored(__instance.noteCutInfo.noteData)) return true;
            Session.Find(__instance.noteCutInfo.noteData, out var judgement); __result = judgement.Score; return false;
        }
    }
    [HarmonyPatch(typeof(GoodCutScoringElement), "get_cutScore")]
    static class GoodCutScoreHook
    {
        static bool Prefix(GoodCutScoringElement __instance, ref int __result)
        {
            var buffer = __instance.cutScoreBuffer;
            if (!Session.Active || buffer == null || !Session.Scored(buffer.noteCutInfo.noteData)) return true;
            Session.Find(buffer.noteCutInfo.noteData, out var judgement); __result = judgement.Score; return false;
        }
    }
    [HarmonyPatch(typeof(StandardLevelScenesTransitionSetupDataSO), "Finish")]
    static class FinishHook
    {
        [HarmonyPrefix, HarmonyPriority(Priority.First)]
        static void Prefix(LevelCompletionResults __0) => Session.Finish(__0);
    }
    [HarmonyPatch(typeof(LevelCompletionResultsHelper), "ProcessScore")]
    static class LocalScoreHook
    {
        static bool Prefix(LevelCompletionResults __3) => !ScoreSubmissionGuard.IsModified(__3) && !Session.Active;
    }
    [HarmonyPatch(typeof(PartyFreePlayFlowCoordinator), "ProcessScore")]
    static class PartyScoreHook
    {
        static bool Prefix(LevelCompletionResults __0) => !ScoreSubmissionGuard.IsModified(__0) && !Session.Active;
    }
    [HarmonyPatch(typeof(ResultsViewController), "Init")]
    static class ResultsFlagHook
    {
        static void Prefix(LevelCompletionResults __0, ref bool __5)
        { if(ScoreSubmissionGuard.IsModified(__0)) __5 = false; }
    }
    [HarmonyPatch]
    static class OfficialScoreHook
    {
        static IEnumerable<MethodBase> TargetMethods()
        {
            foreach(var m in AccessTools.GetDeclaredMethods(typeof(PlatformLeaderboardsModel)))
                if (m.Name == "UploadScore") yield return m;
        }
        static bool Prefix() => !Session.Active;
    }
    [HarmonyPatch(typeof(FlyingScoreEffect), "InitAndPresent")]
    static class FlyingInitHook
    {
        static void Prefix(FlyingScoreEffect __instance, ref float __1)
        { Visuals.Restore(__instance); if(Session.Active && Session.Settings.ShowJudgements) __1 = Math.Max(__1, .9f); }
        static void Postfix(FlyingScoreEffect __instance) => Visuals.Attach(__instance);
    }
    [HarmonyPatch(typeof(FlyingScoreEffect), "RefreshScore")]
    static class FlyingRefreshHook
    {
        [HarmonyPostfix, HarmonyPriority(Priority.Last)]
        static void Postfix(FlyingScoreEffect __instance) => Visuals.Refresh(__instance);
    }
    [HarmonyPatch(typeof(FlyingScoreEffect), "ManualUpdate")]
    static class FlyingUpdateHook
    {
        [HarmonyPostfix, HarmonyPriority(Priority.Last)]
        static void Postfix(FlyingScoreEffect __instance, float __0) => Visuals.Update(__instance, __0);
    }
    [HarmonyPatch(typeof(FlyingScoreSpawner), "HandleFlyingObjectEffectDidFinish")]
    static class FlyingFinishHook
    { static void Prefix(FlyingObjectEffect __0) { if(__0 is FlyingScoreEffect effect) Visuals.Restore(effect); } }
    // Clear the session latch for game modes that are outside this port's contract.
    [HarmonyPatch]
    static class UnsupportedModeHook
    {
        static IEnumerable<MethodBase> TargetMethods()
        {
            foreach (var name in new[] { "MultiplayerLevelScenesTransitionSetupDataSO", "MissionLevelScenesTransitionSetupDataSO", "TutorialScenesTransitionSetupDataSO" })
            {
                var type = AccessTools.TypeByName(name);
                if (type == null) continue;
                foreach(var method in AccessTools.GetDeclaredMethods(type)) if(method.Name == "Init") yield return method;
            }
        }
        static void Prefix() { Session.Active = false; Session.Clock = null; Visuals.Destroy(); }
    }
}
