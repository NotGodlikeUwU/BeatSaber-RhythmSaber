using System;
using System.Collections.Generic;
using System.IO;
using HarmonyLib;
using UnityEngine;
using Newtonsoft.Json;

namespace RhythmSaber
{
    internal static class Session
    {
        public static bool Active;
        public static string Reason = "";
        public static PluginConfig Settings;
        public static double[] Windows;
        public static int[] Points;
        public static AudioTimeSyncController Clock;
        public static GameplayCoreSceneSetupData Setup;
        public static bool ObservedMap;
        public static readonly long[] Counts = new long[6];
        public static long Measured, Estimated, Unmeasurable;
        public static double SumDelta;
        public static int Revision;
        static readonly Dictionary<NoteController, Motion> motion = new Dictionary<NoteController, Motion>(2048);
        static readonly Dictionary<NoteData, Judgement> results = new Dictionary<NoteData, Judgement>(2048);
        static readonly Queue<NoteData> order = new Queue<NoteData>(2048);
        struct Motion { public NoteData Note; public TrajectoryTiming Timing; }
        static readonly AccessTools.FieldRef<NoteMovement, NoteJump> jumpField = AccessTools.FieldRefAccess<NoteMovement, NoteJump>("_jump");
        static readonly AccessTools.FieldRef<NoteJump, float> halfField = AccessTools.FieldRefAccess<NoteJump, float>("_halfJumpDuration");
        static readonly AccessTools.FieldRef<NoteJump, float> durationField = AccessTools.FieldRefAccess<NoteJump, float>("_jumpDuration");
        static readonly AccessTools.FieldRef<NoteJump, float> timeField = AccessTools.FieldRefAccess<NoteJump, float>("_noteTime");
        static readonly AccessTools.FieldRef<NoteJump, Vector3> startField = AccessTools.FieldRefAccess<NoteJump, Vector3>("_startPos");
        static readonly AccessTools.FieldRef<NoteJump, Vector3> endField = AccessTools.FieldRefAccess<NoteJump, Vector3>("_endPos");
        public static void Begin()
        {
            Active = false; Setup = null; Clock = null;
            Settings = Plugin.Config.Snapshot(); Windows = Settings.Windows(); Points = Settings.Points();
            Reason = Plugin.Failure;
            if (Reason.Length == 0 && !Settings.Valid()) Reason = "Invalid settings: check timing windows, points and beam values.";
            if (Reason.Length == 0) Reason = ScoreSubmissionGuard.Check();
            Active = Settings.Enabled && Reason.Length == 0;
            motion.Clear(); results.Clear(); order.Clear(); Array.Clear(Counts, 0, Counts.Length);
            Measured = Estimated = Unmeasurable = 0; SumDelta = 0; Revision++;
            if (Active) BS_Utils.Gameplay.ScoreSubmission.DisableSubmission("RhythmSaber");
            Plugin.Log.Info(Active ? "RhythmSaber active; submission disabled before gameplay." : "Original scoring: " + Reason);
        }
        public static bool Scored(NoteData data) => data != null && (int)data.scoringType >= 1 && (int)data.scoringType <= 8;
        public static void Reset(NoteController note) { if(note != null) motion.Remove(note); }
        public static void Observe(NoteController note)
        {
            if (!Active || Clock == null || Clock.state != AudioTimeSyncController.State.Playing || !Scored(note.noteData) || Visuals.Plane == null) return;
            if (!motion.TryGetValue(note, out var entry) || !ReferenceEquals(entry.Note, note.noteData)) entry = new Motion { Note = note.noteData };
            entry.Timing.Sample(Clock.songTime, Visuals.Plane.InverseTransformPoint(note.noteTransform.position).z);
            if (motion.Count >= 2048 && !motion.ContainsKey(note)) return;
            motion[note] = entry;
        }
        public static bool Find(NoteData note, out Judgement judgement)
        { judgement = Judgement.Unmeasurable; return note != null && results.TryGetValue(note, out judgement); }
        public static void Cut(NoteController note, in NoteCutInfo info)
        {
            if (!Active || !Scored(info.noteData) || (Clock != null && Clock.state != AudioTimeSyncController.State.Playing)) return;
            if (!info.allIsOK) { Counts[5]++; Revision++; Visuals.Failure(note, "BAD CUT"); return; }
            var judgement = Judgement.Unmeasurable;
            double actual = Clock != null ? Clock.songTime : double.NaN;
            double speed = Clock != null ? Clock.timeScale : double.NaN;
            double expected = double.NaN;
            bool estimated = false;
            if (Clock != null && Visuals.Plane != null && note is INoteMovementProvider provider)
            {
                var movement = provider.noteMovement;
                var jump = movement == null ? null : jumpField(movement);
                double distance = Visuals.Plane.InverseTransformPoint(note.noteTransform.position).z;
                if (jump != null)
                {
                    Vector3 start = startField(jump), end = endField(jump);
                    double duration = durationField(jump), half = halfField(jump), time = timeField(jump);
                    double linearZ = start.z + (end.z-start.z)*(actual-time+half)/duration;
                    if (!ObservedMap && info.noteData.rotation == 0 &&
                        Math.Abs(distance-(movement.localPosition.z-Settings.LineDistance)) < .02 &&
                        Math.Abs(linearZ-movement.localPosition.z) < .02)
                        expected = TimingScoring.Crossing(time, half, duration, start.z, end.z, Settings.LineDistance);
                }
                if (!TimingScoring.Finite(expected))
                {
                    Observe(note);
                    if (motion.TryGetValue(note, out var entry))
                        entry.Timing.Expected(actual, Math.Max(.5, Windows[4]/1000+.1)*speed, out expected, out estimated);
                }
                if (TimingScoring.Finite(expected))
                    judgement = TimingScoring.Judge(TimingScoring.Offset(actual, expected, speed, Settings.TimingOffset),
                        Windows, Points, ScoreModel.GetNoteScoreDefinition(info.noteData.scoringType).maxCutScore, estimated);
            }
            if (!results.ContainsKey(info.noteData))
            {
                if (order.Count >= 2048) results.Remove(order.Dequeue());
                order.Enqueue(info.noteData);
            }
            results[info.noteData] = judgement;
            Counts[(int)judgement.Grade]++; Revision++;
            if (TimingScoring.Finite(judgement.Delta)) { Measured++; SumDelta += judgement.Delta; }
            else Unmeasurable++;
            if (judgement.Estimated) Estimated++;
            if (Settings.Diagnostics) Plugin.Log.Info($"cut type={info.noteData.scoringType} actual={actual:F6} expected={expected:F6} delta={judgement.Delta:F2} score={judgement.Score} estimate={estimated}");
        }
        public static void Miss(NoteController note)
        { if (Active && Scored(note.noteData)) { Counts[5]++; Revision++; Visuals.Failure(note, "MISS"); } }
        public static void Finish(LevelCompletionResults result)
        {
            if (!Active || result == null) return;
            ScoreSubmissionGuard.Mark(result);
            try
            {
                string dir = Path.Combine(IPA.Utilities.UnityGame.UserDataPath, "RhythmSaber");
                Directory.CreateDirectory(dir);
                var row = new {
                    schema = 2, version = "0.1.0-pc", utc = DateTime.UtcNow,
                    levelId = Setup?.beatmapKey.levelId, difficulty = Setup?.beatmapKey.difficulty.ToString(),
                    score = result.multipliedScore, modifiedScore = result.modifiedScore,
                    fullCombo = result.fullCombo, goodCuts = result.goodCutsCount, badCuts = result.badCutsCount,
                    misses = result.missedCount, endState = result.levelEndStateType.ToString(), endAction = result.levelEndAction.ToString(),
                    counts = Counts, meanDelta = Measured > 0 ? (double?)(SumDelta/Measured) : null,
                    estimated = Estimated, unmeasurable = Unmeasurable, settings = Settings
                };
                File.AppendAllText(Path.Combine(dir, "results.jsonl"), JsonConvert.SerializeObject(row) + Environment.NewLine);
            }
            catch(Exception e) { Plugin.Log.Error("Cannot save local RhythmSaber result: " + e); }
        }
    }
}
