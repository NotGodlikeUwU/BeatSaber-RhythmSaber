# Quest research: 0.2.0 and later

The 0.1.x sections below are historical. Current rules are in README.md and the release notes. The old Arc/Chain and mapping-data refusals were removed in 0.2.0.

- The supplied HitScoreVisualizer-master/src/Main.cpp was studied for FlyingScoreEffect.InitAndPresent/ManualUpdate and pooled FlyingTextSpawner creation through EffectPoolsManualInstaller. Its bs-cordl 4008.0.0 matches this target. RhythmSaber implements its own handlers; the reference code is excluded from archives.
- Generated ABI confirms FlyingScoreEffect.RefreshScore(int score, int maxPossibleCutScore). Clang/MAKE_HOOK_MATCH checks new hook signatures. NoteController.ManualUpdate is 24 bytes; tiny 8-byte getters are not hooked.
- GoodCutScoringElement.get_cutScore at 0x3bef4b4 adds before/after/center and NoteScoreDefinition.fixedCutScore. For native types 1–8, both final getters return the full timing field and bypass that bonus. Native ScoreModel maxima remain.
- NoodleExtensions applies animation/track offsets before original NoteController.ManualUpdate and clears temporary context afterward. Observation after original reads the final position. The beam uses PlayerTransforms.originParentTransform; beamTransform.InverseTransformPoint(position).z accounts for parent scale/shear.
- Analytic timing is used only when current position matches linear NoteJump movement and the world plane. Otherwise actual movement history is used. Future crossings can only be locally predicted and display `~`; arbitrary modcharts are not claimed to be exact.
- Actual time is AudioTimeSyncController.songTime in HandleNoteWasCut, matching the native collision handler's clock. note.time-timeDeviation is diagnostic only; Noodle linked notes can copy another note's timeDeviation.
- Mapping support does not weaken BeatLeader/ScoreSaber protection described below.

# Historical Quest 1.40.8 research (0.1.x)

## Evidence and references

Real repositories and pinned versions were inspected; game API uses local bs-cordl 4008.0.0. Critical formulas were compared with the supplied ARM64 libil2cpp.so 1.40.8, SHA256 `EDAE5059BD5A85BA354EB679E1CDCC3816116D2E2B29B3A16A70C05B56DD07CB`. Game binaries are not distributed. Headers establish ABI, not complete implementation; formula evidence also uses disassembly. This is static evidence, not a live gameplay check.

- [BSMG Quest intro](https://bsmg.wiki/modding/quest/intro.html), [configuration](https://bsmg.wiki/modding/quest/config.html), [UI](https://bsmg.wiki/modding/quest/ui.html), [Quest modding](https://bsmg.wiki/quest-modding), [MBF](https://bsmg.wiki/quest/modding-with-mbf.html).
- [Scotland2](https://github.com/sc2ad/scotland2), build/headers 0.1.7.
- [MetaCore v1.6.2](https://github.com/Metalit/MetaCore/tree/v1.6.2), commit f54126c58b4604d5fe04c105a774aaa1bd81a593: shared/game.hpp, src/game.cpp, src/hooks.cpp.
- [MetaCore v1.6.3](https://github.com/Metalit/MetaCore/tree/v1.6.3), commit 65bf1d39f9e0f416b6e66df2422fa69d137eda1d. game.hpp/game.cpp and submission hooks are unchanged from v1.6.2. Dependency updated in 0.1.1 to avoid downgrading the installed QuestPatcher version.
- [BeatLeader v0.9.0](https://github.com/BeatLeader/beatleader-qmod/tree/v0.9.0), commit 64558e197473a94b86569f25089ada88a83df273: include/Utils/ModConfig.hpp, src/Utils/ReplayManager.cpp.
- [ScoreSaber Quest v3.4.1](https://github.com/ScoreSaber/quest-mod/tree/v3.4.1), commit e437af98de0359cfc8c19ccd4144a949c0c84e18: ScoreSubmissionController.cpp and LiveReplayStreamingService.Stream/Proto/Recording.cpp.
- [TriickSaber](https://github.com/BinitDOX/TriickSaber), commit 66d58cf5a28704096eb744045e0df16973eca60e. README mentions bs-utils, but current qpm.json uses MetaCore. README alone is not proof that all submission paths are blocked.

Legacy bs-utils/PC BS_Utils examples are not copied into Quest. The actual MetaCore API is verified; no invented BS_Utils.DisableScoreSubmission or PC Harmony patches are used on Quest.

## Native scoring chain

NoteCutter.Cut(Saber*) performs swept geometry testing. GameNoteController.HandleCut(Saber*, Vector3, Quaternion, Vector3, bool) validates color/direction/speed, creates NoteCutInfo and sends NoteController.SendNoteWasCutEvent(ByRef<NoteCutInfo>).

ScoreController.HandleNoteWasCut selects a good/bad scoring element. GoodCutScoringElement.Init and CutScoreBuffer.Init initialize the normal 70+30+15 buffer. In this version RefreshScores updates swing fields; its callers send notifications. RhythmSaber replaces RefreshScores, storing the full timing score in _beforeCutScore and setting _afterCutScore/_centerDistanceCutScore to zero. This field placement does not impose a 70-point limit.

CutScoreBuffer.get_cutScore and GoodCutScoringElement.get_cutScore return the same complete timing score. Native ScoreController/ScoreMultiplierCounter/ComboController handle multipliers, maxima, failures, bombs and walls. No separate add-score call is made, preventing duplicate increments.

Before/after/center getters are 8 bytes, below the safe inline-hook minimum. They are not hooked. RefreshScores is changed without bypassing beatsaber-hook's tiny-method protection.

ScoreController.DespawnScoringElement removes temporary note records. The buffer retains its score independently; final getters do not depend on the deleted record. A fixed 512-record capacity covers pending swings without per-hit GameObjects/maps; absent records yield zero and diagnostics.

## Movement and clock (historical linear implementation)

NoteMovement._jump points to NoteJump. During the main jump, Z interpolates from _startPos.z to _endPos.z, starting at _noteTime-_halfJumpDuration with duration _jumpDuration. X/Y additions do not change the Z plane crossing. The final quarter may include an extra end-distance movement; the forward line is within the main portion. Altered trajectories were excluded in 0.1.x.

NoteMovement.Init already incorporates spatial zOffset into NoteJump offsets; do not subtract it again. NoteJump.ManualUpdate uses world coordinates and _worldRotation. Rotating maps were excluded in 0.1.x; the line used static world X/Z rather than HMD position.

GameNoteController.HandleCut calculates timeDeviation=noteData.time-songTime. The old implementation reconstructed actual time from that field and computed plane expected time separately. Native deviation has the opposite sign to actual-expected. The current implementation reads songTime directly, as explained above.

IAudioTimeSource provides songTime, timeScale and lastFrameDeltaSongTime. There is no wall-clock timer, Time.deltaTime accumulation or separate post-pause timer. SaberMovementData has positions/times, but NoteCutter/HandleCut do not expose a verified first-contact time parameter. cutPoint alone cannot identify an exact sub-frame timestamp for a moving/rotating collider. Swept TOI reconstruction needs separate validation and is not implemented.

## Hooks

| Hook | Purpose |
|---|---|
| GameplayCoreInstaller.InstallBindings | Capture setup/lifecycle without changing bindings |
| AudioTimeSyncController.Start | Mode/map checks, immutable settings, beam/HUD creation before gameplay |
| AudioTimeSyncController.Update | Song-clock text expiry and statistics |
| AudioTimeSyncController.Pause/Resume/OnDestroy | Pause/cleanup; Destroy does not release submission protection |
| ScoreController.HandleNoteWasCut/HandleNoteWasMissed | Timing for real cuts; native miss statistics |
| CutScoreBuffer.RefreshScores/get_cutScore | Replace full 70+30+15 formula |
| GoodCutScoringElement.get_cutScore | Timing total for native scoring/multiplier |
| ScoreController.DespawnScoringElement | Remove temporary records |
| LevelCompletionResultsHelper.ProcessScore | Block original PB and official submission for altered play |
| PartyFreePlayFlowCoordinator.WillScoreGoToLeaderboard | Additional Party leaderboard block |
| StandardLevelScenesTransitionSetupDataSO.Finish | Save separate local result, then native results screen |

Later releases also observe NoteController.ManualUpdate and style pooled flying score/text effects; inspect src/Hooks.cpp for the complete current list.

## Quest submission guard (still applicable)

MetaCore Game::SetScoreSubmission("rhythmsaber", false) adds a unique key to the shared disabler set before level start. Activation verifies its own key via GetScoreSubmissionDisablers(). DisableScoreSubmissionOnce is deliberately avoided because scene-end removal risks result-callback order.

Protection survives pause, restart, finish and Destroy. It is released only before a new untouched song with the feature off. Result hooks preserve protection after later menu changes. SetScoreSubmission(..., true) removes only RhythmSaber's key and preserves other disablers.

BeatLeader 0.9.0 checks MetaCore in ProcessReplay, but saved replays can pass RetryPosting → TryPostReplay without another check. MetaCore alone is insufficient for every submission path. RhythmSaber refuses activation when Quest BeatLeader is installed.

ScoreSaber 3.4.1 checks MetaCore in Decide for ranked upload, including fail/restart/quit. However HandleLevelFinished calls live-streaming Complete beforehand, and streaming sends ScoreSummary. TryStartStreaming/CanUsePublicPresenceForCurrentLevel do not check MetaCore. RhythmSaber refuses activation when Quest ScoreSaber is installed.

Both checks use mod IDs and known .so paths in menu and at level start; both conflicts are listed when present. No server changes, packet interception or anti-cheat bypass. Supporting these Quest mods requires guarded APIs covering every path and device validation.

## Arc/Chain policy history

Real NoteData.ScoringType includes Normal, ArcHead, ArcTail, ChainHead, ChainLink, ArcHeadArcTail, ChainHeadArcTail, ChainLinkArcHead, NoScore and Ignore. ScoreModel.GetNoteScoreDefinition supplies distinct maxima/fixed parts, so not every element may receive 115.

In 0.1.2, SliderData or any non-Normal non-bomb scoring type disabled altered scoring for the entire map before its first hit. Arc/Chain handlers were not implemented then. Since 0.2.0, scored types 1–8 are handled with proportional native maxima and no fixed bonus; old whole-map refusals no longer apply.

## Compatibility history: 0.1.2

Blanket checks for installed AutoSaber, Replay, Noodle Extensions, Mapping Extensions and multiplayer plugins were removed. [AutoSaber-Quest](https://github.com/BinitDOX/AutoSaber-Quest) generates maps; treating it as autoplay was incorrect. Ranking checks remain separate.

[Replay 0.5.2](https://github.com/Metalit/Replay/tree/c3c2eac70e9fa45a25c0b765964a62dc46a3a4ac), src/manager.cpp, sets MetaCore disabler replay on StartReplay and removes it on stop. Only active playback is checked, not libreplay.so presence.

[MappingExtensions](https://github.com/rcelyte/MappingExtensions/tree/eafc097b355a30021d1491c5368187938aafe012) activates transformations for the selected difficulty's requirements. Library presence does not affect every song. Actual lane/type/rotation/NJS checks remained in 0.1.2.

MapCompatibility reads IBeatmapLevelData.GetBeatmapString(ByRef<BeatmapKey>) and serialized v2/v3/v4 note customData. Movement/track/animation/fake/cut flags and player-space events were compared with [Noodle Extensions 1.6.2](https://github.com/StackDoubleFlow/NoodleExtensions/tree/565e3b33c98e9536d5b7e9c78d7f96e3ab2a4fad). In 0.1.2, rejection applied to the map, not the installed library; decorative colors/environment-only tracks were allowed, while note tracks/animations retained original scoring.

Parsing runs once at custom-level start without new SongCore/CustomJSONData dependencies. Unreadable serialized maps were refused in 0.1.x. libil2cpp 1.40.8 confirms FileSystemBeatmapLevelData.GetBeatmapString at 0x26ce27c resolves FileDifficultyBeatmap and calls its synchronous GetBeatmapString; this is a real API, not an assumed NotImplementedException. Live combinations still need device testing.
