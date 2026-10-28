# PREREG D3-CF-SNAP-RETIRE: retire OuterSnapshot / OuterRestore (L-43 step 3, 2026-10-28)

Branch `orch/D3-CF-SNAP-RETIRE` from `orch/integration` `cdbbbfe0b` (HB-FULLCHECK-R17 merged, run13_reference = verify/hbfullcheck_20261028/tracks_score.txt). Committed alone, before the code.

## 0. Condition (L-43 answer, `log/orch/DECISIONS.md`; L-46 = a)

L-43: retire `OuterSnapshot` / `OuterRestore` once the mismatch count stays 0 with `MASHED_SNAP_POST` default ON.
P0 (measuring leg, before the retirement is accepted): exe `89f91d07` built from orch/integration `d92bb3a5e` (no
`mashedmod/` change to `cdbbbfe0b`), no knob, `MASHED_SNAP_LOG=1`, on G-CTRL (`H_SL`), B K0 / K1 / K26 (`B_SL_K*`) and the 13
tracks. Prediction: 0 `[substep] outer` mismatch lines and 0 `[snap]` restore lines (`dead`, `spin`, `reloc`, `mismatch`) on
every arm, `reloc-post` only. Any nonzero count: no retirement, RED / DECISION with the count.

## 1. Why the default path never reaches the restore (static, `D3d9Render/TrackRenderer.cpp` at the base)

- `dead` / `spin` (`outerUndo`, 5763 / 5767): skipped when `snapKeep` (5759: SNAP_POST and `s_outer[v].have`). When
  `have` is false, `outerUndo` is a no-op.
- `reloc`: under SNAP_POST `stuckReloc` runs at 5891, after the commit, where `have` has already been cleared (5853 / 5849).
- `mismatch` (5832): the only remaining caller. Fires when the opponent loop's io differs from the pre-phase io.
- `!phys` (`MASHED_REAL_PHYSICS=0`, the kinematic scaffold, A/B only): `StepCarsOuter` (VehiclePhysicsRun.cpp 2644) does
  not check the physics, so `have` can be set. `snapPost` is false there (5758), so the old code restored at `dead` / `spin`
  / `reloc` (pre-commit) and never at the other cars (they `continue` into `AiOptionBStep` at 5812). The retirement keeps
  the `!phys` skip and relocation order and loses only those restores, on a non-default path no gate runs.

## 2. Change

1. Delete `VehiclePhysics_OuterSnapshot` / `VehiclePhysics_OuterRestore` and `OuterSnap` (`Vehicle/VehiclePhysicsRun.cpp`,
   `.h`) and the snapshot loop (TrackRenderer 5451).
2. Delete the `MASHED_SNAP_POST` knob and its opt-outs (`MASHED_NO_SNAP_POST=1`, `MASHED_SNAP_POST=0`). The post-commit
   relocation is the only form. The pre-flip timing needs the restore, so the opt-out cannot survive the retirement.
   `!phys` keeps the pre-commit relocation, as it had with the knob ON (5758: `snapPost = phys && SnapPost()`), and
   `snapKeep` keeps its `phys &&` term.
3. Mismatch net: no restore is possible. The car keeps the outer driver's result and the `[substep] outer` line is still
   written (text "kept the outer result"), plus `[snap] mismatch` under `MASHED_SNAP_LOG=1`. On every measured arm this branch
   is not reached.
4. `MASHED_SNAP_LOG` stays (diagnostic, sites `reloc-post` and `mismatch`).

## 3. Predictions (default exe, no knob)

- P1: G-CTRL = `merge_train.json` `gctrl.ref` and B = `b.ref` (6 hashes unchanged).
- P2: 13-track table identical to `verify/hbfullcheck_20261028/tracks_score.txt` (0 differing rows).
- P3: `MASHED_SNAP_LOG=1` arms log 0 `mismatch` lines and the same `reloc-post` (frame, slot) pairs as the measuring leg.

## 4. Verdict

GREEN if P1, P2, P3 hold: enqueue. Any hash or row moves: not gate-neutral, DECISION with before / after, no enqueue.
