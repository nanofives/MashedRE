# PREREG D3-CF-RFLAME: the R_FLAME 2-query residue and FUN_0045ac40

Committed alone, before any arm below runs. Code under test: `55138534c` on `orch/D3-CF-RFLAME`.

## 1. Question

`ROADMAP.md` §D3 CLOSED carries one powerup residue: on capture `g3`
(`verify/d3_contact_20260928/g3.msd`) the R_FLAME query site `0x45afcc` counts 546 original
vs 548 port, lerp `0x45aff3` and basis `0x45b04c` exact. `re/analysis/D3_CONTACT_PORT_2026-09-28.md`
§4.2d names the cause as the unported per-group sort `FUN_0045ac40`.

`FUN_0045ac40` is now ported (`RFlame_SortFive`, `Powerup/PowerupEffects.cpp`) behind
`MASHED_RFLAME_SORT=1`, default OFF. This PREREG fixes how the port is measured.

## 2. What the existing g3 capture cannot test

The sort orders a group's records by squared distance from the reference
`FUN_004671d0(0)` (CALL `0x0045af35`). `g3` records neither the reference nor any spark
position. The port's sparks also sit at stand-in positions (no jitter, no car matrix), so a
sort run on them orders nothing the original ordered. So on `g3` itself the knob can only be
shown to be a no-op: with no `.puflame.csv` the backend gives no reference and the sort is
skipped. That is arm `G3_ON`, and it is a control, not the re-score.

## 3. New capture

`G3F`: the `g3` argv from `verify/d3_contact_20260928/g3.msd.provenance.json`, plus
`--puhook-flame`, written to `verify/d3rflame_20261024/g3f.msd`, original side, private root
`D:\MashedWT\D3-CF-RFLAME_root`, under `mashed_lock.py run --label D3-CF-RFLAME`. A piz-collision
crash (`MASHED.exe+0x1509b4` / `+0xe66db`) is re-run, not reported. If the agent reports
`refMoved > 0` the reference is not one value per tick and the injected reference is reported
per tick-entry value with that count.

Round-level outcomes do not reproduce across runs, so `g3f` is a new capture with its own
query counts, not `g3` again.

## 4. Arms (all on `pu_replay.exe` built from `55138534c`, slot 0)

| arm | input | env |
|---|---|---|
| `G3_OFF` | `g3` | knob unset |
| `G3_ON` | `g3` | `MASHED_RFLAME_SORT=1` |
| `F_OFF` | `g3f` | knob unset, births + reference injected |
| `F_ON` | `g3f` | `MASHED_RFLAME_SORT=1`, births + reference injected |
| `F_ON_NB` | `g3f` | `MASHED_RFLAME_SORT=1`, `MASHED_PU_FLAME_NOBIRTH=1` (stand-in geometry, control) |
| `SW_ON` | the 12 `sweep.ps1` captures | `MASHED_RFLAME_SORT=1` |
| `GATES` | `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this worktree's `mashed_re.exe` | knob unset |

Every `g3f` arm also prints the SORT CHECK: the ported sort run on the original's tick-entry
records (with that tick's births overlaid) against the original's tick-exit slot order,
identity = dword 10 (the emitter's angle). Controls in the same table: no sort, a key
recomputed after each swap, nearest-first.

## 5. Pass criteria

- **P1 (sort fidelity).** SORT CHECK tested groups >= 1, port mismatches = 0, and the no-sort
  control mismatches > 0 (the original moved at least one group, so the check is not vacuous).
  Tested = 0 is NOT TESTED, never a pass.
- **P2 (residue).** `F_ON`: the three R_FLAME rows `clean` (orig = port, 0 port-only,
  0 orig-only). `F_OFF` is reported beside it. If `F_OFF` is also clean, the residue did not
  recur on `g3f` and P2 is recorded as non-discriminating.
- **P3 (no default change).** `G3_OFF` = `G3_ON` = 546 / 548 with lerp / basis 23 / 23.
  `SW_ON` decision and contact verdicts equal the knob-unset sweep (12/12 decision CLEAN,
  contact CLEAN except g3).
- **P4 (gates).** G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6`, B `f9deb9b9 / d3601fcc / 9da9a26a`.

Verdict: GREEN = P1 + P3 + P4 hold and P2 holds or is non-discriminating (stated as such).
RED = P1 port mismatches > 0, or P2 fails with `F_ON`, or P3 / P4 fail. No default flip is
proposed by this step either way: the flip is a decision row.
