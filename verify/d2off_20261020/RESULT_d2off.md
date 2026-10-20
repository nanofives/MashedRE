# RESULT d2off: the hidden D2 offsetting error (2026-10-20)

Branch `work/d2off` from `work/u9772` `fe143a2fa`. PREREG `PREREG_d2off.md` (`c7ca497fc`), amendment 1 (`f8ef83bea`), each
committed before its arms. Exes `d9a9c76b` (base), `d58732f9` (+ amendment 1 knobs), `MASHED_NO_DEPLOY=1`, root
`D:\MashedWT\u9756_root`, 13 tracks on `D:\MashedWT\u9749_root13`. Original reference `orig_solo3.msd` (= `orig_solo_r1` field for
field); new capture `o_wwaf4` (D2 argv + `--watch-write af4,afc`). Arm outputs in `D:\MashedWT\d2off_data` (`MOVED_OFF_GIT.txt`).
No default changed.

## 0. Verdict

1. **The offsetting error was the position model.** Without `MASHED_POS_TRANS` (d2posmodel fix B part 3, in `MASHED_POS_MODEL`)
   the default physics carries the caller's y: the spawn y line gives the original's 0.437426805, the physics gets
   0.4749268 (`+ car_ground_off_` 0.0375), and the player's record y stays 0.0404..0.066 high all run. The rest of the default
   is tuned around that offset. `POS_MODEL` alone fails D2 (driving-median 1857.11). FAITH without it also fails (1865.04).
2. **The full faithful stack `FAITH` passes D2 3/3** on the re-based gate (whole log, 8-run band): slip1500 0.1918 x3, slip2000
   0.2499 / 0.2501 / 0.2501, driving-median 1910.76 / 1909.67 / 1909.67, H1 398/400. On it the player's record is bit-equal to
   the original's from the first moving frame through k = 63 (dynamic block `+0x928..+0x9f0`, `+0xb00..+0xb2c`). `FAITH` (38
   knobs) = u9771's `FULLGO` without its diagnostics, plus `MASHED_A5_FD0_LIVE=1`, `MASHED_A5_DRAFT_LUT=1` and `MASHED_FIX_5CEA44`:
   `AI_COUNTDOWN_CLOCK, POS_MODEL, PATHPROG, ROUND_GRID, ROUND_GRID_ORDER, TGT3, GRID_DIR, AI_CONTACT_EVENT, AI_TICK_FIRST,
   PATHPROG_LATE, REASSERT_KEEP_NORMAL, SPAWN_BASIS_RW, FIX_5CEA60, UP_TRINORMAL, RW_SSE_POINTS, RING_DBUF, FWD_AT, ROTATE_LUT,
   ORTHO_LUT, A6A_LC0, ROTATE_PC24, HEADING_FWD_LIVE, AI_LUT=7, STEER_ACOS_CRT, AI_FWD_ATROW, STEER_SCALE_RDATA, REGISTRY,
   PICKUPS_ORIG, REG_CARS, REG_CALLBACKS, MODE7, CC_PC24, A4_ORDER, REG_LATE, A6A_GEAR_ORDER, A5_FD0_LIVE=1, A5_DRAFT_LUT=1,
   FIX_5CEA44` (all `MASHED_`).
3. **The first material split on FAITH is k = 94** (`+0x9b0` 176.46936 vs 176.479889): two new contacts (slots 5 and 9, same ids)
   whose normals differ at 1e-5. Cause: the collision leaves `FastSqrt` / `Vec3Mag` / `Vec3Normalize` (`Collision/ContactDeps.h`)
   run `std::sqrt`, where the original runs the LUT bodies FUN_004c3b30 / FUN_004c3ac0 / FUN_004c39b0. New knob
   **`MASHED_COLL_LUT=1`** (default OFF) routes them to `Math::LutSqrt / LutLength / LutNormalize`. With it the split moves
   from k 94 to k 258 (|dv| > 1e-3; > 1 from k 146 to k 337), with ulp-level drift from k 113 and k 138.
4. **But `COLL_LUT` fails D2:** FAITH + COLL_LUT 1882.20, FAITH2 (+ ring order) 1878.85 x2, driving-median under 1893.63;
   slips in. So a further offsetting error remains on the D2 path after k = 138. Next split: `+0x9b0` 3 ulp at k 138, then the
   drive accumulators `+0xb0c / +0xb14 / +0xb1c` from k 139 (new U-9774).
5. **A5's face-normal ring, sign-flipped in the port, is fixed and inert:** the original calls FUN_0046c5f0 with EAX s[0], ECX
   s[6], EDX s[3] (`0x0046e6f3..0x0046e728`); the port passed s[3], s[6]. **`MASHED_A5_RING_ORDER=1`** (default OFF) makes the ring
   equal (through k 138 on FAITH2). FAITH + it gives a motion_diag log byte-identical to FAITH's, so the ring and its average
   are not read on D2. The average (`+0xaf4..+0xafc`, FUN_00470c70 `0x004711b1..0x00471244`, watch-write pcs `0x00471232` /
   `0x0047124a`) is not ported; no instruction in `.text` addresses it by displacement except the reset at `0x0046bba6..0x0046bbbc`.
6. **Residual record differences that do not reach the dynamic block on FAITH:** the per-wheel history `+0x1ac..+0x1e8` (stride
   0xc4) holds the same values rotated 4 slots (the original starts writing slot 6, the port slot 2), and `+0x44 / +0x48 /
   +0x13c / +0x140` differ from the first frame (static).

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | both exes, knobs unset: B `d6d87a6c / b903bddb / 0d23fb1f`; D2 dump `D2_RD_1` = `D2_RD_2` record for record (only the `+0x944` pointer word); D2 0.1859 / 0.2534 / 1987.69 | PASS |
| G-D2 FAITH | 0.1918 / 0.2499..0.2501 / 1909.67..1910.76, H1 398/400, 3/3 | PASS |
| G-D2 FAITH + COLL_LUT | 1882.20; FAITH2 1878.85 x2 (driving-median OUT) | FAIL |
| G-B FAITH | K0 / K1 / K26 `2c24bca3 / 9be204c1 / 97cac7c8`: A 2/3 (cars 2, 3), B 2/3 (cars 1, 3), C 1/3 (car 1) | reported |
| G-13 FAITH | 39 frames, 868 rows, all cited, 0 RED (against u9771's FULLGO 869: Roundabout 1091 34 -> 33, Storm 928 22 -> 21, Training 884 21 -> 20, 924 / 959 20 -> 21) | PASS |
| G-CTRL FAITH | `H_FAITH` `826277f2 / 250c1569 / 2c8164b2` | reported |

(b) on FAITH against the default (A 1/3 car 3, B 3/3, C 0/3): A gains car 2 and keeps car 3, C gains car 1, B loses car 2.
Under the rule "no car that passes on the default may fail" FAITH is not a default candidate (B car 2). User decision.

All D2 runs alone, 90 s, 1630..1637 motion_diag lines; same-stack runs are byte-identical on their common prefix.

## 2. Files

- `mashedmod/src/mashed_re/Collision/ContactDeps.h`: `CollLut()`, the three leaves.
- `mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp`: `MASHED_A5_RING_ORDER`.
- `verify/d2off_20261020/`: `run_arms.py` (stacks `RD`, `RDS`, `FAITH`, `FD`, `FNPM`, `PM`, `FAITH2`, `FD2`, `FRO`, `FCL`), `d2rec.py`
  (fields per call), `d2first.py` (first differing call over an offset set), `d2new.py` (first new difference after k = 0),
  `msdpeek.py`, `recpos.py`, `vsum.py`.
