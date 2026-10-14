# RhythmSaber 0.1 — Osu-like scoring system mod for Beat Saber 1.40.8 Quest/PC

WARNING: This mod does NOT WORK with ScoreSaber/BeatLeader installed. Please disable these mods to play RhythmSaber

An experimental mod that changes Beat Saber's scoring logic to OSU/DJMax-like timing scores based on music rhythm, a white judgement beam and flying hit grades. This repository contains two separate version of this mod, both PC and Quest versions.


## Features

- Replaces the entire swing/after-swing/center scoring formula with musical timing. Ordinary notes retain a maximum of 115 before the native multiplier.
- Fully configurable scores and timings.
- Displays a colored grade, points and signed offset in the game's pooled FlyingScoreEffect at the cut location. EARLY/LATE can replace milliseconds; optional statistics remain separate.
- One white judgemental line (fully configurable position, size and color)
- Noodle Extensions, Mapping Extensions, Cinema and Chroma support. Vivify is not tested.
- Native color, direction, accepted-cut speed, bad cuts, misses, bombs, walls, combo and multipliers remain under game control.

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
