# RESULT: D-11072 leg P3, `FUN_00414a70` + `FUN_00414300` verbatim on the live path progress

Pre-registration: `PREREG_P3.md` (`09169f23`, before any code). Knob `MASHED_TGT3` (default OFF, active
only with `MASHED_PATHPROG`).

## 0. Verdict first

> **G-P3-REPLAY PASS: 5,490 of 5,490 original calls exact** (return value and `cand` bits; 63 `ret 1`,
> 36 `ret 2`, 5,391 `ret 0`; `cand` untouched on every `ret != 1` call), replaying the original's own
> recorded inputs (`o_tg1`, new `--tgt-probe`) through the naked body at x87 **PC_24**. At PC_53, 58
> `cand` mismatches (returns still exact). The transcription is right.
> **G-P3-OFF PASS** (`ae67b13a / c8317ce2 / 31db9384`). `P3_PP` (PATHPROG on, TGT3 off) reproduces
> `P2_LIVE` exactly (`4f675870 / 90e76845 / 4701b993`).
> **G-P3-COUNT FAIL** (1 of 6 items pass). **(b) regime A 0/3**, the same as PATHPROG alone (`P3_PP`
> 0/3), against 2/3 for the default build. Car 2's `|steer|` median stays 0.
> The failure is in the INPUTS, not the body: the port's (b) window starts at frame 0 with the cars
> bunched on the grid (no countdown in the default build), the original's at frame 882.

| gate | result |
|---|---|
| G-P3-REPLAY PC_24 | **5,490 / 5,490 exact** |
| (diagnostic) PC_53 | 5,432 / 5,490 (58 `cand` ulp mismatches, 0 return mismatches) |
| G-P3-OFF | `ae67b13a / c8317ce2 / 31db9384` |
| G-P3-INPUTS | progress in [0, 29.999], node count 30 on 100% of calls, speed > 0 on moving cars: all live |
| G-P3-SPEEDTAIL | 6 flips over 18,918 calls, **1 inside car 3's window**: not measured-harmless |
| G-P3-COUNT | FAIL (below) |

## 1. G-P3-COUNT (window = first 220 calls from the first `c4 != 0`)

| car | original `==1` / `==2` | port `==1` (with LOS) / `==2` | verdict | open-loop prediction |
|---|---|---|---|---|
| 1 | 16 / 0 | 38 (38) / 30 | FAIL, FAIL | 27 / 0 |
| 2 | 35 / 10 | 27 (27) / 16 | **PASS**, FAIL (by 1) | 27 / 95 |
| 3 | 12 / 26 | 30 (30) / 82 | FAIL, FAIL | 0 / 104 |

Prediction check: "closed-loop `==2` below open-loop for cars 2 and 3" held (16 < 95, 82 < 104).
"Car 1 `==2` PASS" failed (30 vs 0). "Car 2 `==1` PASS" held. "At least one `==2` FAIL" held.
Original window starts at frame 882 (`o_tg1`), the port's at frame 0: the default build has no countdown,
so the port scores the grid start, where the cars are within the 1.5 brake radius of each other.

## 2. (b) per regime (`ai_ctrl_window.py --check-regime`)

| arm | car 1 | car 2 | car 3 |
|---|---|---|---|
| `P3_CTRL` (default) | PASS | FAIL (`|steer|` 0) | PASS |
| `P3_PP` (PATHPROG) | FAIL | FAIL (`|steer|` 39, 3 items) | FAIL |
| `P3_LIVE` (PATHPROG + TGT3) | FAIL (`|steer|` 0) | FAIL (`|steer|` 0, `c0_distinct` 38) | FAIL (`|steer|` 0, `c0_distinct` 39) |

PATHPROG already moves (b) from 2/3 to 0/3 (faithful race_pct to the AI's readers, RESULT_P2 §1.2).

## 3. Inputs findings

1. **Speed (`+0xbc` = record `+0x9e4`) has a second writer the port lacks.** `FUN_00470c70`'s tail
   loop `0x004710b0..0x00471104` rewrites `+0x9e4 = |+0x9b0|` (`0x004710c3`/`0x004710c8`) and
   `+0x9e8 = |+0x9bc|` (`0x004710d5`/`0x004710da`) for every record with `[rec+4] != 0 || [rec+0x10] == 1`,
   after the per-car step loop and before the `FUN_004709a0` substeps (`0x00471137`). The port has only
   Integrate2's head write (`0x00467673`, `Integrate2.cpp:129`). Measured on `FUN_00414a70`: 6 of 18,918
   returns flip, 1 inside a window. Median `|+0x9e4 - |vel||` 0.2..0.6, max 1196.
2. **`FUN_0046d510` writes record `+0x9d4`** with the matrix `at` row. The shim does not. The two differ
   bitwise on 40,596 of 40,632 calls, by at most 0.0044 (player trace: `bodyfwd` vs `r0at`), a lag
   between the two updates.

## 4. Collateral (`collateral.py --mode paired`, `P3_PP` vs `P3_LIVE`, floor `P2_LIVE`)

272 of 500 fields past the floor (0, port bit-deterministic across boots: `P3_PP` == `P2_LIVE`). AI
targeting state from frame 1 by construction. **Player physics first diverges at frame 28** (pos, yaw,
basis, velocity together): the grid is bunched and an AI car's changed control reaches the player
through contact. The only D2 watch-list field that moves, `c9ec`, first diverges at frame 2047,
downstream of that.

**D2 REOPEN CANDIDATE (report only, from code reading, not a measured D2 delta):** finding 3.1 sits in
the substep/chunk loop `0x00470c70` on the watch list. `ContactFixup.cpp:133/253` reads `+0x9e4` inside
`FUN_004709a0`, which the original runs AFTER the tail rewrite, so the original's fixup sees post-step
`|vel|` where the port's sees Integrate2's pre-step value.

## 5. Next

The body is done and exact. To score it fairly the port's window must start where the original's does:
run P3 with the countdown (`MASHED_SUBSTATE` + `MASHED_MODE5_RESET`, D-11073) so the cars leave the grid
on the same frame, then re-score G-P3-COUNT. Separately, port the dispatcher speed tail (finding 3.1),
pre-registered as its own leg because it changes `ControlStep`'s speed read and, possibly, D2.
Mode 7 (`FUN_00414c30`, car 1) remains the later leg.

## 6. Raw

`o_tg1.msd.tgt.csv` (original, 5,490 calls), `o_tg1.msd.aistep.csv`, `p3_replay_pc{24,53}.txt`,
`run_p3_replay.ps1`, `score_p3.py`, `model_14a70.py`, `COLLATERAL_P3*.txt`, `piv_*.csv`,
`../d3_gatefire_20261008/P3_{CTRL,PP,LIVE}.*` (`P3_LIVE.tgt3.csv` = per-call trace).

## 7. Correction 2026-10-10 (x87 precision scope)

The `PcScope` in `Ai/AiTargetClosest.cpp` (and P1's `PathNodes.cpp`, P2's `PathProgStep.cpp`) never restored
PC_53, so `P3_PP` and `P3_LIVE` ran the whole game thread at PC_24 after track load
(`verify/d3_mode7_20261010/RESULT_M1.md` §3). Re-run on the fixed build: `P3F_LIVE` is byte-identical to
`P3_LIVE`, and `P3F_PP` first differs from `P3_PP` at frame 1134 with identical (b) windows. Every number in
§0..§4 stands.
