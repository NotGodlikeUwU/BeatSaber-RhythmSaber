# PC 1.40.8 gameplay validation

Build, independent math, ABI and managed scoring have been checked. Earlier PC gameplay was confirmed by the user; the new UI/visual changes and real online-submission paths still need live validation.

Before testing, close the game, extract the ZIP and compare SHA256SUMS.txt. Use the separate BSManager 1.40.8 instance and retain your existing VR launch parameters. Do not update ranking DLLs between their hash audit and testing.

## Gameplay checklist

1. Open RhythmSaber settings without BSML/Zenject/Harmony errors. All tabs, scrolling and reset must be usable; config must survive restart. Verify the corrected viewport in the headset.
2. Test an OST song and an ordinary downloaded map in Solo. One white beam: distance 1.35 m, height 0.80 m, width 2.8 m, opacity 0.75. HMD rotation must not move it.
3. Compare actual/expected/delta diagnostics with the popup: 0 ms = PERFECT 115, +/-30 ms = GREAT 100, +/-45 ms = GOOD 70, +/-85 ms = POOR 0 under current defaults.
4. Edge cuts and small accepted swings must not change timing points. A large swing with poor timing must not earn the original 70+30+15.
5. Wrong saber/direction, physical misses, bombs and walls follow native rules. No duplicate increments. A physically valid timing MISS retains native combo and earns zero.
6. Test simultaneous colors, high BPM, long songs, different NJS/Jump Distance/Jump Offset and Faster/Slower/Practice speed.
7. Pause/resume and practice seek must clear stale trajectory state. Restart applies new config. No accumulated drift.
8. Test ArcHead/Tail, ChainHead/Link and combinations. PERFECT earns the native maximum, not 115 for every link. Total/max score, Accuracy and FC use native mechanics and new earned points.
9. Test Noodle, Mapping Extensions, Chroma, Cinema/Theater and Vivify maps. Compare rotated/animated note centers to the visible plane. Predictions display `~`; teleports/unmeasurable motion show NO CROSSING and zero.
10. With HitScoreVisualizer active, verify grade/points/offset, opaque GOOD/GREAT, smaller BAD CUT/MISS, fade and pool reuse. Original properties must return after disabling RhythmSaber. Also test the player option to hide cut scores.
11. Change points, windows, text size, RGB/alpha and position; changes apply next song. Invalid windows or increasing grade points prevent altered scoring and explain why.
12. Disable RhythmSaber and start a new song: original angular points, popup, result and combo; no beam/statistics.
13. Multiplayer/Campaign/Tutorial retain original scoring without leftover Solo state.

## Online-submission matrix

For each row test active/off, clear/FC, fail, quit, restart, practice, replaying the same map, returning to menu and starting a different song.

| BeatLeader | ScoreSaber | Expected active behavior |
|---|---|---|
| Absent | Absent | Timing; no official upload or original PB |
| Audited DLL | Absent | Timing; no BL save/upload/retry |
| Absent | Audited DLL | Timing; no SS upload |
| Audited DLL | Audited DLL | Timing; no BL/SS upload |
| Unknown version of either | Any | Original scoring with refusal reason |

Use fresh game logs to verify submission-disable and blocked-guard entries, no new BeatLeader replay and no ScoreSaber upload-start. Confirm no leaderboard entry or original PB change. A modified label alone is not evidence. Test a legitimate old BL retry followed by modified play: no new modified replay may be created for later retry. A tagged object must remain blocked after starting an ordinary song.

With RhythmSaber off, the next ordinary song may submit if the game and other mods allow it. Old modified results remain blocked. Do not manually edit original records or server data.

## Performance and evidence

Repeat at available 72/90/120 Hz with the same controllers/audio path, a long dense map, HitScoreVisualizer and a mapping-mod scene. Compare CPU/GPU frame times. Beam updates at most 20 times per second; mesh/material are not recreated per hit. Text uses native pools but formatting allocates strings, so zero GC is not claimed.

Record map name/hash, modifiers, config, fresh log, DLL hash, FPS and observed outcome. Build/static evidence alone does not close gameplay or online compatibility.
