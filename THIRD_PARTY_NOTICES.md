# Third-party notices

RhythmSaber uses the Quest modding libraries listed in qpm.json/qpm.shared.json.
Runtime dependency packages are installed separately and retain their own licenses.

The binary compiles the beatsaber-hook inline-hook sources and header utilities.
Their original MIT license is included in third_party/beatsaber-hook-LICENSE.txt.
The ARM64 inline-hook implementation by Rprop retains its separate MIT notice
in third_party/And64InlineHook-LICENSE.txt.
Custom-types, config-utils, fmt and Scotland2 header licenses are also retained.
The CMake utility/toolchain helpers are derived from Lauriethefish's
quest-mod-template (Unlicense; text included). Native game binaries and cloned
research repositories are not redistributed in the source archive.

- beatsaber-hook: https://github.com/QuestPackageManager/beatsaber-hook
- custom-types: https://github.com/QuestPackageManager/Il2CppQuestTypePatching
- config-utils: https://github.com/Metalit/config-utils
- Scotland2: https://github.com/sc2ad/scotland2
- fmt: https://github.com/fmtlib/fmt
- BSML: https://github.com/bsq-ports/Quest-BSML
- MetaCore: https://github.com/Metalit/MetaCore
- quest-mod-template: https://github.com/Lauriethefish/quest-mod-template
- QMOD validation schema: the unmodified schema embedded by ModsBeforeFriday, originally from QuestPatcher.QMod. Provenance and the original zlib license are retained in schemas/README.md and schemas/QuestPatcher.QMod-LICENSE.txt.
