# PREREG D4-WSB-C4-BROAD-WS: write-set C4 for 5 BROAD rows

Committed alone, before any arm. Scope and method: `verify/wsbc4scope_20261027/RESULT_D4-WSB-C4-SCOPE.md`
(group BROAD-WS), tools from `verify/wsbbvh_20261027/` (D4-WSB-BVH-WRITESET, merged).

Rows: `0x0055a1f0`, `0x0055ab30`, `0x0055bae0`, `0x0055c000`, `0x0055c0f0` (all C3 in `hooks.csv`).

## Harness change (commit `879e1ad65`, before this file)

- `0x0055a1f0`, `0x0055bae0` (`RwpSolverBroadphase3.cpp`) and `0x0055ab30` (`RwpSolverGlue5.cpp`): wrapper
  `ShadowAB::Run` -> `ShadowAB::RunTracked` (page write tracking + caller 4 KB stack window, pre-state restored).
- `0x0055c000` (`RwpIntegrator.cpp`): body renamed `FUN_0055c000_impl` (static), new `extern "C" FUN_0055c000`
  wrapper calling `ShadowAB::RunTracked`, phase gate `kPhaseRace`. The row had no shadow wrapper.
- `0x0055c0f0`: already `RunTracked`, unchanged.

Unarmed every wrapper calls only `_impl`. No default, knob or behaviour change.

## Pre-condition

`D4-WSB-NARROW-FLIP`'s merge commit is an ancestor of this branch (`git merge-base --is-ancestor`) before any gate,
shadow or racediff arm. The exe and `.asi` of every arm are built from that tree.

## Leg 1: default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT` = this step's private root,
under the test slot and `mashed_lock.py`. Pass = every sha8 equals `re/orchestrator/merge_train.json`
`gctrl.ref` / `b.ref`. Any difference: stop, no runtime leg is scored, verdict DECISION.

## Leg 2: write-set shadow (one hook per boot, hook installed alone)

`run_shadow.py <rva>` (copy of the BVH-WRITESET driver, label and data path changed): census scenario (track 0,
mode 10, 4 cars, 60 s hold), `--hooks 0x<rva>` (MASHED_HOOK_ONLY), `MASHED_SHADOW_AB=1`.

- CLEAN = n >= 1, 0 divergent samples, `PATCHBYTE installed=e9`, `A/B-IS-REAL`.
- Any divergent sample = the row stays C3 (a diagnostic re-run may be reported, it does not change the verdict).
- NO_LOG or attach failure: re-run once.
- n = 0 (not observed): for `0x0055bae0` only (census NOT-EXEC), one more shadow boot each on the Quick Battle
  scenario of `c4_racediff_v6.py`. Still n = 0: NOT-OBSERVED, the row stays C3.

## Leg 3: racediff, tool v6 (`verify/wsbbvh_20261027/c4_racediff_v6.py`, unmodified)

Driver `run_bvh2.py` copy, one OFF boot + one ON boot per row, Quick Battle window (default `--lo 300 --hi 1200`).
Pass = verdict `C4-CLEAN` with `installed=True`. Both sides empty: the driver re-runs once. If Quick Battle records
0 calls on both sides after the re-run, census racediff (`C4_SCENARIO=census C4_WINDOW="1 3000"`), 2 attempts.
No result: NOT RUN, the row stays C3. Any other verdict: the row stays C3.

## Promotion

A row goes C3 -> C4 only when legs 2 and 3 are both clean on this branch's build and leg 1 passed. Through the
re-classify rules (no unfiled `[UNCERTAIN]` in the row's note, no stub in its body), one transaction for all
promoted rows. No other row changes level.
