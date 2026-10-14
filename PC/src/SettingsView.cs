using BeatSaberMarkupLanguage.Attributes;
namespace RhythmSaber {
public class SettingsView {
[UIValue("status")] public string Status { get { if (Plugin.Failure.Length > 0) return Plugin.Failure; if (!Plugin.Config.Valid()) return "Invalid settings: windows must increase; points must not increase."; var reason = ScoreSubmissionGuard.Check(); return reason.Length > 0 ? reason : "Timing scoring is local only. Changes apply next song."; } }
[UIComponent("status-label")] private TMPro.TextMeshProUGUI statusLabel = null;
[UIAction("#post-parse")] public void RefreshStatus() { if (statusLabel != null) statusLabel.text = Status; }
[UIValue("Enabled")] public bool Enabled { get => Plugin.Config.Enabled; set { Plugin.Config.Enabled = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("ShowLine")] public bool ShowLine { get => Plugin.Config.ShowLine; set { Plugin.Config.ShowLine = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("ShowJudgements")] public bool ShowJudgements { get => Plugin.Config.ShowJudgements; set { Plugin.Config.ShowJudgements = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("ShowOffset")] public bool ShowOffset { get => Plugin.Config.ShowOffset; set { Plugin.Config.ShowOffset = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("ShowStatistics")] public bool ShowStatistics { get => Plugin.Config.ShowStatistics; set { Plugin.Config.ShowStatistics = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("Diagnostics")] public bool Diagnostics { get => Plugin.Config.Diagnostics; set { Plugin.Config.Diagnostics = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("LineDistance")] public float LineDistance { get => Plugin.Config.LineDistance; set { Plugin.Config.LineDistance = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("LineHeight")] public float LineHeight { get => Plugin.Config.LineHeight; set { Plugin.Config.LineHeight = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("LineWidth")] public float LineWidth { get => Plugin.Config.LineWidth; set { Plugin.Config.LineWidth = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("TextSize")] public float TextSize { get => Plugin.Config.TextSize; set { Plugin.Config.TextSize = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("LineThickness")] public float LineThickness { get => Plugin.Config.LineThickness; set { Plugin.Config.LineThickness = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("LineBrightness")] public float LineBrightness { get => Plugin.Config.LineBrightness; set { Plugin.Config.LineBrightness = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("LineOpacity")] public float LineOpacity { get => Plugin.Config.LineOpacity; set { Plugin.Config.LineOpacity = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("LineRed")] public float LineRed { get => Plugin.Config.LineRed; set { Plugin.Config.LineRed = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("LineGreen")] public float LineGreen { get => Plugin.Config.LineGreen; set { Plugin.Config.LineGreen = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("LineBlue")] public float LineBlue { get => Plugin.Config.LineBlue; set { Plugin.Config.LineBlue = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("TimingOffset")] public float TimingOffset { get => Plugin.Config.TimingOffset; set { Plugin.Config.TimingOffset = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("PerfectWindow")] public float PerfectWindow { get => Plugin.Config.PerfectWindow; set { Plugin.Config.PerfectWindow = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("GreatWindow")] public float GreatWindow { get => Plugin.Config.GreatWindow; set { Plugin.Config.GreatWindow = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("GoodWindow")] public float GoodWindow { get => Plugin.Config.GoodWindow; set { Plugin.Config.GoodWindow = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("BadWindow")] public float BadWindow { get => Plugin.Config.BadWindow; set { Plugin.Config.BadWindow = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("PoorWindow")] public float PoorWindow { get => Plugin.Config.PoorWindow; set { Plugin.Config.PoorWindow = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("PerfectPoints")] public int PerfectPoints { get => Plugin.Config.PerfectPoints; set { Plugin.Config.PerfectPoints = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("GreatPoints")] public int GreatPoints { get => Plugin.Config.GreatPoints; set { Plugin.Config.GreatPoints = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("GoodPoints")] public int GoodPoints { get => Plugin.Config.GoodPoints; set { Plugin.Config.GoodPoints = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("BadPoints")] public int BadPoints { get => Plugin.Config.BadPoints; set { Plugin.Config.BadPoints = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIValue("PoorPoints")] public int PoorPoints { get => Plugin.Config.PoorPoints; set { Plugin.Config.PoorPoints = value; Plugin.Config.Changed(); RefreshStatus(); } }
[UIAction("reset")] public void Reset() { Plugin.Config.CopyFrom(new PluginConfig()); Plugin.Config.Changed(); RefreshStatus(); }
}}
