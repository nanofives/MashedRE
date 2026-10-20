# PREREG faithflip: the full faithful stack FAITH default ON (2026-10-20)

Branch `work/faithflip` from `race/first-frame-parity` `ae21a9f6e` (d2off merged). Mariano's decision (2026-10-20, relayed by the
orchestrator): turn FAITH default ON, accepting regime B car 2's loss ((b) A 2/3, B 2/3, C 1/3); this overrides the "no passing car
may fail" rule for this flip only. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`,
13 tracks on `D:\MashedWT\u9749_root13`. Written before any arm.

## 1. The 38 knobs (FAITH = `verify/d2off_20261020/run_arms.py` `STACK['FAITH']`; all `MASHED_`)

| # | knob | value | reader before the flip |
|---|---|---|---|
| 1 | AI_COUNTDOWN_CLOCK | 1 | already default ON (opt-out `MASHED_NO_AI_COUNTDOWN_CLOCK`); no `MASHED_AI_COUNTDOWN_CLOCK` reader |
| 2 | POS_MODEL | 1 | default OFF |
| 3 | PATHPROG | 1 | default OFF |
| 4 | ROUND_GRID | 1 | default OFF |
| 5 | ROUND_GRID_ORDER | 1 | default OFF |
| 6 | TGT3 | 1 | default OFF |
| 7 | GRID_DIR | 1 | already default ON (opt-out `MASHED_NO_GRID_DIR`); no `MASHED_GRID_DIR` reader |
| 8 | AI_CONTACT_EVENT | 1 | default OFF |
| 9 | AI_TICK_FIRST | 1 | default OFF |
| 10 | PATHPROG_LATE | 1 | default OFF |
| 11 | REASSERT_KEEP_NORMAL | 1 | default OFF |
| 12 | SPAWN_BASIS_RW | 1 | default OFF |
| 13 | FIX_5CEA60 | 1 | already default ON (opt-out `MASHED_NO_FIX_5CEA60`); no `MASHED_FIX_5CEA60` reader |
| 14 | UP_TRINORMAL | 1 | default OFF |
| 15 | RW_SSE_POINTS | 1 | already default ON (opt-out `MASHED_NO_RW_SSE_POINTS`); no `MASHED_RW_SSE_POINTS` reader |
| 16 | RING_DBUF | 1 | default OFF |
| 17 | FWD_AT | 1 | already default ON (opt-out `MASHED_NO_FWD_AT`); no `MASHED_FWD_AT` reader |
| 18 | ROTATE_LUT | 1 | default OFF |
| 19 | ORTHO_LUT | 1 | default OFF |
| 20 | A6A_LC0 | 1 | default OFF |
| 21 | ROTATE_PC24 | 1 | default OFF |
| 22 | HEADING_FWD_LIVE | 1 | default OFF |
| 23 | AI_LUT | 7 | default mask 5 (opt-out `MASHED_NO_AI_LUT` = mask 0); FAITH sets 7 |
| 24 | STEER_ACOS_CRT | 1 | default OFF |
| 25 | AI_FWD_ATROW | 1 | default OFF |
| 26 | STEER_SCALE_RDATA | 1 | default OFF |
| 27 | REGISTRY | 1 | default OFF |
| 28 | PICKUPS_ORIG | 1 | default OFF |
| 29 | REG_CARS | 1 | default OFF |
| 30 | REG_CALLBACKS | 1 | default OFF |
| 31 | MODE7 | 1 | default OFF |
| 32 | CC_PC24 | 1 | default OFF |
| 33 | A4_ORDER | 1 | default OFF |
| 34 | REG_LATE | 1 | default OFF |
| 35 | A6A_GEAR_ORDER | 1 | default OFF |
| 36 | A5_FD0_LIVE | 1 | default OFF |
| 37 | A5_DRAFT_LUT | 1 | default OFF |
| 38 | FIX_5CEA44 | 1 | default OFF |

`MASHED_COLL_LUT` and `MASHED_A5_RING_ORDER` stay default OFF and are not seeded.

## 2. Mechanism

New exe-only TU `Core/FaithDefaults.cpp` (in `exe_sources.rsp` only, under `MASHED_STANDALONE`, `#pragma init_seg(lib)`): before
every user TU's initializer it seeds each `MASHED_<name>` with its FAITH value, unless `MASHED_<name>` is already set or
`MASHED_NO_<name>` is set, and seeds nothing under `MASHED_NO_FAITH=1`. The 38 reader sites are not edited, so the new default is
exactly the environment of the d2off `FAITH` arm, and `MASHED_NO_FAITH=1` is exactly the pre-flip default. Per-knob opt-out:
`MASHED_NO_<name>=1` for every knob (the five already-ON knobs and `AI_LUT` keep their existing `MASHED_NO_<name>` meaning;
`MASHED_AI_LUT=5` restores the pre-flip mask alone).

## 3. Gates (a D2 or 13-track failure means no flip; report)

| gate | rule |
|---|---|
| G-CTRL x2 | `H_DEF`, `H_DEF-2` = `826277f2 / 250c1569 / 2c8164b2` (d2off `H_FAITH`) |
| G-OPTOUT | `MASHED_NO_FAITH=1`: `H_NF` = `66d51720 / ca4284be / 1193bbd1`; `B_NF_K0/K1/K26` = `d6d87a6c / b903bddb / 0d23fb1f` |
| G-B | `B_DEF_K0/K1/K26` = d2off FAITH `2c24bca3 / 9be204c1 / 97cac7c8`; (b) expected A 2/3 (cars 2, 3), B 2/3 (cars 1, 3), C 1/3 (car 1); every changed verdict against the pre-flip default reported |
| G-D2 | default, three runs each alone, `score_d2_v2.py port`, 8-run band: PASS 3/3 |
| G-13 | default, 13 tracks: 0 RED, row changes against the pre-flip default (ccinv's 868-row run) cited |

## 4. Then

U-9774 from the merged tip (knobs default OFF, its own PREREG).
