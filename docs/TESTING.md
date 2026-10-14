# Quest gameplay test plan

Historical sections retain the original version-specific numbers and restrictions. They do not override current README behavior.

## Current 0.2.2/0.2.3 display checks

172 host checks cover the existing math/guard and the smaller failure font/opacity curve. Check in the headset: readable opaque GOOD/GREAT, half-size BAD CUT/MISS, final fade, original pool-property restoration and optional text-size changes. PC additionally needs the wider settings viewport verified. Version 0.2.3 changes documentation/metadata only.

## 0.2.1 additions

164 host checks covered defaults, manual geometry preservation, configurable points, the 115 ceiling, grade ordering, special-note scaling and previous math/guard.

1. PERFECT/GREAT/GOOD award 115/100/70 with unchanged windows. Change each points field; start a new song and compare popup, total and results.jsonl.points. MISS stays zero.
2. Change Scoring text size below/above 1.6. Grade/points/offset and failures resize without changing timing. At 1.6 text is 40% smaller than 0.2.0.
3. Fresh config/reset enables RhythmSaber, line, judgements and milliseconds; statistics off. Geometry: 1.35 / 0.80 / 2.8 / 0.015, brightness 1, opacity .75, size 1.6.
4. Upgrading migrates old geometry defaults; manual values/windows survive. Point fields persist after restart.
5. Values >115, <0 or GOOD>GREAT explain the error and prevent altered scoring. Default-on/reset still runs BeatLeader/ScoreSaber conflict checks.
6. Disabled RhythmSaber plus a new level restores original points/fonts and permitted submission. Repeat Arc/Chain and animated-map checks.

## 0.2.0 additions (historical point values)

147 portable checks covered strict windows/scores, special-note maxima, formatter/~ marker, signed crossings, approaching/reverse/repeated trajectories, freeze/teleport/restart and guard matrix. Run `pwsh scripts/test.ps1`.

1. In 0.2.0, 0 ms →115; +/-20/30 →80; +/-40 →40; +/-65 →10; +/-85 →0. Swing/center add nothing; physical bad cuts/misses remain native. Current configurable defaults supersede these point values.
2. Upgrade 0.1.3 to 0.2.0 preserves Enable/calibration/distance/height. Old default windows migrate to 15/30/50/75/100; custom windows survive; beam is white with alpha .75.
3. Verify native popup grade/points/ms together, simultaneous left/right effects, fade and pool reuse. Show Judgements off and RhythmSaber off restore font/richText/overflow/indicator.
4. With HitScoreVisualizer active, test init, swing refresh, fade and reuse. RhythmSaber reapplies text in FlyingScoreEffect.ManualUpdate; coexistence requires live testing.
5. Arc heads/tails/combined and Chain heads/links/combined: PERFECT=native max; 0.2.0 GREAT=round(max*80/115); POOR/MISS=0. Check multiplier/FC, total/max and no fixed bonus.
6. Papercut/Bones select analytic; Breezer definitePosition/tracks select observed. Test per-note NJS, extended lanes, rotation, player tracks and dynamic NJS.
7. Noodle/Mapping/Cinema/Vivify/Chroma libraries must not prevent activation. Measure physical collider trajectories; shader-only displacement does not move the collider.
8. Predicted early animated cuts show `~`; observed crossings do not. Freeze/teleport/missing history yield zero, not original swing points. Check estimatedCuts/unmeasurableCuts in JSONL.
9. One beam without posts/lower rail, soft halo/ripples. Height/width do not change expected time; distance does. Player-track movement moves beam/plane together. Verify readability/FPS at 72/90/120 Hz.
10. Pause/restart/practice/long songs/failures/disable+next-song, colors/directions/bombs/walls; repeat ranking matrix. No altered online uploads.

These additions were not agent-tested on Quest at build time.

## Portable tests and package checks

`pwsh scripts/test.ps1` compiles actual independent C++ math/guard code with MSVC. Checks are not disabled by Release assert settings. Any C++20 host can also run:

```text
cmake -S . -B build-host -DRHYTHMSABER_HOST_TESTS=ON
cmake --build build-host --config Release
ctest --test-dir build-host -C Release --output-on-failure
```

Coverage includes sign symmetry, 0/+/-30/+/-85 ms, inclusive boundaries/next values, NaN/invalid windows, plane geometry, changed NJS/JD/spatial offsets, speed/calibration/long songs, 72/90/120 Hz frame-registration model, statistic denominators, ranking IDs/paths and guard retention until a new untouched song. A frame model is not a measurement of headset FPS/tracking.

`scripts/verify-package.ps1` checks manifest/dependencies, ARM64 ELF and build/QMOD SHA256 equality. It does not test MBF installation or IL2CPP loading.

## Device-test preparation

- Use Beat Saber 1.40.8_7379, Scotland2 and pinned manifest dependencies.
- Begin with an ordinary Standard map and calibration zero; disable other gameplay and ranking mods for the control run. Later test Arc/Chain and altered movement separately.
- Enable diagnostics, capture full logcat and retain matching .debug.so. Record Quest model, refresh rate, map/hash/difficulty, speed/NJS/JD/offset, windows and installed-library SHA256.
- Inspect the headset view. A mirror screenshot does not establish VR depth, readability or stability.

## Core scenario matrix

The original 0.1.2 matrix used windows 20/40/70/110/150 and points 115/100/75/40/15. Use the current configured table when testing current releases.

| Scenario | Acceptance criterion |
|---|---|
| Center crosses line plane, cut | Calibration zero: near-zero delta, PERFECT115 before multiplier; allow frame uncertainty |
| About -30/+30 ms | Correct sign and GREAT100 with current defaults |
| About -85/+85 ms | Original 0.1.2 BAD40; current defaults POOR0 |
| Edge cut at ideal time | 115 with allIsOK, identical to center cut |
| Small accepted swing at ideal time | 115 when native speed/direction validator accepts it |
| Large swing, poor timing | Only configured timing points; no angle/center addition |
| Wrong saber | Native bad cut/combo/energy loss |
| Wrong direction | Native bad cut; no successful timing award |
| Miss/bomb/wall | Native penalties/combo; bombs excluded from note Perfect% denominator |
| High BPM/simultaneous colors | Independent results; one increment per note; no lost cached records |
| Different NJS/JD/Jump Offset | Stable plane; expected from real trajectory; no double offset |
| Pause/resume/practice seek | No pause judgements or drift; stale trajectory/text cleared on seek |
| Disable plus next song | Native 70/30/15 restored, beam/HUD gone, only own MetaCore disabler removed |
| Enable with Quest BeatLeader/ScoreSaber | Refusal before altered scoring, toggle off, named conflict |
| Disabled with ranking mods | Ordinary upload behavior restored; no network interception |

Also test clear/native FC, physically valid timing-zero FC, fail, quit, ten restarts, repeated entry, scene/map change, No Fail, Faster/Slower/Practice speed. Result score must equal timing points through native multipliers/modifiers and match JSONL. Original PBs must not change during active play.

## Collision window measurements

For NJS such as 8/12/20/30 where available, collect physical early/late attempts at a fixed plane. Separate small/large hitboxes, correct/wrong color and direction. Log minimum/maximum accepted actual-expected and native bad-cut/miss flags. Never replace an unregistered cut with synthetic success. Adjust windows to reachable native collision ranges; all +/-150 ms may not be physically reachable.

Repeat at 72/90/120 Hz and under actual dropped frames. Capture lastFrameDeltaSongTime, mean/absolute error and grade distribution. Calibrate controller/audio latency separately; compare the beginning/end of a 10–20 minute song. Do not hide FPS-dependent error behind sub-frame precision claims.

## Quest ranking matrix

| Installed ranking mods | RhythmSaber requested | RhythmSaber off plus new level |
|---|---|---|
| Neither | Allowed; own MetaCore key before level, original ProcessScore blocked | Own guard released |
| BeatLeader only | Refused; original scoring | Native BeatLeader, including retry |
| ScoreSaber only | Refused; original scoring | Native ScoreSaber |
| Both | Refused; both conflicts named | Native combination |
| Old/unknown versions | Refused by ID/path regardless of version | No API interference |

For every row test clear/FC/restart/practice/quit/reenter/replay menu. Refused activation must compute/add no timing scores. Active testing requires disabling/removing both ranking mods and restarting the process. A modified label or invalidated flag alone is insufficient evidence.

Historical 0.1.2 compatibility additions: installed AutoSaber/Replay/Noodle/Mapping/MultiplayerCore should allow ordinary Solo Standard maps; active Replay preserves original replay data and disables timing only during playback. In 0.1.x, note movement/tracks/fake/cut flags/player-space animation/extended lanes refused only the current map; colors/environment tracks did not. Since 0.2.0 those map restrictions are replaced by observed-motion measurement. Library presence is not a claim of exact arbitrary-animation timing.

## Release readiness

A working claim for a specific Quest requires installed-hash verification and the relevant device scenarios. Host/build/package evidence alone is insufficient. Original 0.1.x releases lacked Arc/Chain; current releases implement proportional scoring but still need live totals and interaction checks. Quest ranking conflicts and sub-frame timing limitations remain explicit.
