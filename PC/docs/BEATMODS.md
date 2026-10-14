# BeatMods publication and BSManager visibility

The submission kit is prepared locally; it is **not published or approved**. BSManager is not modified. Use the exact ZIP/DLL from the kit when submitting: installed-mod detection depends on matching catalog content hashes.

## Kit contents

- RhythmSaber-PC-VERSION.zip: installable release, containing Plugins/RhythmSaber.dll, optional symbols, English instructions and license.
- RhythmSaber-PC-VERSION-source.zip: source, build scripts and English documentation; no game or dependency DLLs.
- summary.txt: short English catalog summary (under 100 characters, satisfying the inspected validator as well as the form's 200-character hint).
- description.md: English Markdown description with support and validation limits.
- MOD-README.md and TESTING.md: full mod instructions, audited ranking DLL hashes and the live-test checklist; LICENSE contains the MIT terms.
- icon.png and icon.svg: original 512px icon and editable vector source.
- publication.json: local preparation record with versions, dependencies and file hashes. **This is not a BeatMods API request or registration file.**
- SHA256SUMS.txt and CONTENT-HASHES.json: package integrity and MD5 content hashes for comparison with the catalog. BeatMods computes its own identifiers/hashes; do not invent database IDs.

## Submit manually

1. Source repository: https://github.com/NotGodlikeUwU/RhythmSaber (tag v0.1). Use the matching source for this release and your author account. Complete the live UI, gameplay and score-submission checks in TESTING.md before representing it as a verified release. GitHub publication does not register the mod in BeatMods.
2. Open [BeatMods Create a New Mod](https://beatmods.com/upload) and sign in with your GitHub account.
3. Select **Beat Saber** and **Gameplay**, name **RhythmSaber**, your source Git URL, icon.png, summary.txt and description.md. Create the mod entry using the site's form.
4. Add version **0.1.0** (public release 0.1), choose **PC (Universal)** (`universalpc`) and supported game version **1.40.8**. Upload **RhythmSaber-PC-0.1.0.zip**, not the outer submission kit, source ZIP or Quest QMOD.
5. Select actual catalog dependency versions compatible with 1.40.8: BSIPA (manifest ^4.3.4, audited 4.3.6), BeatSaberMarkupLanguage (^1.12.5, audited 1.12.5+bs.1.40.0), SiraUtil (^3.1.11, audited 3.2.1+bs.1.40.0), BS Utils (^1.14.2). The form uses dependency-version database IDs; select them through the UI rather than guessing. Do not claim other game versions without an audit.
6. Follow the site's verification workflow and moderator feedback. Creating an entry/uploading a ZIP does not establish approval. No submission has been made by the package-generation script.
7. After catalog acceptance, refresh/reopen the instance's Mods tab in BSManager. Install/import that exact release; its content hash should identify RhythmSaber and show the English description. An older DLL hash remains a different release.

Until publication is accepted, manual installation still works through BSIPA, but BSManager 1.6.0 may omit the custom DLL from its installed-mod list. No in-DLL manifest flag can force catalog approval.

## Rebuild the kit

Run build.ps1 normally, or after an already validated build run tools/Package.ps1. The release version comes from manifest.json. Packages must be rebuilt after changing the embedded manifest. Do not edit/recompile the DLL after submitting its hashes without uploading a new matching version.

## Evidence

Inspected on 13 October 2026: installed BSManager 1.6.0; [BSManager source commit 00380c7](https://github.com/Zagrios/bs-manager/tree/00380c798ea3a8447270e8bbd367e921d03d154b), main/services/mods/bs-mods-manager.service.ts getModsInDir/getModFromHash and renderer/services/bs-mods-manager.service.ts installed-mod mapping. Unknown hashes are omitted before a metadata manifest can supply catalog identity. The [official import guide](https://github.com/Zagrios/bs-manager/wiki/install-mods-on-your-version) supports ZIP/DLL import.

The live BeatMods form exposes Game, Category, Icon, Mod Name, Git URL, Summary and Markdown Description. [Official frontend](https://github.com/Futuremappermydud/bsmods-frontend) NewProject.svelte and upload/version/[id]/+page.svelte confirm separate mod creation/version upload, supported versions, platform, dependencies and verification. Validator.ts constrains summary to 100 and description to 4096 characters. This kit uses the stricter summary limit despite the form hint. Forms/rules can change; follow the current website without bypassing validation.
