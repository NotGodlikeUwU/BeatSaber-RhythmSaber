# RhythmSaber 0.2.1 — historical release notes

Scoring text size replaces Flying judgement size, with range 0.25–3.0. Default remains 1.6 as in the screenshots. Grade/points/offset and BAD CUT/MISS are 40% smaller than 0.2.0. The existing Judgement Scale config key is retained.

PERFECT/GREAT/GOOD/BAD/POOR points are individually configurable: defaults 115/100/70/10/0, range 0–115, non-increasing values with equality allowed. Invalid values prevent altered scoring. MISS remains zero. Arc/Chain scale by points*nativeMax/115 with rounding and retain their maximum. JSONL records points and configurable-1 policy.

Screenshot defaults: Enable, Show Line, Show Judgements and Show milliseconds on; Statistics off. Distance 1.35 m, height 0.80 m, width 2.8 m, text size 1.6, thickness 0.015 m, brightness 1.0, opacity 0.75, white. Timing windows remain 15/30/50/75/100 ms.

Old default geometry 1.5 m / 1.35 m / 0.025 m migrates once. Manual geometry, Enable and timing windows survive. Statistics is disabled during migration. Reset to defaults restores the full screenshot profile; reopen the page. Default-on uses the same validity/submission-conflict checks as manual enabling.

164 automated checks passed; ARM64 compilation/link used --no-undefined. Logs: tests-021.log, build-021.log, package-021.log. New text readability was not headset-checked at build time. Observed/predicted Noodle timing and other 0.2.0 limitations are unchanged.

Import dist/RhythmSaber-0.2.1.qmod through QuestPatcher/MBF and fully restart. Previous 0.2.0 artifacts remain for rollback.
