# RhythmSaber 0.2.0 — historical release notes

Strict windows +/-15/30/50/75/100 ms award 115/80/40/10/0 points in this release. Old default windows migrate once; custom windows survive. FlyingScoreEffect is enlarged 1.6 times and displays grade, points and offset for at least 0.9 seconds. The local HitScoreVisualizer-master implementation was studied; its source is not redistributed.

The frame becomes one white energy beam with alpha 0.75, soft transparent edges and subtle ripples. No posts, second rail, collider or bloom. Distance/height survive; width and judgement size are added.

Blanket refusals for Noodle/Mapping data, Arc/Chain, Cinema/Vivify/Chroma, rotations, lane counts and dynamic NJS are removed. All native scored types 1–8 use their own ScoreModel maximum and proportionally rounded timing points. GoodCutScoringElement adds no fixedCutScore. Ignore/NoScore are not scored; physical rules and multipliers remain native.

## Modchart measurement limits

Linear motion uses analytic expected time. Modified motion uses world positions after NoteController.ManualUpdate and signed distance to the visible beam plane in player-origin space. INoteMovementProvider supports GameNoteController and BurstSliderGameNoteController.

Observed crossings are interpolated. Early cuts predict from latest velocity and display `~`; arbitrary future Noodle/Vivify motion may invalidate the prediction. Missing history, a stationary note away from the plane, teleport >2 m/frame or unmeasurable passage yields zero. Repeated crossings choose the nearest observed/predicted time. Swing/center scoring never returns mid-song.

Schema 2 records strict-1 policy, observedMotion, estimatedCuts and unmeasurableCuts. Shader-only deformation without collider movement does not change physical timing. Sub-frame TOI is not implemented. Solo/Party are supported; multiplayer/mission retain original scoring.

## Validation and installation

147 host checks passed. ARM64 build: logs/build-020.log; packaging: logs/package-020.log. Papercut/Bones copies select analytic, Breezer selects observed. This validates code/data, not live gameplay. The agent did not install 0.2.0 on Quest; no device was connected for the final build.

Import dist/RhythmSaber-0.2.0.qmod through QuestPatcher/MBF and fully restart. The previous 0.1.3 is preserved in dist and backups/0.1.3-live-confirmed. Config migrations do not automatically reverse on downgrade.

Submission protection is unchanged. Quest BeatLeader/ScoreSaber still require disabling because of audited unguarded retry/live-stream paths. Installed Replay is allowed; active playback does not enable altered scoring.
