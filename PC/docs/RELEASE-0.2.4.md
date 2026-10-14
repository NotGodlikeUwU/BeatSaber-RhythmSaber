# PC 0.2.4 — settings-tab content area

The user screenshot from 13 October 2026 shows tall General/Beam/Timing/Points buttons and almost no usable settings page in PC 0.2.3. The game log confirms RhythmSaber 0.2.3 and BSML 1.12.5+bs.1.40.0 loaded. No RhythmSaber parser exception accompanies the screenshot; unrelated Camera2/FPSCounter errors are not treated as its cause.

## Investigation

The previous generated root was a VerticalLayoutGroup with childControlHeight enabled. Its direct tab children carried min-height/pref-height attributes. In [BSML v1.12.5](https://github.com/monkeymanboy/BeatSaberMarkupLanguage/tree/b25aac67022343d3982396588294f9a91d66163c), TabTag derives from BackgroundTag and creates ContentSizeFitter, Backgroundable and Tab, but no LayoutElement or layout group. LayoutElementHandler therefore has no target for those height attributes. A parent's controlled-height layout has no tab preferred height to use. TabSelectorTag clones the native segmented-control hierarchy; pref-height likewise does not add a missing LayoutElement. These methods were also inspected in the installed BSML.dll IL.

The wrong LayoutElement assumption explains why the prior size declarations did not size the content. Source evidence supports the mechanism; final engine layout still requires the headset check below.

## Correction

Use BSML's own tab-container pattern: a stretch-anchored background rather than a vertical layout parent. The status occupies the top 5 units, selector the next 7 with a 1-unit gap, reset the bottom 6 with a 1-unit margin. Pages stretch horizontally and vertically between them: their height is the actual settings-panel height minus 22, with no fixed 48-unit tab declaration. All pages and settings scroll roots fill the same rectangle. Native Mod Settings already provides a 110-unit-wide view controller; its outer navigation and other mods are not resized.

ContentSizeFitter is Unconstrained on the outer frames/reset so it cannot override explicit anchors. TabSelector still switches the same four pages and settings binding is unchanged. No new hooks, dependencies, gameplay changes or per-frame layout scripts are added. GenerateSettings.py remains the source of the generated BSML.

## Validation and manual check

Build/ABI checks verify the installed dependencies and embedded markup. Ordinary-CLR tests cannot execute Unity's layout engine; a passed build is not visual proof.

1. Fully close/restart the same 1.40.8 instance with existing OpenXR launch arguments.
2. Open Mod Settings → RhythmSaber: selector buttons should be one short row near the top, with the list below taking most of the panel.
3. Switch all four pages. General must expose the toggles/text size; Beam and Timing must scroll to their last rows; Points must show all five point settings.
4. Change a setting, switch tabs, close/reopen the menu and confirm the value persists. Reset and the host OK/Cancel buttons must remain separate and reachable.
5. Repeat menu opening/closing and tab switching; inspect fresh logs for BSML errors. Confirm no overlap or clipped labels in VR.

PC 0.2.3 source/binaries and generator are preserved in backups/0.2.3-before-PC-tab-layout. Quest remains at 0.2.3. BeatMods kits are preparation only and have not been published.
