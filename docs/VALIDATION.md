# Validation history

## Public 0.1 (manifest 0.1.0) — 14 October 2026

This is the latest implementation renumbered for publication, not the initial historical 0.1.0 prototype. Quest passed 172 host checks and ARM64 compilation/link with --no-undefined; dependency-header warnings remain. QMOD metadata/dependencies/ELF/build hash and official QMOD/QPM/lock schema checks passed. PC compiled with zero errors/warnings; 429 timing/config/trajectory/feedback, 112 ABI/resource and 482 real managed scoring checks passed. Its embedded manifest and assembly use 0.1.0.

Both READMEs and source documentation are English. Release assets contain separate Quest and PC install/source packages and a prepared BeatMods kit. The repository excludes local logs/configs/caches, game binaries and cloned research trees. Historical development artifacts and the original prototype were backed up separately before reusing the version number.

No new live VR/game-network validation was performed for this metadata release. The latest PC settings layout and submission matrix still require the documented gameplay checks. Publishing to GitHub does not establish BeatMods approval or BSManager catalog registration.

## 0.2.3 — 13 October 2026

English documentation/metadata update; gameplay remains 0.2.2. PC builds with zero errors/warnings: 429 math/config/trajectory/feedback assertions, 112 ABI/resource checks and 482 real managed scoring checks passed. Quest passes 172 host checks and ARM64 --no-undefined compilation/link; existing dependency-header warnings remain. QMOD package/ELF/hash validation and official QMOD/QPM/lock schemas passed.

Logs: logs/pc-build-023.log, logs/tests-023.log, logs/build-023.log and logs/package-023.log. Checksums: dist/SHA256SUMS-0.2.3.txt and PC/dist/SHA256SUMS.txt. Current source/install documentation and the submission kit are checked for English text and matching file hashes. Old archives are retained unchanged.

The BeatMods kit is prepared only: no upload, catalog approval or BSManager modification. Its real source-repository URL and author account must be supplied before publication. No new live VR or network test was performed; the existing gameplay/submission test matrix remains open.

Build/static checks and live headset evidence are different. Historical entries retain their original test counts and limitations; later release support supersedes older restrictions.

## 0.2.2 — 13 October 2026

172 host checks passed. ARM64 compilation/link completed with --no-undefined. QMOD manifest/dependencies/ARM64 and packaged/build SHA256 equality passed; hashes are in dist/SHA256SUMS-0.2.2.txt. PC compiled without errors/warnings: 429 timing/config/trajectory/feedback, 112 ABI and 482 native managed scoring checks passed.

Changes: PC settings viewport, opaque initial grade phase and half-size BAD CUT/MISS on both platforms. No new runtime dependencies, scoring or guard changes. New appearance requires a fresh VR check. Previous artifacts are preserved.

## 0.2.1 — 13 October 2026

164 host checks and ARM64 --no-undefined compilation/link passed. QMOD manifest/dependencies/ARM64 and JSON schema passed. Packaged/build SHA256: `35AD2DBCEAF71C9BEF239B8709BE1C71EAF9E9742546C0CA605BC59CA8D7E2AB`.

Changes: smaller adjustable text, configurable points and screenshot defaults/migration. No new dependencies; guard unchanged. Logs: tests-021.log, build-021.log, package-021.log. The agent did not test 0.2.1 on Quest; user screenshots showed settings from the preceding release.

## 0.2.0 — 13 October 2026

The user confirmed 0.1.3 works. New 0.2.0 passed 147 host checks and ARM64 --no-undefined compilation/link. New hooks use generated 4008.0.0 headers; fixed bonus was checked against supplied libil2cpp. No new dependencies. QMOD verify-package and official JSON schema passed (logs/package-020.log, logs/schema-020.log). Packaged/build SHA256: `E1B4041E180B876A8A5526A79B0BE49F49D063384B7658A1FBC2BD3D7B7C99B2`.

Papercut/Bones copies select analytic; Breezer animated definitePosition/track selects observed (logs/custom-maps-020.log). This is file-level mode selection, not gameplay. Final adb devices output was empty.

Not device-confirmed at build time: single beam/flying grades/FPS, Arc/Chain totals, HSV/Noodle/Mapping/Cinema/Vivify/Chroma, player tracks, pause/restart/practice and ranking matrix. Future nonlinear timing remains predictive with `~`; sampling has frame error, unmeasurable cuts get zero. Exact timing on all modcharts is not claimed. Working 0.1.3 remains in dist and backups/0.1.3-live-confirmed.

## 0.1.3 — historical build evidence

Experimental build; new changes still needed Quest validation. The user confirmed 0.1.2 works, without establishing every matrix scenario.

The open frame was 2.8 m wide at distance 1.5 m with top rail 1.35 m. Height setting and one-time default-distance migration were added. All rail vertices use XY/local Z=0; visuals and PERFECT share session.settings.lineDistance.

Downloaded songs also resolve through FileSystemBeatmapLevelData/GetDifficultyBeatmap. JSON accepts BOM/comments/trailing commas, constant per-note NJS/Jump Offset and color-only animation. Complex Noodle motion/Arc/Chain were not yet supported.

- ARM64 --no-undefined passed (logs/build-013.log). Library/QMOD SHA256: `427AA320B14F8BCFD316DDEEDB89DC29353F94F2C6316DA478465E7B8779D59F`.
- 111 C++ checks passed (logs/tests-013.log), including defaults/migration, rails, ideal Z=1.5 time, decorative animation and per-note jump settings.
- Quest copies of Papercut/Expert and Bones/ExpertPlus passed inspection. Breezer/Normal was correctly refused for definitePosition/track/disableNoteGravity. logs/custom-maps-013.log validates JSON, not gameplay.
- QMOD game version, Scotland2, dependencies, MetaCore=1.6.3 and ARM64 passed (logs/package-013.log). Previous 0.1.2 packages were preserved.
- Frame visibility, reachability, custom-map gameplay and FPS were not headset-tested in this build entry.

## 0.1.2 — compatibility correction

Removed blanket installed-mod checks for AutoSaber, Replay, Noodle Extensions, Mapping Extensions and multiplayer plugins. Active Replay uses its MetaCore disabler. Custom maps were inspected through IBeatmapLevelData.GetBeatmapString: ordinary notes/colors allowed; movement/tracks/cut flags/player-space animation retained original scoring. Lane/type/rotation/NJS and ranking checks remained.

ARM64 --no-undefined passed (logs/build-012.log); 94 host checks passed (logs/tests-012.log), including installed/active Replay, allowed extensions and custom data. MetaCore stayed 1.6.3. Allowing a library does not prove arbitrary-trajectory support or live coexistence.

## 0.1.1 — installation correction

0.1.0 requested metacore=1.6.2; QuestPatcher refused to downgrade installed 1.6.3. QPM dependency/lock/downloadIfMissing were updated to 1.6.3 and linked against its official release. Compared API/submission hooks did not change. Package checks reject a manifest requesting 1.6.2.

ARM64 --no-undefined and 73 host checks passed (logs/build-011.log, logs/tests-011.log). QMOD manifest uses MetaCore=1.6.3 and the v1.6.3 URL; packaged library equals the build (logs/package-011.log).

ADB confirmed Beat Saber 1.40.8_7379 on Quest 3 and installed MetaCore SHA256 `D2BAC7FAA77279FD7660A9FDCA21AC7D3596CAE0797E8BC213246731A38C063C`, matching the official library used to build. This verifies the dependency, not RhythmSaber installation/gameplay. Scoring/visuals did not change.

## Initial build evidence

- Dependencies restored by QPM 1.5.11/local cache. bs-cordl 4008.0.0; exact versions in qpm.shared.json.
- Android NDK r27d/27.3.13750724, Clang 18, CMake 4.4.2, Ninja, Android API24 ARM64, C++20, RelWithDebInfo.
- --no-undefined linkage; target headers confirm hook types/signatures. Inline-hook size protection is retained.
- ELF64/AArch64, SONAME librhythmsaber.so; setup/late_load exports. DT_NEEDED uses standard Android and real release dependency names, without .debug.so.
- 73 C++ math/guard checks with MSVC 14.51.36231 x64/C++20 (logs/tests.log).
- qpm qmod zip --skip_build --offline plus verify-package: game1.40.8_7379, Scotland2, one late mod, runtime dependency manifests, ARM64 and byte-identical build/ZIP SHA256 (logs/package.log).
- qpm.json/qpm.shared.json pass official QPM schemas. mod.json passes official QMOD constraints; only obsolete fragment-only $id annotations incompatible with draft2020-12 were stripped. Required fields/types/patterns/constraints remain unchanged (scripts/validate-schema.py, logs/schema.log).
- Movement/timeDeviation/RefreshScores formulas checked against local libil2cpp; game binary excluded from artifacts.

Clang warnings remain in third-party generated/custom-types/BSML headers, including unused registration counters. Successful compilation does not establish runtime correctness.

## Historical unverified scenarios and artifacts

Initial adb devices was empty. A device appeared during 0.1.1 and MetaCore was checked, but the following were not then tested: MBF installation/installed hash, IL2CPP lifecycle/crashes, floor-line/HUD appearance and FPS, collision windows at72/90/120 Hz, Combo/FC/Accuracy, pause/restart/practice, ranking matrix and ordinary submission restoration.

Arc/Chain were not implemented in 0.1.x; current releases supersede that restriction. Quest BeatLeader/ScoreSaber conflict refusal and lack of sub-frame TOI remain.

Historical artifacts: dist/RhythmSaber-0.1.2.qmod and source ZIP, preserving the map-specific policy; build/librhythmsaber.debug.so contains symbols for the current build and must not replace the release library. dist/SHA256SUMS.txt describes the latest package/source/release/debug artifacts.

Use a Quest with USB debugging and complete [TESTING.md](TESTING.md) for a current live-validation claim.
