# Round-restart grid, Stage A: RESULT (2026-10-11)

Verdict: **Stage A is ported behind `MASHED_ROUND_GRID` (default OFF, needs `MASHED_PATHPROG`). Every gate in
FIX_BRIEF §5 that can run with the knob OFF passes.**
- G-RG-SNAP 18504/18504 (negative control 3/3).
- G-RG-GRID: 5 original restart nodes match as sets, worst 0.0048 against the 0.02 tolerance.
- G-RG-NODE 3/3.
- G-CTRL unchanged (`ae67b13a / c8317ce2 / 31db9384`).
- The knob has not been switched on in a race: `PREREG_STAGE_A.md` comes first.

## What was ported (`mashedmod/src/mashed_re/Race/RoundGrid.{h,cpp}`, exe-only, `exe_sources.rsp`)

| piece | original | exe |
|---|---|---|
| race-init node | 0x0040fef0 `mov [0x0063ba80], ebx` (0) | `RoundGrid_RaceInit()` in `TrackRenderer::StartMatch` |
| round-end node | FUN_00410860 0x00410890..0x004108c5: last active slot with alive == 1; `__ftol(FUN_00408a50(slot))` | `RoundGrid_RoundEnd()` in the `r == 0` (next round) branch, over `race_[i].alive`. The exe's `round_winner_` (first alive) is logged beside it |
| snap | FUN_00409290 (110 B) | verbatim naked `RG2_409290` (`re/tools/naked_specs/roundgrid_a.json` → `RoundGrid_naked.inc`); callees bound to the C3 exports `GetDat0066d6e0` / `GetTableEntry0066d658` |
| anchor | FUN_00408b00: A = FUN_00426cb0(n) (record + 0x0c); rank axis = the RAW record dir (record + 0, `local_30`..); lateral = FUN_004c39b0(corner0 - corner3) (FUN_00426d00(n, 0/3) = record + 0x18 / + 0x3c) | `RoundGrid_Anchor()` → `StartRound` replaces `gates_[0]` / the gate dir with them. The formation table is unchanged |

**Inputs:** the progress, node records and Safe_Start_Lines table all come from PATHPROG P1/P2.

**Correction during the work:** the first anchor normalized both axes in 2-D. That spaced the ranks 1.1 apart
where the original's spacing is the raw record length (about 1.0 at nodes 11 and 14). Now both axes are taken as
FUN_00408b00 scales them.

## Gates

| gate | result | evidence |
|---|---|---|
| G-RG-SNAP | 18,504/18,504 exact; the snap changes 8,210 of them | `snap.tsv` (`re/tools/roundgrid_unicorn.py`, original bytes 0x00409290 + 0x00426bc0/bd0), `snap_synth.txt` |
| G-RG-SNAP negative control | 18,501/18,504, bad = 3 = the 3 corrupted rows | `snap_negctl.txt` |
| G-RG-GRID | nodes 0 / 6 / 10 / 14: 0.0001 / 0.0000 / 0.0048 / 0.0000 (`o_rounds_dense`); node 7: 0.0000 (`o_rounds7b`). All PASS (≤ 0.02) | `grid_compare.py`, `grid_check_synth.txt` (all 30 nodes) |
| G-RG-NODE (original mechanism) | 3/3: trunc(progress of the last alive car) == the logged `DAT_0063ba80` (6, 10, 13) | `node_check.py` on `o_rounds_dense` |
| G-CTRL (knob OFF) | `ae67b13a / c8317ce2 / 31db9384` | `verify/d3_gatefire_20261008/RG_CTRL.*` |

**Notes on the node check:**
- The 4th row is the match end (6 → 11). There is no next round, so there is no write, and the exe
  matches: it writes only in `r == 0`.
- One snap was observed: round 3 ended at node 13 and round 4 gridded at 14.

## Finding: the exe's round-1 grid is off by up to 0.0236

The gate-based formation in `StartRound` (`TrackRenderer.cpp:5148`) differs from the node-0 formation by
0.0035 / 0.0079 / 0.0157 / 0.0236 (slots 0..3). Node 0 matches the original's round-1 grid at 0.0001.
- So the existing gate anchor is off from the original by up to 0.0236. Its code comment claims ≤ 0.02.
- With the knob ON, round 1 also moves to the node-0 anchor, toward the original.

## Grid comparison method

The AI cars move during sub-state 5 (the Branch A launch), so `grid_compare.py` takes the last row in each
countdown where all four speeds are 0.
- The first version used a mid-window row and reported false failures up to 1.40. Its output is superseded.
- The sparse captures (`--aileaf-probe 20`) have no all-stationary rows for most rounds. Use `--aileaf-probe 1`.

## Not in Stage A

**Stage B:** the car-to-slot order. The original reassigns grid slots per round (`orig car -> exe slot` is
`(1, 3, 0, 2)` at node 14 and `(0, 3, 2, 1)` at node 7); the exe keeps player = slot 0. This needs FUN_0040e590's
assignment and FUN_00408b00 verbatim, with a separate PREREG.

**Also not ported:** FUN_00410860's other writes, and FUN_004704c0's placement physics.
