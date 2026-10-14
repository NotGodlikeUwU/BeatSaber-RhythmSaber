using System;

namespace RhythmSaber
{
    public class PluginConfig
    {
        public virtual bool Enabled { get; set; } = true;
        public virtual bool ShowLine { get; set; } = true;
        public virtual bool ShowJudgements { get; set; } = true;
        public virtual bool ShowOffset { get; set; } = true;
        public virtual bool ShowStatistics { get; set; } = false;
        public virtual bool Diagnostics { get; set; } = false;
        public virtual float LineDistance { get; set; } = 1.35f;
        public virtual float LineHeight { get; set; } = .80f;
        public virtual float LineWidth { get; set; } = 2.8f;
        public virtual float TextSize { get; set; } = 1.6f;
        public virtual float LineThickness { get; set; } = .015f;
        public virtual float LineBrightness { get; set; } = 1f;
        public virtual float LineOpacity { get; set; } = .75f;
        public virtual float LineRed { get; set; } = 1f;
        public virtual float LineGreen { get; set; } = 1f;
        public virtual float LineBlue { get; set; } = 1f;
        public virtual float TimingOffset { get; set; } = 0f;
        public virtual float PerfectWindow { get; set; } = 15f;
        public virtual float GreatWindow { get; set; } = 30f;
        public virtual float GoodWindow { get; set; } = 50f;
        public virtual float BadWindow { get; set; } = 75f;
        public virtual float PoorWindow { get; set; } = 100f;
        public virtual int PerfectPoints { get; set; } = 115;
        public virtual int GreatPoints { get; set; } = 100;
        public virtual int GoodPoints { get; set; } = 70;
        public virtual int BadPoints { get; set; } = 10;
        public virtual int PoorPoints { get; set; } = 0;
        public virtual void Changed() { }
        public virtual void OnReload() { }
        public virtual void CopyFrom(PluginConfig other) { }
        public double[] Windows() => new double[] { PerfectWindow, GreatWindow, GoodWindow, BadWindow, PoorWindow };
        public int[] Points() => new int[] { PerfectPoints, GreatPoints, GoodPoints, BadPoints, PoorPoints };
        public PluginConfig Snapshot()
        {
            // Copy values, not the generated store's locks/events.
            var copy = new PluginConfig();
            foreach (var p in typeof(PluginConfig).GetProperties()) p.SetValue(copy, p.GetValue(this));
            return copy;
        }
        public bool Valid()
        {
            if (!TimingScoring.Valid(Windows(), Points())) return false;
            foreach(var p in typeof(PluginConfig).GetProperties())
                if (p.PropertyType == typeof(float) && !TimingScoring.Finite((float)p.GetValue(this))) return false;
            return LineDistance >= -.5 && LineDistance <= 5 && LineHeight >= .1 && LineHeight <= 3 &&
                LineWidth >= .5 && LineWidth <= 10 && TextSize >= .25 && TextSize <= 3 &&
                LineThickness >= .001 && LineThickness <= .1 && LineBrightness >= 0 && LineBrightness <= 3 &&
                LineOpacity >= 0 && LineOpacity <= 1 && LineRed >= 0 && LineRed <= 1 &&
                LineGreen >= 0 && LineGreen <= 1 && LineBlue >= 0 && LineBlue <= 1 && Math.Abs(TimingOffset) <= 500;
        }
    }
}
