# PREREG r10: D1 breadth, matched-pose in-race draw-list adjudication on the 10 capfix tracks

Written before the first standalone replay of this lane. Base `0006dbf2d` (race/first-frame-parity),
branch `work/r10`. Measurement only: no renderer, physics or AI code changes.

## Question

capfix (`verify/capfix_20261016/RESULT_capfix.md`) captured the original's in-race D3D9 draw lists for
the 10 tracks that d1r1 did not cover (Egypt, Neustein, Highway, Sands, SuperG, Roundabout, Storm,
Forest, Dump, Warzone), 3 shots each. Does the current default standalone, replayed at the same
camera and car poses, reach the d1r1 end state on all of them: every divergence row of
`re/tools/drawlist_diff.py` 3D mode carries a citation whose cause holds for that track?

## Method (identical to d1r1, nothing re-tuned)

- Standalone: clean build of the base in this worktree (`mashedmod\build\mashed_re.exe`, SHA-256 recorded in the RESULT).
- Replay: `verify/d1r1_20261016/run_replay3d.py` (one deterministic round, `MASHED_CAM_REPLAY_BASIS=1`,
  `MASHED_CAR_REPLAY=1`, `MASHED_DBG_D3DREC=1`), default build, no opt-in knobs, `--shots` = the 3 shot rows of
  each capfix `summary.json`, `--frames` = last shot row + 350.
- Score: `drawlist_diff.py A B --trace trace.csv --cite cites_r10.json`, A = capfix `orig_row_<K>.draws.json`,
  B = the replay's `stand_row_<K>.draws.json`.
- `cites_r10.json` = the de-duplicated union of `cites_d1r1/rstates/rfx/rfx2/rprops.json` (all merged at the base).
  No entry is added before the first scoring run. Entries added afterwards are listed in the RESULT with the
  evidence behind each.

## Gates (a track is scored only if all pass)

Same as d1r1 PREREG section 2, per frame: I-COVER, I-FRAME, I-NAME (original side, from the capfix records),
I-SYNC (actual_row == K, recorder armed, camera src 4, max deviation from `M_*` <= 1e-5) and I-NAME
(standalone side). A frame that fails a gate is re-run (up to 3 attempts, piz-collision crashes are re-runs per
CLAUDE.md U-9677) and is reported as UNSCORED if it never passes. No gate is relaxed.

## Verdict rule

- Per track and frame: matched draws, matched triangles, rows, CITED rows, GREEN (no rows), RED rows.
- A row counts as CITED only if its cite id is in the merged file AND the cause is shown to hold on that track.
  Generic causes (CULL by frustum test, STATE/FOG/ORDER, car LOD, shadow/particle/FX/weapon texture names) hold by
  construction of the row. Cites keyed on a track-specific texture list (C10, C11, C12, C13, C14, C16) are
  checked row by row on the new tracks: a name match alone is reported as "name-only", not as cause-verified.
- U-9702 closes only if all 10 tracks x 3 frames are scored, every RED row has a cause group, and every cause
  group that is not an existing d1r1 group has a U-row unless it is fully explained by cited evidence.
- Otherwise U-9702 stays open and the RESULT lists exactly which frames or rows block it.

## Predictions (written before the run)

- Pr-1: most rows fall into existing d1r1 groups (CULL, STATE-LIBRW, ORDER, FOG, CARLOD, SHADOW, PARTICLE, PICKUP,
  STARTDISP, FX). Some RED rows remain on at least 3 of the 10 tracks because the cite file was calibrated on
  TRAINING, Arctic and City only (track-specific props, animated textures, per-track FX).
- Pr-2: the 10 tracks show draw-count-matched fractions in the same range as d1r1 (77 to 91% triangles).
- Pr-3: no gate failure beyond occasional piz-collision re-runs.
