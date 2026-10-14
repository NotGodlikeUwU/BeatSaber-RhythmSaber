# QMOD schema provenance

qmod.schema.json is the unchanged schema embedded by ModsBeforeFriday's agent, inspected on 15 October 2026.

Source: https://github.com/Lauriethefish/ModsBeforeFriday/blob/2d6f84a9f9c9d5a5723866de9c8ea0dfb20b37d4/mbf-agent/src/mod_man/qmod_schema.json

SHA-256: 70DD0939BBEA00552883D659D8215E79294A0ADA7FEBD566B32E159B14E9988E

The MBF loader identifies it as the QuestPatcher QMOD schema. Original project: https://github.com/Lauriethefish/QuestPatcher.QMod . Its zlib license is retained in QuestPatcher.QMod-LICENSE.txt.

scripts/verify-package.ps1 validates the manifest read from the actual QMOD with PowerShell 7 Test-Json. No schema constraints are removed or relaxed. Version 0.1 is rejected at /version; full SemVer such as 0.1.1 is required. Caret runtime-dependency requirements are separately checked against mod.template.json.
