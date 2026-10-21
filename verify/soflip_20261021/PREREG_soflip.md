# PREREG soflip: MASHED_SUBSTEP_OUTER default ON, u9778's fiber form deleted (2026-10-21)

Branch `work/soflip` from `race/first-frame-parity` `fa50143b3` (work/substep merged). Model claude-opus-5-5, account2. Builds with
`MASHED_NO_DEPLOY=1`, private roots `D:\MashedWT\soflip_root` (arms) and `D:\MashedWT\soflip_root13` (13 tracks), evidence in
`D:\MashedWT\soflip_data` (`MOVED_OFF_GIT.txt`). Written after the deletion commit (`59b2916df`) and before any flip arm.

## Mariano's decision

Mariano's decision (2026-10-21), on substep's section 6 (`verify/substep_20261021/RESULT_substep.md`):

1. Flip `MASHED_SUBSTEP_OUTER` to default ON.
2. Delete the u9778 fiber form (`Vehicle/SubstepInterleave.cpp`, `MASHED_SUBSTEP_INTERLEAVE`, `IlYield`, `g_ilScratch`).

Out of scope: U-9780 / U-9781 / U-9782, and any snapshot / restore redesign.

## 1. Deletion (own commit, before this PREREG)

**Deleted.** `Vehicle/SubstepInterleave.cpp` and its `exe_sources.rsp` line (the fibers, `MASHED_SUBSTEP_INTERLEAVE`,
`MASHED_SUBSTEP_INTERLEAVE_ORDER`, `VehiclePhysics_InterleaveEnabled / InterleaveRun / IlInFiber / IlSwitchToMain`). In
`VehiclePhysicsRun.cpp`: `g_ilScratch[16]`, `IlYield` and its call in `StepCarImpl`'s substep loop, the snapshot's `scratch` field
(it saved `g_ilScratch`, written only inside a fiber). In `TrackRenderer.cpp`: the `InterleaveEnabled()` arms of the A4
pre-phase test and of the player's step site, and the `InterleaveRun` call.

**Kept, renamed.** What the outer driver uses: the snapshot / restore (`VehiclePhysics_IlSnapshot / IlRestore` ->
`VehiclePhysics_OuterSnapshot / OuterRestore`, `IlSnap` -> `OuterSnap`), with the per-slot `g_cSlot` scratch in it; the AI-car
handoff in `TrackRenderer.cpp` (`IlCar` / `s_il` / `IlSameInput` / `s_ilMismatch` -> `OuterCar` / `s_outer` / `OuterSameInput` /
`s_outerMismatch`, `ilUndo` -> `outerUndo`); the io-mismatch log line, now tagged `[substep] outer: slot %d io differs ...`.

**`MASHED_SUBSTEP_OUTER_ORDER=fiber`: kept.** Read from the code (`VehiclePhysicsRun.cpp`, `OuterRunToYield` /
`VehiclePhysics_StepCarsOuter`): it drives the same phase functions (`StepBegin`, `ChunkDue`, `StepChunkHead`, `SubstepDue`,
`StepSubstep`, `StepChunkTail`, `StepEnd`) in the fiber's yield order with a per-car stage counter. It calls no fiber API and none of
the deleted code, so it stays as a control. `ORDER=car` stays too.

**Proof** (exe `683956fd`, `D:\MashedWT\soflip_data\del`, `RUNLOG.txt`):

| arm | measured | expected |
|---|---|---|
| B_DEF K0 / K1 / K26 (knob off) | `7ab70c5f / 15262e4a / d1b4bc64` | `7ab70c5f / 15262e4a / d1b4bc64` |
| B_SX K0 / K1 / K26 (`MASHED_SUBSTEP_OUTER=1`) | `9bd86ffe / 433abcd2 / 1a1ff727` | `9bd86ffe / 433abcd2 / 1a1ff727` |
| B_SXF K0 (`ORDER=fiber`) | `9bd86ffe` | `9bd86ffe` |
| B_SXC K0 (`ORDER=car`) | `7ab70c5f` | `7ab70c5f` |
| H_DEF (knob off) | `ce087436 / b0a2973b / c4f7a000` | `ce087436 / b0a2973b / c4f7a000` |
| H_SX (`MASHED_SUBSTEP_OUTER=1`) | `f78de789 / 43bbe0fb / 161b1a6a` | `f78de789 / 43bbe0fb / 161b1a6a` |

rva-lint (`scripts/lint_rva_bodies.py`, also run by `build.bat`): 25 findings, all allowlisted, NEW=0.

## 2. Change

Exe `50558a73`. `Core/FaithDefaults.cpp` seeds `SUBSTEP_OUTER` (value 1) after `MODE_LIVE` / `SLOT_TYPE_LIVE`, same mechanism: opt-out
`MASHED_NO_SUBSTEP_OUTER=1`, `MASHED_NO_FAITH=1` seeds nothing, an explicit `MASHED_SUBSTEP_OUTER=0` still wins (the read site
`VehiclePhysics_OuterEnabled` takes `e[0] == '1'` as ON). Standalone only.

The outer driver steps only the AI cars the A4 pre-phase stepped (`s_outer[v].have` is set inside `VehiclePhysics_A4OrderEnabled()`),
so it needs the seeded `A4_ORDER`. With no such car (`MASHED_NO_A4_ORDER=1`, `MASHED_MEASURE_NOOPP`, or no AI tick) `outerN == 1`
and the player takes the per-car `StepPlayer` path, the old default.

No runner change: `run_arms.py` here is the substep copy with the `IL` stacks removed and `NOSX` (`MASHED_NO_SUBSTEP_OUTER=1`) added.

## 3. Predictions

From substep (exe `e856ca97`, the `SX` column) and fflip. `H_` G-CTRL, `B_` (b) dumps, `D2_` D2, on recipe F.

| gate | arm | prediction |
|---|---|---|
| G-CTRL x2 | `H_DEF`, `H_DEF-2` | `f78de789 / 43bbe0fb / 161b1a6a`, both equal |
| G-OPT | `H_NOSX` (`MASHED_NO_SUBSTEP_OUTER=1`) | the fflip default `ce087436 / b0a2973b / c4f7a000`. **Must hold.** |
| G-NOFAITH | `H_NF` (`MASHED_NO_FAITH=1`) | fflip's NO_FAITH `997f4117 / e6288905 / a63bdcfa`. Expectation: NO_FAITH seeds neither `SUBSTEP_OUTER` nor `A4_ORDER`, so the outer driver never runs, and the deletion left the knob-off path byte-equal (section 1) |
| G-B | `B_DEF_K0/K1/K26` | `9bd86ffe / 433abcd2 / 1a1ff727`; flag_a368 0 / 0,1 / 0,2 |
| G-B-OPT | `B_NOSX_K0/K1/K26` | `7ab70c5f / 15262e4a / d1b4bc64`; flag_a368 0 / 0,1 / 0,2 |
| (b) | `vsum.py` on `B_DEF_*` | A 3/3, B 3/3, C 3/3 (9/9). Opt-out: A 3/3, B 2/3 (car 2 FAIL), C 3/3 |
| io mismatch | `[substep] outer: ... io differs` lines in the root's `mashed_re.log` per arm | 0 on B x3 and H |
| G-D2 | `D2_DEF_1..3`, alone | `score_d2_v2.py port`, whole log, 8-run band: PASS 3/3 (substep SX: slip1500 0.1900 x3, slip2000 0.2550 x3, drvmed 1965.35 / 1965.67 / 1965.35, H1 398/400) |
| G-13 | `run13.py run` on recipe F, `score` against `verify/rres_20261016/cites_rres.json` | 39 frames, 877 rows, all cited, 0 RED, instrument gates PASS (= substep `t13_SX`) |

Default-ON rule: no car passing on the fflip default (A cars 1, 2, 3; B cars 1, 3; C cars 1, 2, 3) may fail, D2 3/3, 13 tracks 0
RED, and G-OPT must reproduce the fflip default. If any gate fails, stop and report; the flip is not committed as a pass.

arms/ and tracks/ live in D:/MashedWT/soflip_data (written there directly via A4_OUT / --out), never committed.
