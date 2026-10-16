# RESULT d2posmodel: fix B (the exe's car position model made the original's)

Pre-registration `PREREG_d2posmodel.md` (`b9330cb6`), amendments 1-3 (`66b57ade`, `67f31f2e`, `787ce166`). Branch
`work/d2posmodel`, base `232f9f76`. Model claude-opus-5-5 (account2). Exe runs used `MASHED_ROOT` = a private copy of
`original/` (two copies, so deterministic arms could run next to the wall-clock D2 arms). Exes `90a55dc7`, then
`06642c53` (adds the amendment-2 diag knobs only; its full-reset path reproduces `90a55dc7`, section 6).

## 0. Verdict first

> **PARTIAL. Fix B is ported behind default-OFF knobs. The settle it was written for now happens in the exe, but
> fix B fails D2. Default build unchanged: G-CTRL `c2e4d60d / b50bd39b / ba90ed88`.**
>
> 1. **The settle works.** With `MASHED_POS_MODEL=1` on the original's grid (`D_BRG`), the player's first-step move is
>    (0, -0.00295204, +0.0000922714) against the original's (0, -0.0029474, +0.0000921): x bit-identical, dz +1.2e-7,
>    dy -4.6e-6. The spawn y convention is the original's (`FUN_004704c0` / `FUN_0046d400` decomp, ground under the
>    car centre): physics spawn y 0.437426984 vs 0.4374268055. G-PROG (amended reading) reads 29.999977 on all 23
>    parked rows, where the base reads 0.0.
> 2. **G-SETTLE as registered FAILS, narrowly.** It was registered on the default grid (`D_B`): dz +3.6e-7 (tolerance
>    2.4e-7), dy -1.33e-5 (tolerance 1e-5). The default grid puts the car at a different pose (yaw -1.576028 vs
>    -1.568874), not on a different model. Not re-thresholded.
> 3. **D2 FAILS 3/3, recovery H1 FAILS 3/3.** `D2_B_1..3`: both slips UNSCORABLE, driving-median 1338.62 (n=54);
>    recovery 11/400, median 32.0. The car launches, hits the x = -2.04 wall at f~95 with the same first fixup as
>    the default build, and stays pinned on it (215 fixups vs 31).
> 4. **Within fix B, part 1 (the seed itself) triggers the pin.** P1 alone collapses identically. P2 alone equals
>    the default. Parts 2 + 3 without the seed (P23) keep both slips in bounds and move driving-median +15.5. The
>    bisect: only restoring the wheel block `+0x194..+0x458` after `FUN_0046baa0` un-collapses the arm, and the
>    minimal seed (states 1, fv 0, gc 4.0) collapses too. Under the seed, `+0xb14` engages at d=14 where the
>    original and the default engage at d=15, and the launch fit goes from 0.19 % to 6.93 %. Why is NOT identified.
>    The trap is not seed-only: P23 + fix C pins as well, with d=15.
> 5. **Fix C re-tested inside fix B: still OUT.** B + C 0.2009 / 0.2635 / 1818.04 (3/3); P23 + C 0.1970 / 0.2654 /
>    1667.87. Fix C stays opt-in.
>
> **U-9180: driving-median does NOT move into its bound in any arm.** Best: P23 1875.84 (default 1860.35), still
> 1.5 % under 1904.70.

| gate | arm | result |
|---|---|---|
| G-SETTLE (registered, default grid) | `D_B`, `D_B-2`, `D_BMIN` | **FAIL** dz +3.6e-7 / dy -1.33e-5, x PASS |
| G-SETTLE (report, original grid) | `D_BRG`, `D_BMINRG` | PASS dz +1.2e-7 / dy -4.6e-6, x bit-identical |
| G-SPAWNY (report) | `D_BRG` | 0.437426984 vs 0.4374268055, +1.8e-7 (pred. 1e-6: PASS) |
| G-PROG as registered (last row) | `D_BRG` | **FAIL**, mis-registered reading point (player drives off from f=24) |
| G-PROG amendment 1 (parked rows) | `D_BRG` / `D_RG` | **PASS** 29.999977 x23 / control 0.0 x27 |
| D2 three metrics, 3/3 | `D2_B_1..3` | **FAIL** NA / NA / 1338.62 (n 0 / 0 / 54) |
| recovery H1, 3/3 | `D2_B_1..3` | **FAIL** 11/400 (2.8 %), median 32.0 |
| D3 (b) report | `B_B_K*` | A **0/3** (car 2 lost), B 2/3, C 0/3 (base 1/3, 2/3, 0/3) |
| collateral (<= base + 2) | `H_B` / `H_B-2` | PASS 40 / 94 (base `H_DEF` 39 / 94; `s.v1.c1` moved out) |
| no-op control | `H_DEF`, `H_DEF-2`, `H_DEF-3` | PASS `c2e4d60d / b50bd39b / ba90ed88` (see section 6 for `-3`) |
| determinism | `H_B` = `H_B-2`; `H_BMIN` = `H_BMIN-2` | PASS `b1b6f319 / 4ea8366f / e92bdf83` |

Default rule (PREREG section 5): G-SETTLE, D2 and recovery fail, so **fix B stays default OFF**. `BMIN` was
ineligible by registration and also collapses. **No default flip.**

## 1. What was ported (all behind `Vehicle/PosModelKnobs.h`, value `1` = on)

- **Part 1, `MASHED_POS_SEED`:** `VehiclePhysicsStateReset(slot)` (the C3 port of `FUN_0046baa0`) right before the spawn-
  settle step, the exe's only pre-GO physics tick. The original runs it at the tail of `FUN_004704c0` (after its
  `0x0046f6c0(0)` at `0x00470638`), in `FUN_0040e590` at `0x0040ed6e`, and on every hold tick (`0x0041033e`,
  `0x0041043a`). STATED: one seeded tick in the exe vs N in the original; round restarts reset nothing (existing gap).
- **Part 2, `MASHED_POS_WHEELS`:** the `MASHED_RW_WHEELPOS` arm of `WheelContactSolver.cpp` (the four `g_suspScratch`
  points through the ring into `DAT_0088e620`), with the current body basis copied into the ring's rows first.
- **Part 3, `MASHED_POS_TRANS`:** `io.pos` is the physics position inside `StepCar`. Adapter IN keeps it when the
  caller round-trips bit for bit, else re-seeds (x, pos[1] - `ground_off`, z). The substep integrates y as well
  (`0x0046ea5f / 0x0046ea69 / 0x0046ea73`). The ring centre is `io.pos`, and the solver's corrected centre goes back into
  `io.pos`. The settle step's position is kept. `TrackRenderer` (player and AI) takes `io.pos` instead of
  `pos + drive_delta` with a GroundHeight-snapped y. No spawn code changed: `car_pos_[1] - car_ground_off_` already IS
  the original's spawn convention (PREREG 0.2).
- Diag only: `SOLVER_IN` / `goff` / `physy0` on `MASHED_SETTLE_DIAG`; `MASHED_POS_SEED_MIN`, `MASHED_POS_SEED_KEEP`.

## 2. The settle, per part (`G_SETTLE.txt`, `D_*.settle.txt`)

| arm | settle postA4 vel | solver call 1 / 2 | first-step move (y, z) |
|---|---|---|---|
| `D_DEF` | (0, -216.668, 0), `+0xb20` 1, loads 0 | stand-in fv -0.04998, +0.04995·n discarded | 0, 0 |
| `D_P1` | **(0, 0, 0), `+0xb20` 0, loads 1083.34192 x3 / 541.670959** | as `D_DEF` | 0, 0 |
| `D_P2` | as `D_DEF` | real fv +0.0096 / -0.0097 | 0, 0 |
| `D_P3` | as `D_DEF` | stand-in, kept | +0.1398, -0.00468 (pred. 4.3 PASS) |
| `D_P23` | as `D_DEF` | y integral -0.0100, then +0.00706·n | -0.00297, **-0.00022** (pred. 4.4 half missed) |
| `D_B` | (0, 0, 0) | call 1 none, call 2 4->3 drop | **-0.00296074, +0.0000925063** |
| `D_BRG` | (0, 0, 0) | call 1 none, call 2 4->3 drop | **-0.00295204, +0.0000922714** |

The seed reaches A5 (A4 sees states 1: loads become the original's 1083.34 / 541.671 and A6a takes its parked full
stop). It does NOT reach solver call 1: every arm's `SOLVER_IN` for the settle's first call reads st (0,0,0,0), fv
0, gc 0. The A4-first `ReassertContacts(r)` runs before this car's first `SolveWheelContacts` and uses `g_cGrounded`,
one global written by whichever car's solver ran last (initial false for slot 0). So it zeroes the seeded states.
Call 1 then yields (0,0,2,2), `ReassertContacts` promotes them to (1,1,2,2), and the 4 -> 3 drop translation lands on
call 2. The end-of-frame position is still D0-e's.

## 3. Why D2 collapses (`D2_BX_1` / `D2_DX_1` diagnostics, bisect, `SCORE_D2.txt`, `SCORE_LAUNCH.txt`)

- Both builds hit the x = -2.04 wall near f 95. The first fixup is the same on both (vel (-1696, 0, 680) ->
  (189, -78, 143), slots s5 / s9, normal (1, 0, 0)). The default slides off (31 fixups, sp 1952 at f 300). Under B
  the car stays side-on to the wall (contact arm x -1.80) at sp 10-50 for the rest of the run: 215 fixups, 0 off-mesh
  recoveries.
- Bisect (part 1 alone, one `FUN_0046baa0` write group restored each): G1, G2, G4-G8 collapse (1338.62 n=54).
  **G3, the wheel block, restored = 0.1927 / 0.2489 / 1865.87** (launch 0.19 %, recovery 398/400). The minimal seed
  collapses. So the trigger is the seeded wheel states / fv in the settle, not the rest of the reset.
- Launch: `+0xb14` engages at d=14 under every seeded arm (P1, B, BMIN, BC, P1MIN, P1KG1/2/4-8). It engages at
  d=15 on the original, the default, P2, P23, P23C and P1KG3. Best-lag error 6.93 % seeded vs 0.19 % default.
  **[UNCERTAIN]** which field of the seeded settle state brings the drive engagement one frame forward. Missing
  evidence: a per-frame diff of A6a's drive gate inputs (`p[-0xf]`, `p[-3]`, the 15-frame timer source) between
  `D2_P1KG3_1` and `D2_P1_1` over d = 0..16.
- **The pin is not specific to the seed.** `D2_P23C_1` (no seed, d=15, launch fit 0.04 %) pins too (recovery 9/400).
  d2settle's fix-C arms fail recovery too (its `D2_NOA3_1` and A + C `D2_DEF_1`, re-scored here with
  `a8_launch.py`: 9/400, median 21.6, `+0xb14` d=15). So the x = -2.04 wall traps the
  car for several perturbed trajectories, and the port's wall response (`0x00469aa0` / `0x0046ef70` chain) is part
  of the D2 problem. Which part is NOT identified. The original escapes on its own recipe (398/400). No original
  capture of a seeded-pose impact exists to say whether the original would escape this one.

## 4. Per-arm table (D2 bounds `d81a8df6`: 0.18855..0.19635 / 0.24488..0.25487 / 1904.70..1982.44)

| arm | slip 1500-2000 | slip 2000-2600 | driving-median | n | recovery | `+0xb14` d |
|---|---|---|---|---|---|---|
| `D2_DEF_1` (control) | 0.1927 in | 0.2489 in | 1860.35 OUT | 376 / 562 / 1366 | 398/400, 1362.8 | 15 |
| `D2_P1_1`, `_2` | NA | NA | 1338.62 | - / - / 54 | FAIL | 14 |
| `D2_P2_1` | 0.1927 in | 0.2489 in | 1860.27 OUT | 376 / 562 / 1368 | PASS | 15 |
| `D2_P23_1` | 0.1944 in | 0.2538 in | **1875.84** OUT | 360 / 566 / 1338 | 398/400, 1362.8 | 15 |
| `D2_B_1..3` | NA | NA | 1338.62 | - / - / 54 | 11/400, 32.0 | 14 |
| `D2_BMIN_1` | NA | NA | 1338.62 | - / - / 54 | FAIL | 14 |
| `D2_BC_1..3` | 0.2009 OUT | 0.2635 OUT | 1818.04 OUT | 121 / 186 / 454 | 11/400, 28.8 | 14 |
| `D2_P23C_1` | 0.1970 OUT | 0.2654 OUT | 1667.87 OUT | 134 / 184 / 530 | 9/400, 21.6 | 15 (fit 0.04 %) |
| `D2_P1KG3_1` | 0.1927 in | 0.2489 in | 1865.87 OUT | 376 / 562 / 1356 | PASS | 15 |

G-CTRL (`RUNLOG.txt`): `H_P1` `12083869 / e9bdb465 / 98dbc415`, `H_P2` `953b6193 / 77cff7f5 / ba90ed88` (player trace
untouched), `H_B` = `H_BMIN` `b1b6f319 / 4ea8366f / e92bdf83`, `H_BC` `b300e27a / 077e785b / 9fdc5307`.

## 5. Decisions needed (user)

1. **Fix B default.** It stays OFF by the registered rule. The position model (parts 2 + 3) does not regress D2 by
   itself (slips in, driving-median +15.5). The seed does: it makes the settle faithful and the launch one frame
   early. Recommendation: keep all of fix B OFF until the d=14 engagement is explained (section 3 [UNCERTAIN]). Do NOT
   ship P23 alone: it fails G-SETTLE (dz has the wrong sign) and it is the unseeded half of a mechanism.
2. **G-SETTLE's arm.** Registered on the default grid, where the spawn pose is not the original's. If the user
   accepts the original-grid arm (`D_BRG`) as the settle witness, G-SETTLE passes. That is a gate-definition choice,
   not made here.
3. Fix C: unchanged (opt-in). U-9684 / `MASHED_TGT3` stay blocked (the settle is not on by default).

## 6. Evidence hygiene

- **Main-root contamination (reported to the orchestrator during the lane).** The first 7 D runs (PIDs 6056 6964
  20740 21980 17616 22356 16048, 09:48:11-09:50:15) had a `MASHED_ROOT` without `original\`, so the exe walked up to
  the main repo and APPENDED to its `player_trace.log` / `mashed_re.log`. Nothing was deleted. Those runs are void
  (`RUNLOG.txt` note). The re-runs with a fixed root reproduced every `settle.txt` sha8. All later runs used
  `scratchpad\root` / `root2`, each with `original\` inside. The main-root trace was not touched again (last write
  09:49:36).
- **Shared `.asi`: never deployed.** Every build had `MASHED_NO_DEPLOY=1`. `original\mashed_re_dev.asi` is still
  13/10/2026 20:39:06.
- PIDs: only spawned PIDs were waited on or killed (`run_arms.py`). Two foreign `mashed_re` processes seen at start
  (20544, 24044) were not touched. No original-side (MASHED.exe) run in this lane.
- Exe `06642c53` vs `90a55dc7`: `D_B-2` settle = `D_B` (`8a6efcb4`), `D2_P1_2` = `D2_P1_1`. `H_DEF-3` on `06642c53` (the
  final branch exe) = `c2e4d60d / b50bd39b / ba90ed88`, so the shipped default is the base, bit for bit.

## 7. Proposed tracker transaction (re-classify, NOT applied here)

IDs: next free at base is U-9687 (d2settle's proposed rows were never applied; renumber at merge if they land first).
- **U-9687 (semantic)**: parked-player settle (d2settle's proposed row, now with the port). Mechanism `FUN_0046baa0` seeds
  + `0x0046f6c0` 4 -> 3 drop translation. Ported as fix B, default OFF. It reproduces the original's first-step move
  on the original's grid. It shifts the D2 launch (`+0xb14` d=14 vs 15) and pins the car on the Training wall.
  Path: section 3 [UNCERTAIN]. Gates U-9684 and any `MASHED_TGT3` default.
- **U-9688 (structural)**: `ReassertContacts`' contact cache (`g_cGrounded`, `g_cOnMesh`, `g_cNormal`) is one global
  written by every car's last `SolveWheelContacts`. The A4-first call after A4 runs before this car's first solver
  call, so it reads another car's (or the initial) cache. Measured: slot 0's seeded states are zeroed before its
  first solver call. D2 watch-list item (`ReassertContacts`).
- **U-9689 (structural)**: d2settle's fix C row. Every-frame `+0x9e4` refresh `0x004710a1..0x004710da`, default OFF.
  Re-tested inside fix B: B + C and P23 + C both out of the D2 bounds.
- **DEFERRED D-11071 (D2 REOPEN CANDIDATE) hits**: this lane changed the inputs and the use of `0x0046f6c0`. It read
  `ReassertContacts`' defect (U-9688) and moved player-car speed on Training (every D2 arm). It did not change
  `+0x4a4`, `ProduceTerrainBatch`, grip-clamp #6 or the chunk loop.
- **hooks.csv:** no status change (no run_diff, no C-level movement).
- **CHANGELOG (proposed):** `2026-10-16 d2posmodel (verify/d2posmodel_20261016/RESULT_d2posmodel.md): fix B (FUN_0046baa0
  seed, real suspension points, 0x0046f6c0 translation as position) ported behind MASHED_POS_MODEL / _SEED / _WHEELS
  / _TRANS, default OFF; settle reproduced on the original's grid; D2 fails (seed shifts +0xb14 to d=14, wall pin);
  parts 2+3 alone driving-median 1875.84; G-CTRL unchanged c2e4d60d/b50bd39b/ba90ed88.`

## 8. Raw

Committed: `PREREG_d2posmodel.md`, this file, `run_arms.py`, `g_settle.py`, `RUNLOG.txt`, `SCORE_D2.txt`,
`SCORE_LAUNCH.txt`, `SCORE_B.txt`, `G_SETTLE.txt`, `COLLATERAL_CROSS_H_{DEF,B}{,_stdout}.txt`, `D_*.settle.txt`,
`D_*.pathprog.csv`, the four collateral cuts `cut_H_{DEF,DEF-2,B,B-2}.csv`. Force-added (gitignored, cited):
`D_*.ptrace.log`, `D2_{DEF_1,B_1,P23_1,P1KG3_1,P1MIN_1,BC_1,BX_1,DX_1}.motion_diag.log`, `BX.wc.log`, `DX.wc.log`.
Not committed: `H_*.{step,gates}.csv`, `H_*.ptrace.log`, `B_*.step.csv`, the other motion logs (sha8 and line
counts in `RUNLOG.txt`).
