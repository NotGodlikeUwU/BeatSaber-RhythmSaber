# RhythmSaber 0.1 — Osu-like scoring system mod for Beat Saber 1.40.8 Quest/PC

WARNING: This mod does NOT WORK with ScoreSaber/BeatLeader installed. Please disable these mods to play RhythmSaber

An experimental mod that changes Beat Saber's scoring logic to OSU/DJMax-like timing scores based on music rhythm, a white judgement beam and flying hit grades. This repository contains two separate version of this mod, both PC and Quest versions. Made with the help of ChatGPT 6.1

<img width="1080" height="1106" alt="IMG_20261014_103132" src="https://github.com/user-attachments/assets/68f9137f-25a3-4db1-a452-65d990f599af" />


## Features

- Replaces the entire Beat Saber accuracy scoring formula with musical timing. Now you don't need to hit precisely at the center of the block to get full 115 - just do it at the right timing
- Fully configurable scores and timings.
- Displays a colored grade (POOR/BAD/GOOD/GREAT/PERFECT), which also shows you how fast/slow you were in milliseconds.
- One white judgemental line (fully configurable position, size and color)
- Noodle Extensions, Mapping Extensions, Cinema and Chroma support. Vivify is not tested.
- block color, direction, accepted-cut speed, bad cuts, misses, bombs, walls, combo and multipliers remain vanilla.

## Timing defaults and settings

| Absolute offset, inclusive upper bound | Grade | Ordinary-note points |
|---|---|---:|
| 0–15 ms | PERFECT | 115 |
| >15–30 ms | GREAT | 100 |
| >30–50 ms | GOOD | 70 |
| >50–75 ms | BAD | 10 |
| >75–100 ms | POOR | 0 |
| >100 ms | MISS | 0 |

All five windows and point values are configurable in Mod Settings and config-utils.

## Installation

QUEST: download .qmod from Release and install it with BMBF/QuestPatcher.
PC: download the .zip file and install it using BSManager or manually extract the contents to Beat Saber/BeatSaber_data/plugins

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
