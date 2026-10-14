# RhythmSaber 0.1 — Quest and PC

Public release 0.1 uses SemVer 0.1.0 in both mod manifests and the PC assembly. It preserves the latest implementation: Quest development 0.2.3 and PC development 0.2.4. This renumbers the current mod; it does not revert to the original 0.1.0 prototype described in historical notes.

Both implementations target Beat Saber 1.40.8; Quest requires build 1.40.8_7379. Quest is native C++/IL2CPP/Scotland2; PC is C#/BSIPA/Harmony. Source, installation/build/debugging instructions and both READMEs are in English.

Included features: full timing-only scoring, configurable windows and points, a white 75%-opacity judgement beam, native flying grades/offsets, smaller BAD CUT/MISS, optional statistics, custom-song handling and proportionally scored Arc/Chain types. PC includes the latest settings-tab content-area correction. Gameplay/configuration and guards are unchanged by this version reset.

Quest refuses altered scoring with BeatLeader or ScoreSaber present. PC supports the specifically audited DLL hashes and refuses unknown ranking DLLs. Animated future crossings are estimates marked `~`; sub-frame physical impact reconstruction is not implemented. See each platform's README for its full policy and live-test matrix.

## Files

- RhythmSaber-0.1.0.qmod: Quest install package.
- RhythmSaber-PC-0.1.0.zip: PC install package.
- RhythmSaber-0.1.0-source.zip: Quest source.
- RhythmSaber-PC-0.1.0-source.zip: PC source.
- RhythmSaber-PC-0.1.0-BeatMods.zip: prepared catalog-submission kit; not automatically submitted to BeatMods.
- SHA256SUMS.txt: checksums of release assets.

The GitHub repository contains both implementations. No game binaries, third-party runtime DLLs, development credentials, user configs, sample-map media or local caches are published.

## Upgrading from development builds

PC: close the game and replace Plugins/RhythmSaber.dll with this release, removing stale pending/duplicate copies; preserve UserData/RhythmSaber.json and existing launch arguments.

Quest: if the installer refuses a downgrade from development 0.2.x, remove only RhythmSaber and import the new QMOD. Preserve its config and installed dependencies; MetaCore stays 1.6.3. Fully restart the game.

Old development artifacts and the original prototype 0.1.0 are retained locally as separate backups. Historical notes retain their development numbers to preserve evidence. Current UI/visual changes and the full real online-submission matrix still require live headset testing; compilation and host checks do not establish those results.
