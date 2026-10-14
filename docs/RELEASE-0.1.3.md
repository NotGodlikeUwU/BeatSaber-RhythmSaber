# RhythmSaber 0.1.3 — historical release notes

These rules describe 0.1.3. Later releases remove several map restrictions.

## Changes

- Replaced the floor stripe with an open floating frame: two rails and side posts, one mesh and UI/Default material, without colliders or post-processing.
- Default distance 1.5 m, top rail 1.35 m, width 2.8 m. Mod Settings exposes distance/height. Visuals and scoring share lineDistance.
- One-time migration from the old default 0.5 m distance; manual distances and other settings survive. The 0.1.2 QMOD/source archive remain available for rollback. Restore distance manually after downgrading if necessary.
- Downloaded songs are recognized through FileSystemBeatmapLevelData. GetDifficultyBeatmap resolves the selected difficulty; FileDifficultyBeatmap.GetBeatmapString reads its JSON. Other providers retain IBeatmapLevelData.GetBeatmapString.
- Inspection accepts UTF-8 BOM, comments and trailing commas. Color-only animation is allowed; spatial, movement, time or cut-rule modifications retain original scoring.
- Constant per-note NJS/Jump Offset are allowed because their real NoteJump retains linear Z with updated endpoints/duration. Animated time/trajectory and dynamic NJS events remain unsupported.
- Local results include lineHeight. Submission protection is unchanged.

## Scope and limits

Ordinary downloaded Standard maps with four lanes and normal notes/bombs/walls. Installed Noodle/Mapping mods are allowed. Arc/Chain, altered movement, rotations and dynamic NJS retain original scoring for the whole map in this release. SongCore loads songs; RhythmSaber does not replace it.

The platform edge is approximate rather than detected from scene geometry; environments differ. Height is a fixed gameplay coordinate adjusted manually for player height. Cuts before/behind the frame still require native collision acceptance.

## Verification and upgrade checklist

The user confirmed 0.1.2 works. At this release's build time, the new frame and custom-map gameplay were not yet headset-tested. Logs: tests-013.log, build-013.log, package-013.log, custom-maps-013.log. User map copies/media are excluded from artifacts.

1. Update QMOD and restart. Preserve Enable, windows, color and calibration. Only the former default distance changes to 1.5 m.
2. Check the top rail at 1.35 m without looking down; posts/open center must not move with the HMD or obstruct cuts.
3. Fence height must not change expected time. Fence distance must shift the ideal plane with the frame. Compare reachability at 1.5 m with native wasCutTooSoon; that game check is not disabled.
4. Play Papercut/Expert and Bones/ExpertPlus. File inspection is not a gameplay test. Expect Custom song inspected, Level armed and the active banner. Breezer/Normal has definitePosition/track and must retain original scoring with an explanation in this version.
5. Arc/Chain and animated Noodle maps retain original scoring in 0.1.3.
6. Check pause, restart, disable and next-song restoration. Online submission must remain blocked for altered results.
