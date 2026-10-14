# RhythmSaber 0.1 — Beat Saber 1.40.8 for Quest and PC

Rhythm-based timing scores, a white judgement beam and flying hit grades. This repository contains two separate implementations for Beat Saber 1.40.8:

| Platform | Implementation | Install file | Instructions |
|---|---|---|---|
| Meta Quest Standalone | Native C++/IL2CPP, Scotland2 | RhythmSaber-0.1.0.qmod | Quest instructions below |
| PC VR | C#/BSIPA/Harmony | RhythmSaber-PC-0.1.0.zip | [PC README](PC/README.md) |

Download both packages from the [0.1 release](https://github.com/NotGodlikeUwU/RhythmSaber/releases/tag/v0.1). Quest and PC binaries are not interchangeable. The manifests use **0.1.0**, the SemVer representation of release **0.1**. This public release preserves the latest development implementation (Quest 0.2.3 and PC 0.2.4), including the PC settings-page correction; it is not the old initial 0.1.0 prototype.

Earlier gameplay was user-confirmed. The latest display changes, PC settings layout and full online-submission matrix still need fresh live VR validation. Build and host tests do not establish headset compatibility. See [release notes](docs/RELEASE-0.1.md).

The remaining sections document the Quest implementation; PC has its own dependencies and score-submission policy.

## Features

- Replaces the entire swing/after-swing/center scoring formula with musical timing. Ordinary notes retain a maximum of 115 before the native multiplier.
- Displays a colored grade, points and signed offset in the game's pooled FlyingScoreEffect at the cut location. EARLY/LATE can replace milliseconds; optional statistics remain separate.
- Scoring text size is adjustable from 0.25 to 3.0. Default 1.6 is 40% smaller than the 0.2.0 presentation; popups last at least 0.9 seconds. BAD CUT/MISS are half their previous size in 0.2.2. Grades stay opaque for the first 75% of their lifetime, then fade. Original pooled text properties are restored on release.
- One white energy beam with a bright core, soft transparent edges and subtle moving ripples. One mesh/material, no posts, lower rail, collider or expensive bloom. Defaults: distance 1.35 m, height 0.80 m, width 2.8 m, thickness 0.015 m, brightness 1.0, opacity 0.75.
- Normal custom songs and native scoring types 1–8, including Arc/Chain combinations. Each type uses its own ScoreModel maximum; fixedCutScore does not add extra points. Ignore/NoScore and loader-excluded fake objects are not scored.
- Installed Noodle Extensions, Mapping Extensions, Cinema, Vivify and Chroma do not cause a blanket rejection. The mod measures the real NoteController position. Arbitrary animated maps have the measurement limits below.
- Native color, direction, accepted-cut speed, bad cuts, misses, bombs, walls, combo and multipliers remain under game control. A physically valid cut outside the timing window scores zero but preserves physical Combo/Full Combo.

## Timing defaults and settings

| Absolute offset, inclusive upper bound | Grade | Ordinary-note points |
|---|---|---:|
| 0–15 ms | PERFECT | 115 |
| >15–30 ms | GREAT | 100 |
| >30–50 ms | GOOD | 70 |
| >50–75 ms | BAD | 10 |
| >75–100 ms | POOR | 0 |
| >100 ms | MISS | 0 |

All five windows and point values are configurable in Mod Settings and config-utils. Windows must strictly increase. Points must be 0–115 and non-increasing from PERFECT to POOR; equal values are allowed. Special notes receive `round(points * nativeMax / 115)`. Reducing PERFECT points leaves the original maximum and Accuracy denominator unchanged. Point values are recorded in local results.

Defaults enable RhythmSaber, line, judgements and milliseconds; statistics are off. The beam is white. Settings include calibration, beam position/width/thickness/brightness/RGB/alpha and text size. Settings take effect next song or restart. Reset restores defaults; reopen the page to refresh controls.

Existing manual settings are preserved. The one-time 0.2.1 migration changes the previous default distance/height/thickness to the screenshot profile and disables statistics once; custom windows and geometry survive. Old default timing windows were migrated in 0.2.0. Config migrations do not automatically reverse on downgrade.

## Measurement and support limits

For original linear movement, expected time comes from actual NoteJump endpoints/duration and the visible beam plane. For modified movement, world positions are sampled after NoteController.ManualUpdate. Beam and plane follow the gameplay origin parent, including Noodle player tracks, rather than the HMD.

Observed crossings are interpolated between frames. Early hits before an animated note reaches the plane use its latest approach velocity and display `~`, such as `~-24 ms`. This predicts future movement; it cannot know arbitrary future Noodle/Vivify animations. Stops, teleports, insufficient history or unmeasurable crossings yield zero timing points without restoring swing scoring mid-song. Repeated crossings select the nearest observed or predicted passage. Shader-only deformation follows the real collider rather than moved pixels.

Actual time is synchronized `AudioTimeSyncController.songTime` inside `ScoreController.HandleNoteWasCut`, the clock used by native GameNoteController/BurstSliderGameNoteController. Native timeDeviation is logged for comparison; Noodle linked notes may copy another note's timeDeviation, so it is not a universal timestamp. Sub-frame physical time of impact is not reconstructed. Frame uncertainty remains at 72/90/120 Hz and grows with dropped frames.

`deltaMs = (actualSongTime - expectedSongTime) * 1000 / timeScale - calibrationMs`

Negative means EARLY; positive means LATE. Positive calibration subtracts systematic lateness. At zero calibration the ideal plane matches the visible beam. Height/width do not change expected time; distance does. There is no accumulated wall-clock timer, and pause stops judgement updates. Automatic controller/audio calibration is not implemented.

Solo and Party are supported. Multiplayer and mission modes retain original scoring. Every combination of third-party hooks still requires live testing; support for all modcharts with exact timing is not claimed.

## Score submission protection

MetaCore SetScoreSubmission plus native result hooks prevent online submission and changes to original personal bests. **Quest BeatLeader and ScoreSaber must be disabled**: the audited versions have retry/live-stream paths outside the common guard. RhythmSaber refuses altered scoring while either is present. Installed Replay is allowed; active playback does not enable the new formula.

The banner reads `RhythmSaber Active — Online Score Submission Disabled`. Results are separate at:

`/sdcard/ModData/com.beatgames.beatsaber/Mods/RhythmSaber/results.jsonl`

Schema 2 records policy `configurable-1`, points, observedMotion, estimatedCuts and unmeasurableCuts. Submission protection for a modified play remains until the next untouched level, including after exit, restart or disabling the setting. Server records, networking and anti-cheat are not modified.

## Install or update on Quest

1. Close the game. Import `dist/RhythmSaber-0.1.0.qmod` in QuestPatcher, or upload your QMOD in ModsBeforeFriday. Use a modded Quest **1.40.8_7379** with Scotland2. When replacing a development version numbered 0.2.x, remove only the existing RhythmSaber mod first if the installer refuses a lower version; keep its config and dependencies, then import this release.
2. Fully restart Beat Saber. MetaCore 1.6.3 is not downgraded. No runtime dependencies were added in this release.
3. Open **Mod Settings → RhythmSaber** and configure the next song.
4. Test an ordinary map first, then Arc/Chain and animated maps. Confirm visibility, reachability, FPS and coexistence with HitScoreVisualizer in the headset.

Previous artifacts remain in `dist` and `backups/0.1.3-live-confirmed` / `backups/0.2.1-before-feedback-fix`. Import a previous QMOD and restart for rollback; restore migrated config values manually if necessary. Do not install the PC DLL on Quest.

## Build on Windows

Requires PowerShell 7, CMake >=3.22, Ninja, QPM, Android NDK r27d (27.3.13750724). Host tests require Visual Studio C++ Build Tools x64. From the repository root:

```powershell
qpm restore
pwsh scripts/test.ps1
pwsh scripts/build.ps1 -Ndk 'D:/Android/android-ndk-r27d' -Ninja 'D:/Tools/ninja.exe'
pwsh scripts/package.ps1 -SkipBuild
pwsh scripts/source-archive.ps1
```

Pass your own `-Ndk` path or remove local `ndkpath.txt` on another machine; it is excluded from source archives. Exact dependencies: bs-cordl 4008.0.0, beatsaber-hook 6.4.2, custom-types 0.18.4, config-utils 2.0.3, BSML 0.4.55, paper2_scotland2 4.8.0, Scotland2 0.1.7, MetaCore 1.6.3. Do not replace codegen with the latest game version. `qpm.shared.json` locks dependencies. The installer supplies Scotland2; its loader library is not bundled.

For Git schannel `SEC_E_NO_CREDENTIALS`, use a working Git for Windows install and temporarily select `http.sslBackend=openssl` for QPM restore. For git-submodule failures check Git's `usr/bin` availability. Do not copy libraries from a different game version.

## Debugging and verification

```powershell
adb devices -l
pwsh scripts/start-logging.ps1 -Output logs/quest-023-live.txt
adb logcat -b crash -d > logs/quest-023-crash.txt
adb pull /sdcard/ModData/com.beatgames.beatsaber/Mods/RhythmSaber/results.jsonl
adb pull /sdcard/ModData/com.beatgames.beatsaber/Configs/rhythmsaber.json
```

Enable Timing diagnostics. Logs include native scoring type, actual/expected times, signed plane distance, score, estimate flag and frame song-time interval. Debug symbols must match the installed build: `build/librhythmsaber.debug.so`.

See [research/ABI](docs/RESEARCH.md), [gameplay test plan](docs/TESTING.md), [validation history](docs/VALIDATION.md), [0.1 release notes](docs/RELEASE-0.1.md) and the release's `SHA256SUMS.txt`. Historical development notes describe their own versions, including restrictions subsequently removed. The Quest-only source ZIP omits the PC folder; the GitHub repository contains both implementations.
