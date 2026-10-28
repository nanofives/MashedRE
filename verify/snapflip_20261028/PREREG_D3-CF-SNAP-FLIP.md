# PREREG D3-CF-SNAP-FLIP (L-43 step 2, 2026-10-28)

Decision L-43 (`log/orch/DECISIONS.md`, Mariano 2026-10-27): run D2 x3 with `MASHED_SNAP_POST=1`; if D2 stays in band, flip
`MASHED_SNAP_POST` default ON as its own decided-flip pass, B K1 ref `e85ea6fb -> 9fcdbd3a` refreshed in the same step.
Evidence for the knob: `verify/snap_20261027/RESULT_D3-CF-SNAP.md`.

Branch `orch/D3-CF-SNAP-FLIP` off `orch/integration` `852369807`. Standalone only (no original, no Frida).

## Arms (runner `verify/snap_20261027/run_arms.py`, private root `D:/MashedWT/D3-CF-SNAP-FLIP_root`)

1. Knob tree (default OFF, exe from `852369807`): `D2_SP_1 D2_SP_2 D2_SP_3`, each run alone under
   `mashed_lock.py run --solo`, no merge train live. An arm under 1600 `motion_diag` lines is re-run (at most 2 times).
   Scored with `verify/d2rebase_20261019/score_d2_v2.py port` against `band.json` (slip1500 [0.18396, 0.20586],
   slip2000 [0.24555, 0.25557], drvmed [1893.63, 1997.46]).
2. Only if all three are in band: flip tree (default ON, `MASHED_NO_SNAP_POST=1` opts out). Arms `H_DEF B_DEF_K0 B_DEF_K1
   B_DEF_K26` (new default), `H_SL B_SL_K0 B_SL_K1 B_SL_K26` (default + `MASHED_SNAP_LOG=1`, the restore / mismatch count), and
   the opt-out control `H_NOSP B_NOSP_K0 B_NOSP_K1 B_NOSP_K26` (`MASHED_NO_SNAP_POST=1`).

## Predictions

- P1: D2 with the knob ON is in band on all three metrics, 3/3. Structural reason (`RESULT_D3-CF-SNAP.md` section 3): D2 arms
  set `MASHED_MEASURE_SOLO=1`, so `ai_cars_` is empty and the opponent loop the knob changes never runs; the knob ON should score
  inside the D2 default's run-to-run spread.
- P2 (flip tree, default): G-CTRL `5943c5f1 / 364350ae / 52052890` (unchanged), B `3a03670c / 9fcdbd3a / 53778664`
  (K1 moves, K0 / K26 unchanged). No other hash moves.
- P3 (flip tree, `SL`): 0 `[snap] dead / spin / reloc / mismatch` restore lines and 0 `[substep] outer` mismatch lines on all
  four arms; `reloc-post` 2 on K1 (frames 331 / 395), 0 elsewhere.
- P4 (flip tree, opt-out): `MASHED_NO_SNAP_POST=1` gives the pre-flip refs G-CTRL `5943c5f1 / 364350ae / 52052890`,
  B `3a03670c / e85ea6fb / 53778664`.

## Verdicts

GREEN: P1 and P2..P4 hold (expected new refs reported for the decided-flip merge). DECISION: any D2 arm out of band after its
re-runs (no flip; scores, band, options). RED: P2 or P4 fails (a hash other than B K1 moves, or the opt-out does not restore
the old refs). `OuterSnapshot` / `OuterRestore` are not retired in this step.
