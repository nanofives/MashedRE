# PREREG d2off: the hidden D2 offsetting error (2026-10-20)

Branch `work/d2off` from `work/u9772` `fe143a2fa` (= `race/first-frame-parity` `0ddf1d4d6` + u9772). Lane set by Mariano's
decision (2026-10-20, relayed by the orchestrator). Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, root
`D:\MashedWT\u9756_root` (run_arms default). Written before any knob arm of the lane.

## 0. Measured before this PREREG (default build, no knob)

1. `D2_RD_1` (D2 recipe + `MASHED_RECDUMP` slot 0): slip 0.1859 / 0.2534, driving-median 1987.69, H1 398/400, the ccinv
   default; the dump does not move the score. 1633 records.
2. The original's player record on the D2 capture argv is the same on two hosts (`orig_solo3` MARIANO-PC and `orig_solo_r1`
   LIVING-PC agree field for field from their first moving frame, `+0x9e4 > 0.01`).
3. First moving frame, original vs port: `+0x9b8` -0.636886 vs -0.637861, `+0x95c` 0.434478 vs 0.474927 (+0.0404), `+0xbf4`
   2600 vs 2800. The trajectories separate from about k = 20 (`+0x9e4` 107.74 vs 90.34).
4. `MASHED_SPAWN_Y_LOG`: the port's spawn y line (FUN_0046d400) gives 0.437426805, the original's grid y. The settle input is
   `io.pos` y 0.4749268 = that + `car_ground_off_` 0.0375: without `MASHED_POS_TRANS` (d2posmodel fix B part 3, default OFF)
   the default physics runs on the caller's y.

## 1. The full faithful stack `FAITH`

Every default-OFF knob of the regime-C lanes' stack (u9771 `FULLGO`, the B-recipe stack whose player and AI records match the
original), without its diagnostics (the slot-1 record dump, the mode-7 trace), plus u9772's pair and `MASHED_FIX_5CEA44`:
`MASHED_AI_COUNTDOWN_CLOCK, POS_MODEL, PATHPROG, ROUND_GRID, ROUND_GRID_ORDER, TGT3, GRID_DIR, AI_CONTACT_EVENT, AI_TICK_FIRST,
PATHPROG_LATE, REASSERT_KEEP_NORMAL, SPAWN_BASIS_RW, FIX_5CEA60, UP_TRINORMAL, RW_SSE_POINTS, RING_DBUF, FWD_AT, ROTATE_LUT,
ORTHO_LUT, A6A_LC0, ROTATE_PC24, HEADING_FWD_LIVE, AI_LUT=7, STEER_ACOS_CRT, AI_FWD_ATROW, STEER_SCALE_RDATA, REGISTRY,
PICKUPS_ORIG, REG_CARS, REG_CALLBACKS, MODE7, CC_PC24, A4_ORDER, REG_LATE, A6A_GEAR_ORDER, A5_FD0_LIVE=1, A5_DRAFT_LUT=1,
FIX_5CEA44` (all `MASHED_`). The list is fixed here; a knob is added or dropped only by an amendment with its measurement.

## 2. Method

`D2_FD` = D2 recipe + `FAITH` + the player record dump. `d2rec.py` compares the player's record against `orig_solo3` from each
side's first moving frame, call by call, until the first field that differs; then the producer of that field is found in the
original (RVA), ported behind a new default-OFF knob, added to `FAITH` by amendment, and the comparison repeats.

## 3. Gates (reporting; no default flip without Mariano)

| gate | rule |
|---|---|
| G-NOOP | knobs unset: B dumps `d6d87a6c / b903bddb / 0d23fb1f`, D2 = `D2_RD_1` |
| G-D2 | `score_d2_v2.py port` (whole log), 8-run band, 3 runs each alone within `h`: PASS 3/3 |
| G-B | (b) on the final stack, vsum.py, reported per car |
| G-13 | 13 tracks on the final stack: rows, RED count |
| G-CTRL | `H_` hashes of the final stack, reported |
