# Quest 0.1.1 — MBF installation metadata

The tester's error rejects a mod version written as 0.1 at /version. That is not full SemVer; RhythmSaber already used 0.1.0. This patch uses 0.1.1 and fixes the separate issue of exact runtime dependency requirements, which can force downgrades or prevent installation after a dependency update.

QMOD requirements are now ^6.4.2 beatsaber-hook, ^0.18.4 custom-types, ^4.8.0 paper2_scotland2, ^0.4.55 BSML and ^1.6.3 MetaCore. The ranges are declared in mod.template.json; QPM's manifest merge preserves these entries, including when qmod zip regenerates the manifest. Build-time QPM/codegen/NDK pins remain unchanged for reproducibility and the strict 1.40.8 target.

The package verifier reads mod.json from the final QMOD and validates it against MBF's own vendored schema, without relaxing constraints. It also rejects non-caret runtime requirements, duplicate entries/dependencies and mismatches between the runtime minimum and audited build dependency. The source ZIP includes that schema and its license.

Package regression checks accept the real QMOD and reject modified copies with version 0.1 or an exact dependency requirement. These reproduce the metadata problems without requiring a headset. They do not demonstrate a live MBF install or future dependency ABI compatibility outside SemVer guarantees.

Quest gameplay/scoring/settings and ranking protection are unchanged. PC remains 0.1.0. The previous joint v0.1 release is retained; v0.1.1 contains the Quest update and corresponding source/checksums.
