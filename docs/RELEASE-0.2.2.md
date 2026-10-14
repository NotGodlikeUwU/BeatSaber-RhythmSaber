# RhythmSaber 0.2.2 — Quest and PC display fixes

PC uses one compact status line, a full-width vertical group, fixed selected-tab height 48 and constrained text/footer heights. ScrollableSettingsContainer retains native stretch anchors inside the tab. The fix follows the user's screenshot and installed BSML 1.12.5 handlers.

On Quest and PC, BAD CUT/MISS use half their previous font size: `2.25 * TextScale`, default 2.16 instead of 4.32. A timing MISS inside a numeric popup is also half the size of other grades. The shared user text-size setting still applies.

Timing judgements no longer inherit native score-popup transparency. Normalized lifetime t=0..0.75 uses alpha 1; t=0.75..1 fades linearly to zero. Refresh callbacks retain progress; initial alpha is 1. Release restores original color/alpha multiplier, font and other saved properties.

Scoring, windows, Arc/Chain maxima, combo and ranking protection are unchanged. Game launch parameters were not edited. Previous artifacts remain in dist and backups/0.2.1-before-feedback-fix.

Validation: 172 C++ host checks; ARM64 --no-undefined build (dependency-header warnings); QMOD/ELF/hash validation. PC: build without errors/warnings, 429 math/feedback checks, 112 ABI checks for current DLLs and 482 native managed scoring checks with Harmony. New UI/text appearance still requires live VR validation.
