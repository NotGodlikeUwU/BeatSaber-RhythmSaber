# RhythmSaber PC 0.1 — Beat Saber 1.40.8

Rhythm-based timing scores, a white judgement beam and flying hit grades. Online score submission is disabled during RhythmSaber play.

This C#/BSIPA/Harmony port targets the managed PC build **1.40.8_7379**. The native Quest project is separate; QMOD, Scotland2 and Android NDK are not used on PC. Public release **0.1** uses **0.1.0** in SemVer metadata and preserves the latest development build, including the 0.2.4 settings-page correction. This is a version reset, not a rollback of features. Download from the [0.1 release](https://github.com/NotGodlikeUwU/RhythmSaber/releases/tag/v0.1).

The previous 0.2.2/0.2.3 layout stretched the tab buttons while collapsing the settings content. In 0.2.4, a 7-unit selector row sits above separate pages that stretch across the native 110-unit settings panel and fill its remaining height. A fixed header and reset footer reserve 22 vertical units; no VerticalLayoutGroup sizes the tabs. Grades remain opaque for the first 75% of their lifetime and then fade; BAD CUT/MISS retain their smaller size. Timing, points and submission protection are unchanged.

## Installation and BSManager

1. Close Beat Saber. Select a modded **1.40.8** instance in BSManager and install the dependencies below.
2. Import **RhythmSaber-PC-0.1.0.zip** using the instance's **Mods → three-dot menu → Import mods**, or extract it beside `Beat Saber.exe`. Replace the old RhythmSaber DLL; do not keep duplicate/pending copies. The DLL must end up at `Plugins\RhythmSaber.dll`; PDB is optional. Import may stage files under `Plugins\Pending` until game launch.
3. Launch that instance with your existing VR/OpenXR launch settings.
4. Open **Settings → Mod Settings → RhythmSaber**. BSIPA stores settings at `UserData\RhythmSaber.json`.
5. Start Solo or Party. Active play displays `RhythmSaber Active — Online Score Submission Disabled`.

**Catalog visibility:** BSManager 1.6.0 identifies installed mods through BeatMods file hashes. Importing an unpublished DLL does not guarantee an entry in its installed-mod list, even though BSIPA loads the mod. After the exact release is accepted into BeatMods for 1.40.8, refresh the Mods tab to obtain its name and English description. This package does not modify BSManager or fabricate catalog registration. See [BeatMods publication instructions](docs/BEATMODS.md).

The ZIP contains RhythmSaber and documentation, with no game or dependency DLLs. QuestPatcher is not needed. The build script does not install into your game. Use your own 1.40.8 instance path.

| Dependency | Audited installed version | Manifest requirement |
|---|---|---|
| BSIPA | 4.3.6 | ^4.3.4 |
| BeatSaberMarkupLanguage | 1.12.5+bs.1.40.0 | ^1.12.5 |
| SiraUtil | 3.2.1+bs.1.40.0 | ^3.1.11 |
| BS_Utils, mod ID `BS Utils` | 1.14.2 | ^1.14.2 |

## Scoring and defaults

The full swing, follow-through, center and special-note fixed bonus formula is replaced. Ordinary notes retain a maximum of 115 before the native multiplier. The game still validates color, direction and cut speed, and handles physical misses, bad cuts, bombs and walls. Native maximum score and multipliers remain; Accuracy uses the new earned points.

A physically correct cut outside the window scores zero but **does not become a physical miss or break Combo/Full Combo**. Timing MISS and a native missed note have different causes, preserving Quest behavior.

| Absolute offset, inclusive upper bound | Grade | Ordinary-note points |
|---|---|---:|
| 0–15 ms | PERFECT | 115 |
| >15–30 ms | GREAT | 100 |
| >30–50 ms | GOOD | 70 |
| >50–75 ms | BAD | 10 |
| >75–100 ms | POOR | 0 |
| >100 ms | MISS | 0 |

Windows must strictly increase. Points must be 0–115 and non-increasing from PERFECT to POOR. Invalid config prevents altered scoring until corrected. Reducing PERFECT points does not reduce the original Accuracy denominator.

Default white beam: distance 1.35 m, height 0.80 m, width 2.8 m, core thickness 0.015 m, brightness 1, opacity 0.75. One mesh with a bright core, soft edges and subtle ripple; no posts, collider or expensive bloom. The scoring plane uses the same transform, even when line rendering is hidden.

Scoring text size defaults to 1.6. Grade, points and offset appear in `FlyingScoreEffect` at the native score location. Disabling milliseconds shows EARLY/LATE. Statistics default to off. Settings are fixed at level start; disabling Enable RhythmSaber restores original scoring next song/restart. To uninstall, close the game and remove `Plugins\RhythmSaber.dll` and optional PDB, including pending copies if present.

## Custom maps, Arcs and Chains

Custom song IDs and Noodle Extensions, Mapping Extensions, Cinema/Theater, Vivify or Chroma requirements do not cause a blanket rejection. Install the mods required by the map; RhythmSaber does not replace their loader, animation or video systems.

All eight scored types from Normal to ChainLinkArcHead use `ScoreModel.GetNoteScoreDefinition` and `round(points * nativeMaximum / 115)`. A link whose native maximum is 20 does not receive 115. Combined Arc/Chain types are included. Ignore/NoScore and the continuous visual Arc do not become additional scored blocks.

Linear expected time comes from `NoteJump`, checked against the real position. Animated, rotated or dynamic-NJS maps use observed note-center positions relative to the visible plane. Parsed v2/v3/v4 JSON selects the measurement method rather than denying the map.

**Measurement limit:** future arbitrary animation cannot be known from past frames. Early animated hits use approach velocity and display `~`, such as `~-24 ms`. Observed crossings are interpolated. Teleports, stops or unmeasurable movement yield zero and `NO CROSSING`, without falling back to swing points. Exact timing of every future Noodle animation is not claimed.

Multiplayer, Campaign and Tutorial retain original scoring. Third-party replay playback and alternate scoring systems are not validated; disable RhythmSaber for these.

## Score submission protection

Before game Start/cut callbacks, `BS_Utils.Gameplay.ScoreSubmission.DisableSubmission("RhythmSaber")` runs; a native SiraUtil submission ticket is created during player initialization. Additional hooks:

- `StandardLevelScenesTransitionSetupDataSO.Finish` tags altered results before completion handlers run.
- `LevelCompletionResultsHelper.ProcessScore` and Party `ProcessScore` prevent original personal-best changes. `PlatformLeaderboardsModel.UploadScore` is blocked for the active session.
- ScoreSaber `UploadDaemon.Five` prevents the altered result from entering its asynchronous upload pipeline.
- BeatLeader `FinalizeReplay` tags a replay while preserving recorder finalization. `ScoreUtil.ProcessReplay` skips saving/uploading it. `UploadReplay` and `UploadPlay` also reject the tagged object on retries. Tags remain with objects after disabling RhythmSaber or starting a new song.

These are local guards before network code. Server records, network requests and anti-cheat are unchanged. RhythmSaber plays do not create BeatLeader replay files; local results go to `UserData\RhythmSaber\results.jsonl`.

Signatures and call paths were audited for these **specific DLLs**:

| Mod | Version | SHA-256 |
|---|---|---|
| BeatLeader | 0.9.33 | `D55139BA64625F93C0A9E029B9D288738ECF9F5F04F24B7BAD4E7EEA82DCCA1A` |
| ScoreSaber | 3.3.17 | `4822E3392A95C6152521C1C90EF09148CFA10D233AC7A30A7C8BCA368F63AA4A` |

These versions may remain installed for testing this PC port. Unknown DLLs **prevent altered scoring** and display a reason: disable the incompatible version or audit it first. Hashes are checked at level start. Absent ranking mods need no hooks. Other mods with their own leaderboard/export systems are not automatically covered.

**Live verification of blocked submission is still required.** Static auditing and managed hook tests do not replace a real song/network test. Use `TESTING.md` in the install ZIP or [docs/TESTING.md](docs/TESTING.md) in source.

## Build on Windows

Requires a .NET SDK supporting C# 9+, .NET Framework 4.7.2 Developer Pack, and modded PC Beat Saber 1.40.8 with the dependencies above. SDK 5.0.214 and local reference assemblies were used. The main project needs no NuGet packages. Game/dependency references use `Private=false` and are not distributed.

From the PC source directory:

```powershell
.\build.ps1 -BeatSaberDir 'D:\Games\Beat Saber\1.40.8'
```

This builds DLL, math tests, ABI checks and an isolated managed scoring harness, then creates install/source ZIPs and the BeatMods submission kit under `dist`. It does not install the DLL or publish anything. DLL output: `bin\Release\net472\RhythmSaber.dll`.

Use `-Dotnet 'C:\Program Files\dotnet\dotnet.exe'` to select the SDK. `-SkipHarness` is a skipped check, not a passed check. SDK 8+ may require the installed 4.7.2 Developer Pack because automatic reference-assembly downloading is disabled.

`src/SettingsView.cs` and `UI/settings.bsml` are included. Regenerate with Python `tools/GenerateSettings.py` after changing the settings list.

## Debugging and validation

Enable **Diagnostic logging**, then start a new song. Search BSIPA's `Logs\_latest.log` / `Logs` directory for `RhythmSaber`. Logs include actual, expected, delta, scoring type, points and estimate flag; each blocked ranking path logs its own entry. PC uses BSIPA/Unity logs rather than ADB/logcat.

The 0.2.2 validation baseline: Release build with zero errors/warnings; 429 independent timing/config/trajectory/feedback checks; 112 ABI/resource checks (count depends on optional ranking DLLs); 482 real managed scoring getter checks through Harmony. Tests cover eight note types, removal of geometric/fixed additions and original scoring restoration.

The harness runs in a separate CLR process without Unity, VR, collisions, BSML or networking. Engine hooks use Unity ECall and are checked by ABI rather than ordinary-CLR runtime execution. RefreshScores prefixes run directly; cutScore getters are detoured through Harmony. The user confirmed earlier PC gameplay and supplied the failing 0.2.3 menu screenshot. The new 0.2.4 layout and online-submission matrix still need fresh live VR validation. See [0.2.4 layout investigation](docs/RELEASE-0.2.4.md).

## Architecture and timing

Plugin/Sira installers handle lifecycle; PluginConfig/SettingsView provide config and BSML; Session reads song time and records measurements/results; TimingScoring provides math; MapCompatibility selects trajectories; Visuals owns beam/pooled text/statistics; ScoreSubmissionGuard blocks local submission paths; Hooks intercept the necessary game methods.

Actual time is `AudioTimeSyncController.songTime` in the cut callback used by GameNoteController/BurstSliderGameNoteController. Position interpolation does not produce a missing sub-frame saber timestamp. No trustworthy finer physical impact timestamp was found in this callback. Tracking/audio latency uses manual Timing Offset; no automatic calibration or accumulated Windows timer is used.

Primary references: [BSMG PC BSML](https://bsmg.wiki/modding/pc/bsml.html), [BS_Utils](https://github.com/Kylemc1413/Beat-Saber-Utils), [BeatLeader PC](https://github.com/BeatLeader/beatleader-mod), [HitScoreVisualizer PC](https://github.com/ErisApps/HitScoreVisualizer). Hooks were checked against installed 1.40.8 assemblies rather than newer game APIs.
