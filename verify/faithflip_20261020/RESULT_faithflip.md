# RESULT faithflip: the full faithful stack FAITH default ON (2026-10-20)

Branch `work/faithflip` from `race/first-frame-parity` `ae21a9f6e`. PREREG `PREREG_faithflip.md` (`7fc61de8f`, with the seed TU),
committed before any arm. Exe `e03c0a3d` (`MASHED_NO_DEPLOY=1`), root `D:\MashedWT\u9756_root`, 13 tracks on
`D:\MashedWT\u9749_root13`. Arm outputs in `D:\MashedWT\faithflip_data` (`MOVED_OFF_GIT.txt`). Model claude-opus-5-5, account2.

## 0. Verdict: FLIPPED (Mariano's decision, 2026-10-20)

Every gate passes. The new default is the d2off `FAITH` arm exactly: its G-CTRL triple, its B dumps, its 13-track scores and its D2
log (byte-identical on the common prefix to `D2_FAITH_2`). `MASHED_NO_FAITH=1` is the pre-flip default exactly.

## 1. Flip

New exe-only TU `mashedmod/src/mashed_re/Core/FaithDefaults.cpp` (listed in `exe_sources.rsp` only, under `MASHED_STANDALONE`,
`#pragma init_seg(lib)`). Before any user TU's initializer it seeds `MASHED_<name>` with the FAITH value for each of the 38
knobs, unless `MASHED_<name>` is already set or `MASHED_NO_<name>` is set. Under `MASHED_NO_FAITH=1` it seeds nothing. The 38
reader sites are unchanged.

| # | knob (`MASHED_`) | value | opt-out |
|---|---|---|---|
| 1 | AI_COUNTDOWN_CLOCK | 1 | `MASHED_NO_AI_COUNTDOWN_CLOCK` (already default ON before; seeding is inert) |
| 2 | POS_MODEL | 1 | `MASHED_NO_POS_MODEL` |
| 3 | PATHPROG | 1 | `MASHED_NO_PATHPROG` |
| 4 | ROUND_GRID | 1 | `MASHED_NO_ROUND_GRID` |
| 5 | ROUND_GRID_ORDER | 1 | `MASHED_NO_ROUND_GRID_ORDER` |
| 6 | TGT3 | 1 | `MASHED_NO_TGT3` |
| 7 | GRID_DIR | 1 | `MASHED_NO_GRID_DIR` (already default ON before; seeding is inert) |
| 8 | AI_CONTACT_EVENT | 1 | `MASHED_NO_AI_CONTACT_EVENT` |
| 9 | AI_TICK_FIRST | 1 | `MASHED_NO_AI_TICK_FIRST` |
| 10 | PATHPROG_LATE | 1 | `MASHED_NO_PATHPROG_LATE` |
| 11 | REASSERT_KEEP_NORMAL | 1 | `MASHED_NO_REASSERT_KEEP_NORMAL` |
| 12 | SPAWN_BASIS_RW | 1 | `MASHED_NO_SPAWN_BASIS_RW` |
| 13 | FIX_5CEA60 | 1 | `MASHED_NO_FIX_5CEA60` (already default ON before; seeding is inert) |
| 14 | UP_TRINORMAL | 1 | `MASHED_NO_UP_TRINORMAL` |
| 15 | RW_SSE_POINTS | 1 | `MASHED_NO_RW_SSE_POINTS` (already default ON before; seeding is inert) |
| 16 | RING_DBUF | 1 | `MASHED_NO_RING_DBUF` |
| 17 | FWD_AT | 1 | `MASHED_NO_FWD_AT` (already default ON before; seeding is inert) |
| 18 | ROTATE_LUT | 1 | `MASHED_NO_ROTATE_LUT` |
| 19 | ORTHO_LUT | 1 | `MASHED_NO_ORTHO_LUT` |
| 20 | A6A_LC0 | 1 | `MASHED_NO_A6A_LC0` |
| 21 | ROTATE_PC24 | 1 | `MASHED_NO_ROTATE_PC24` |
| 22 | HEADING_FWD_LIVE | 1 | `MASHED_NO_HEADING_FWD_LIVE` |
| 23 | AI_LUT | 7 | `MASHED_AI_LUT=5` = the pre-flip mask; `MASHED_NO_AI_LUT=1` keeps its meaning (mask 0) |
| 24 | STEER_ACOS_CRT | 1 | `MASHED_NO_STEER_ACOS_CRT` |
| 25 | AI_FWD_ATROW | 1 | `MASHED_NO_AI_FWD_ATROW` |
| 26 | STEER_SCALE_RDATA | 1 | `MASHED_NO_STEER_SCALE_RDATA` |
| 27 | REGISTRY | 1 | `MASHED_NO_REGISTRY` |
| 28 | PICKUPS_ORIG | 1 | `MASHED_NO_PICKUPS_ORIG` |
| 29 | REG_CARS | 1 | `MASHED_NO_REG_CARS` |
| 30 | REG_CALLBACKS | 1 | `MASHED_NO_REG_CALLBACKS` |
| 31 | MODE7 | 1 | `MASHED_NO_MODE7` |
| 32 | CC_PC24 | 1 | `MASHED_NO_CC_PC24` |
| 33 | A4_ORDER | 1 | `MASHED_NO_A4_ORDER` |
| 34 | REG_LATE | 1 | `MASHED_NO_REG_LATE` |
| 35 | A6A_GEAR_ORDER | 1 | `MASHED_NO_A6A_GEAR_ORDER` |
| 36 | A5_FD0_LIVE | 1 | `MASHED_NO_A5_FD0_LIVE` |
| 37 | A5_DRAFT_LUT | 1 | `MASHED_NO_A5_DRAFT_LUT` |
| 38 | FIX_5CEA44 | 1 | `MASHED_NO_FIX_5CEA44` |

All of FAITH: `MASHED_NO_FAITH=1`. `MASHED_COLL_LUT` and `MASHED_A5_RING_ORDER` stay default OFF.

## 2. Gates

| gate | measured | verdict |
|---|---|---|
| G-CTRL x2 | `H_DEF` = `H_DEF-2` = `826277f2 / 250c1569 / 2c8164b2` (= d2off `H_FAITH`) | PASS |
| G-OPTOUT | `MASHED_NO_FAITH=1`: `H_NF` `66d51720 / ca4284be / 1193bbd1`; `B_NF_K0/K1/K26` `d6d87a6c / b903bddb / 0d23fb1f` | PASS |
| G-B | `B_DEF_K0/K1/K26` `2c24bca3 / 9be204c1 / 97cac7c8` (= d2off FAITH); (b) A 2/3 (cars 2, 3), B 2/3 (cars 1, 3), C 1/3 (car 1) | as expected |
| G-D2 | 3 runs alone: slip1500 0.1914 / 0.1914 / 0.1912, slip2000 0.2501 x3, driving-median 1907.69 x3, H1 398/400; logs byte-identical on their common prefix to each other and to d2off `D2_FAITH_2` | PASS 3/3 |
| G-13 | 39 frames, 868 rows, all cited, 0 RED; identical to d2off's FAITH run | PASS |

Changed (b) verdicts against the pre-flip default (`B_NF`: A 1/3 car 3, B 3/3, C 0/3): **A car 2 FAIL -> PASS**, **B car 2 PASS ->
FAIL** (`c1_distinct 63 [17,46]`, `steer_distinct 100 [29,84]`, `c1_median 6.0 [0,0]`, `abs_steer_median 29.0 [0,16]`; accepted by
Mariano's decision), **C car 1 FAIL -> PASS**. Unchanged: A car 1 FAIL (now `c1_distinct 20 [21,70]`), A car 3 PASS, B cars 1 and 3
PASS, C cars 2 and 3 FAIL (`c1_distinct 40 [41,41]` / `80 [79,79]`).

13-track rows changed against the pre-flip default (ccinv's 868-row run): Storm 928 22 -> 21, Training 884 21 -> 20, Training
924 20 -> 21, Training 959 20 -> 21; every row cited (`cites_rres.json`).

## 3. Files

`Core/FaithDefaults.cpp`, `mashedmod/exe_sources.rsp`; `verify/faithflip_20261020/` (`run_arms.py` stacks `NF`, `NFRD`, `FLRD`),
`apply_trackers.py` (flip notes on 23 RESOLVED U-ids, one CHANGELOG line), `gates_line.txt`.
