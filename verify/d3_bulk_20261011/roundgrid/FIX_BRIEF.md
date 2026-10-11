# Fix brief: the round-restart grid (modes third, round flow), 2026-10-11

## 1. Problem

The exe restarts every round on the start-line grid. `TrackRenderer::StartRound()`
(`D3d9Render/TrackRenderer.cpp:5148`) anchors the formation at `gates_[0]` each time.

The original restarts each round at the node where the surviving car was when the previous round ended.
- Measured: `verify/d3_bulk_20261011/aileaf/RESULT_AILEAF.md` Phase 2e, and the capture `o_rounds7b` below.
- Consequence seen so far: round-2+ situations differ by track position. The exe's 19 G-AILEAF mode-1 commits come
  from its start-line restart: a rammed, parked player on the low-curvature start straight.

## 2. The original's chain (RVAs this fix reproduces)

| step | RVA | what it does (cited) | tracker |
|---|---|---|---|
| race init | 0x0040fef0 in FUN_004111c0 | `mov [0x0063ba80], ebx` (round-1 node; reads 0 in `o_rounds7b`) | C2 mapped |
| round end | FUN_00410860, 0x00410890..0x004108c5 | survivor = the last slot i (0..3) with `[[0x005f2770] + 0x34 + i*4] != 0` and `FUN_0046c7b0(i) == 1`; `DAT_0063ba80 = __ftol(FUN_00408a50(survivor))`, i.e. the truncated progress `0x008a96e8 + i*0x30c` | C2 mapped (.asi-only body) |
| round start | FUN_0040e590 placement loop | `FUN_00409290(&DAT_0063ba80)`, then per slot `FUN_00408b00(DAT_0063ba80, slotval, DAT_008a94d0, &pos, &yaw)`, `FUN_00408a70(i, (float)DAT_0063ba80)`, `FUN_004704c0(...)` | C2 new |
| snap | FUN_00409290 (110 B) | clamps `*p` into the ordered (lo, hi) ranges: below a range's lo it becomes lo; past the last range it becomes the first range's lo | C2 new |
| ranges | FUN_00426bc0 / FUN_00426bd0 | count `[0x0066d6e0]`; pair i = u16 `[0x0066d658 + i*4]` / `[0x0066d65a + i*4]` (LAPDATA `Safe_Start_Lines`) | C3 impl, exe-built (`Frontend/GlobalGetters_s5.cpp`) |
| anchor | FUN_00408b00 → FUN_00426cb0(node) = `0x00663664 + node*0x4c` (record centre), FUN_00426cc0(node) = `0x00663658 + node*0x4c` (record dir), FUN_00426d00(node, 0 / 3) (corners) | grid formation `pos = A + F*fwdOff[s] - L*latOff[s]` (`TrackRenderer.cpp:5139`) | C2 new; getters C3 / C4 |

## 3. Inputs and exe status

| input | original | exe |
|---|---|---|
| progress `0x008a96e8 + i*0x30c` | FUN_00408610 | written only under `MASHED_PATHPROG` (P2 verbatim, replay 100% at PC_24) |
| course block: node records `0x00663658`, ranges `0x0066d658` / count `0x0066d6e0` | track load (FUN_00426e10 → LAPDATA) | `Race/PathNodes.cpp`, PATHPROG P1. Byte-compared vs the original's live block (G-P1); `Safe_Start_Lines` handler at `:85-88` |
| survivor / alive | `FUN_0046c7b0`, slot table `0x005f2770` | `VehicleSlotGetter` (record-rebound); slot-state seed (default ON). The exe has its own `round_winner_` (`TrackRenderer.cpp:5547`) |
| node at round start | `DAT_0063ba80` | no writer (0x004108c5 has no exe equivalent) |

So the knob must require `MASHED_PATHPROG`, as TGT3 and AILEAF do.

## 4. Port plan

One default-OFF knob, `MASHED_ROUND_GRID`, which requires `MASHED_PATHPROG`. Stages:

**Stage A (anchor only):**
- **A1:** at round end (the exe's equivalent of FUN_00410860, i.e. where `NextRoundOrEnd` runs), write
  `DAT_0063ba80 = __ftol(progress[survivor])`. The survivor is chosen with FUN_00410860's rule: the last active
  slot whose alive word is 1. Do NOT use `round_winner_` without first checking the two agree: log both.
- **A2:** port FUN_00409290 verbatim (gen_naked_asm: 110 B; callees are the C3 getters).
- **A3:** in `StartRound`, when the knob is on, take A / L / F from the course-block node `DAT_0063ba80`
  (FUN_00426cb0 / cc0 / d00) instead of `gates_[0]` / `gates_[1]`. Keep the existing formation table and the
  slot = spawn-order assignment.

**Stage B (formation and order, separate PREREG):**
- Port FUN_00408b00 (C2, 1,883 B; executed on Training per the Phase 0 census) verbatim.
- Port FUN_0040e590's slot assignment (finish order, ties: plate sections C..F). That replaces the exe's fixed
  player = slot 0.
- In `o_rounds7b` the grid order changes per round, so Stage A alone cannot match per-car positions.

Not in scope:
- FUN_00410860's other writes (`0x007f1014`, the FUN_00429b30 call): listed, not ported.
- FUN_004704c0's grid placement physics: D2-adjacent, it calls VehicleWheelContactSolve 0x0046f6c0.

## 5. Gates

**Before code (predict-a-port's-output):**
- On the exe, under PATHPROG, log the survivor index (both rules) and its progress at each round end, plus the
  snapped node FUN_00409290 would return, from a knob-OFF run.
- Expected: integer nodes in 0..29 advancing around the lap, like the original's
  `7, 11, 14, 21, 24, 28, 1` (`o_rounds7b`; one run; round outcomes do not reproduce, so match the mechanism, not
  the sequence).

**G-RG-SNAP (replay, the original's bytes):**
- FUN_00409290 over a grid of (node, range table).
- Use the exe's own P1 table plus edge cases: below the first lo, inside ranges, between ranges, past the last.
- Compare in Unicorn (the `aileaf_unicorn.py` pattern) against the naked body. Negative control on every replay.

**G-RG-NODE (mechanism):**
- Original side: capture the survivor's progress and `DAT_0063ba80` at each 6 → 7 transition. Extend
  `--continue-rounds` (it already logs `node_63ba80`) with each car's progress and alive word at the transition.
- Check that `__ftol(progress[survivor])` equals the logged node on every round, then run the same check on the exe.

**G-RG-GRID (positions, Stage A):**
- For each original round r with node N_r (`o_rounds7b`: N = 0, 7, 11, 14, 21, 24, 28, 1), the exe's Stage-A
  formation at N_r must reproduce the original's four grid positions as a SET, within 0.02 in x/z.
  The tolerance is the one the round-1 formula met: `TrackRenderer.cpp:5146`.
- The per-car assignment is Stage B.
- Expected original grids, front car (x, z, progress):

| round | node | front car | progress |
|---|---|---|---|
| 1 | 0 | c0 (0.5, -2.0) | 30.00 |
| 2 | 7 | c0 (9.6, -21.7) | 7.00 |
| 3 | 11 | c0 (11.5, -6.0) | 11.03 |
| 4 | 14 | c0 (15.5, 6.0) | 14.00 |
| 5 | 21 | c2 (8.0, 25.5) | 20.98 |
| 6 | 24 | c0 (0.5, 22.0) | 23.98 |
| 7 | 28 | c0 (0.5, 6.0) | 28.00 |
| 8 | 1 | c0 (0.5, -6.0) | 1.01 |

  All four cars per round: `round_census.py orig/o_rounds7b.log orig/o_rounds7b.msd.aileaf.tsv`.

**G-CTRL (knob OFF):** `ae67b13a / c8317ce2 / 31db9384`. The knob also needs PATHPROG, so the knob-ON base
arm is AL_PP-style (`4e250237 / 22cceec1 / dc72ab09`).

**Prediction for the knob-ON race arm** (PREREG before enabling):
- The round-2+ grids leave the start line.
- The exe's G-AILEAF 19 mode-1 commits (when stacked) change or vanish.
- (b) is scored in round 1 only, so it is unchanged (window ends before round 2). Report per regime.
- Collateral (`--mode paired`, same-arm repeat floor) as a report. D2 watch list: `+0x4a4`, the contact
  collector, substep 0x00470c70. The grid move changes where contacts happen, so any hit there is a
  D2 REOPEN CANDIDATE row.

## 6. Promotion leg (fix-briefs-carry-a-promotion-leg)

- **RVAs reproduced:** 0x00409290 (verbatim) and the 0x004108c5 store (a statement of FUN_00410860, not the
  whole body).
- **0x00409290:** pure CPU over two C3 getters. Put a shared-TU body at the RVA in a TU compiled into both
  targets, `RH_ScopedInstall(…, 0x00409290)`, a `hooks_registry.py` entry, `run_diff.py` + `run_verify_hook.py`
  → a C3 path.
- **FUN_00410860 and FUN_00408b00 (Stage B):** exe-only until their own diffs exist. Exe-only bodies stay
  below C3.
- **Trackers via re-classify:** a row for the exe StartRound start-line anchor vs the original's
  `DAT_0063ba80` node, citing this brief.

## 7. Evidence files

- `verify/d3_bulk_20261011/aileaf/orig/o_rounds7b.log`: the sub-state transitions with `node_63ba80`.
- `o_rounds7b.msd.aileaf.tsv`: the per-call car rows.
- `o_rounds7.*`: the first 8-round capture.
- `o_rounds_log.*`: no presses, the stall control.
- `RESULT_AILEAF.md` Phase 2e: the chain disassembly and the survivor / next-grid check.
- Disassembly of FUN_00426bc0 / FUN_00426bd0: §2; their bytes at 0x00426bc0..0x00426bf2.
