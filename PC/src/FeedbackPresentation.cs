using System;
namespace RhythmSaber
{
    public static class FeedbackPresentation
    {
        public static float FailureFontSize(float textSize) => 2.25f*.6f*textSize;
        public static float JudgementOpacity(float progress) =>
            TimingScoring.Finite(progress) ? Math.Max(0, Math.Min(1, (1-progress)*4)) : 1;
    }
}
