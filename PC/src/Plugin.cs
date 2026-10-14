using System;
using IPA;
using IPA.Config.Stores;
using HarmonyLib;
using SiraUtil.Zenject;
using Zenject;
using BeatSaberMarkupLanguage.Settings;

namespace RhythmSaber
{
    [Plugin(RuntimeOptions.SingleStartInit)]
    public class Plugin
    {
        internal static PluginConfig Config;
        internal static IPA.Logging.Logger Log;
        internal static Harmony Harmony;
        internal static string Failure = "";
        [Init]
        public Plugin(IPA.Logging.Logger logger, IPA.Config.Config config, Zenjector zenjector)
        {
            Log = logger; Config = config.Generated<PluginConfig>();
            zenjector.Install<MenuInstaller>(Location.Menu);
            zenjector.Install<GameInstaller>(Location.StandardPlayer);
        }
        [OnStart]
        public void Start()
        {
            try
            {
                string versionPath = System.IO.Path.Combine(IPA.Utilities.UnityGame.InstallPath, "BeatSaberVersion.txt");
                if (!System.IO.File.ReadAllText(versionPath).Trim().StartsWith("1.40.8_", StringComparison.Ordinal))
                    throw new InvalidOperationException("This build requires PC Beat Saber 1.40.8.");
                System.Runtime.CompilerServices.RuntimeHelpers.RunClassConstructor(typeof(Session).TypeHandle);
                System.Runtime.CompilerServices.RuntimeHelpers.RunClassConstructor(typeof(Visuals).TypeHandle);
                Harmony = new Harmony("RhythmSaber.PC");
                Harmony.PatchAll(typeof(Plugin).Assembly);
                ScoreSubmissionGuard.Install(Harmony);
                Log.Info("RhythmSaber PC 0.1.0 initialized. Settings apply next level.");
            }
            catch(Exception e)
            {
                Failure = e.Message;
                Harmony?.UnpatchSelf();
                Log.Error("RhythmSaber scoring disabled: " + e);
            }
        }
    }
    public class MenuInstaller : Installer
    {
        public override void InstallBindings() => Container.BindInterfacesTo<SettingsRegistration>().AsSingle();
    }
    public class SettingsRegistration : IInitializable, IDisposable
    {
        readonly BSMLSettings settings;
        readonly SettingsView view = new SettingsView();
        public SettingsRegistration(BSMLSettings settings) { this.settings = settings; }
        public void Initialize() => settings.AddSettingsMenu("RhythmSaber", "RhythmSaber.UI.settings.bsml", view);
        public void Dispose() => settings.RemoveSettingsMenu(view);
    }
    public class GameInstaller : Installer
    {
        public override void InstallBindings()
        {
            // InstallBindings runs before gameplay IInitializable / Start callbacks.
            Session.Begin();
            Container.BindInterfacesAndSelfTo<GameSession>().AsSingle();
        }
    }
    public class GameSession : IInitializable, ITickable, IDisposable
    {
        readonly AudioTimeSyncController clock;
        readonly PlayerTransforms player;
        readonly GameplayCoreSceneSetupData setup;
        readonly SiraUtil.Submissions.Submission submission;
        readonly DiContainer container;
        public GameSession(AudioTimeSyncController clock, PlayerTransforms player, GameplayCoreSceneSetupData setup,
            SiraUtil.Submissions.Submission submission, DiContainer container)
        { this.clock = clock; this.player = player; this.setup = setup; this.submission = submission; this.container = container; }
        public void Initialize()
        {
            Session.Clock = clock;
            if (Session.Active)
            {
                submission.DisableScoreSubmission("RhythmSaber", "Timing scoring changes the complete cut score.");
                BS_Utils.Gameplay.ScoreSubmission.DisableSubmission("RhythmSaber");
                Session.Setup = setup;
                Session.ObservedMap = MapCompatibility.NeedsObservedTiming(setup);
            }
            if (Session.Active || (Plugin.Config.Enabled && Session.Reason.Length > 0))
            {
                try { Visuals.Create(player, container); }
                catch(Exception e)
                {
                    // Setup failed before any notes: keep the game usable with original scoring.
                    Session.Active = false; Session.Reason = "Visual setup failed: " + e.Message;
                    Visuals.Destroy(); Plugin.Log.Error("RhythmSaber level disabled: " + e);
                }
            }
        }
        public void Dispose()
        {
            // Keep the result guard latched through all completion callbacks and menu.
            Visuals.Destroy(); Session.Clock = null;
        }
        public void Tick() => Visuals.Tick();
    }
}
