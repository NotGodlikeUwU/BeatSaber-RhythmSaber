using System;

namespace RhythmSaber
{
    public enum Grade { Perfect, Great, Good, Bad, Poor, Miss }
    public readonly struct Judgement
    {
        public readonly Grade Grade;
        public readonly int Score;
        public readonly double Delta;
        public readonly bool Estimated;
        public Judgement(Grade grade, int score, double delta, bool estimated = false)
        { Grade = grade; Score = score; Delta = delta; Estimated = estimated; }
        public static Judgement Unmeasurable => new Judgement(Grade.Miss, 0, double.NaN);
    }
    public static class TimingScoring
    {
        public static bool Finite(double x) => !double.IsNaN(x) && !double.IsInfinity(x);
        public static bool Valid(double[] windows, int[] points)
        {
            if (windows == null || points == null || windows.Length != 5 || points.Length != 5) return false;
            for (int i = 0; i < 5; ++i)
                if (!Finite(windows[i]) || windows[i] <= 0 || (i > 0 && windows[i] <= windows[i-1]) ||
                    points[i] < 0 || points[i] > 115 || (i > 0 && points[i] > points[i-1])) return false;
            return true;
        }
        public static double Offset(double actual, double expected, double speed, double calibration)
            => Finite(speed) && speed > 0 ? (actual - expected) * 1000 / speed - calibration : double.NaN;
        public static double Crossing(double time, double half, double duration, double startZ, double endZ, double planeZ)
        {
            if (!Finite(time + half + duration + startZ + endZ + planeZ) || duration <= 0 || Math.Abs(endZ-startZ) < 1e-6) return double.NaN;
            double fraction = (planeZ-startZ)/(endZ-startZ);
            return fraction >= 0 && fraction <= 1 ? time-half+fraction*duration : double.NaN;
        }
        public static Judgement Judge(double delta, double[] windows, int[] points, int nativeMax, bool estimated = false)
        {
            if (!Finite(delta) || !Valid(windows, points) || nativeMax <= 0 || nativeMax > 115) return Judgement.Unmeasurable;
            int i = 0; while (i < 5 && Math.Abs(delta) > windows[i]) ++i;
            int score = i == 5 ? 0 : (int)Math.Round(points[i] * nativeMax / 115.0, MidpointRounding.AwayFromZero);
            return new Judgement((Grade)i, score, delta, estimated);
        }
    }
    public struct TrajectoryTiming
    {
        double previousTime, previousDistance, velocity, crossing;
        bool sampled, moving, crossed;
        public void Sample(double t, double d)
        {
            if (!TimingScoring.Finite(t) || !TimingScoring.Finite(d)) return;
            if (sampled && t < previousTime-1e-5) this = default;
            if (sampled && t > previousTime+1e-6)
            {
                double dt = t-previousTime;
                if (dt > .25) { moving = false; crossed = false; }
                else
                {
                    velocity = (d-previousDistance)/dt; moving = Math.Abs(velocity) > .01;
                    if (Math.Abs(d-previousDistance) <= 2 && ((previousDistance > 0 && d <= 0) || (previousDistance < 0 && d >= 0)))
                    { crossing = previousTime + dt*(-previousDistance)/(d-previousDistance); crossed = true; }
                }
            }
            previousTime = t; previousDistance = d; sampled = true;
        }
        public bool Expected(double actual, double maxPrediction, out double time, out bool estimated)
        {
            time = double.NaN; estimated = false;
            if (!sampled || !TimingScoring.Finite(actual) || !TimingScoring.Finite(maxPrediction) || maxPrediction <= 0) return false;
            bool found = crossed && actual >= crossing-1e-5 && actual-crossing <= maxPrediction;
            if (found) time = crossing;
            if (!moving || Math.Abs(actual-previousTime) > .05) return found;
            double dt = -previousDistance/velocity;
            if (dt < 0 || dt > maxPrediction) return found;
            double prediction = previousTime+dt;
            if (!found || Math.Abs(actual-time) > Math.Abs(prediction-actual)) { time = prediction; estimated = true; }
            return true;
        }
    }
}
