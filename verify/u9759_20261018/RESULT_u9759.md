# RESULT u9759: the up vector +0x9cc (2026-10-18)

Branch `work/u9759` from `race/first-frame-parity` `f22c7d7a1` (default G-CTRL `b40b47bc / fb690fb1 / d3a20f4c`). Model
claude-opus-5-5, account2. PREREG `PREREG_u9759.md` with the knob (`8b2cb262a`), amendment 1 (`113d50225`), each committed
before the arms it governs. Exe `92b5e645` (`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\u9756_root`. Original
references: o_wobj2 and new captures `o_up1` (write watch), `o_bq1 / o_bq2 / o_bq3` (batch probe), `o_w1d4` (write watch).

## 0. Verdict

1. **The up vector's writer is the tri-normal `FUN_0046c5f0`** (`pc 0x0046c6a8 / 0x0046c6b1`, called at `0x0046f7b0`), the
   function `MASHED_UP_TRINORMAL` ports. Its arithmetic is exact: the port's formula on the original's own wheel points
   (`trinorm.py` on `o_bq1`) reproduces the original's up vector on every row checked.
2. **The difference was in the wheel points, and it is the RW points transform.** At runtime the vector-module slot `+0xc`
   is `0x004dc370`, not the x87 default `0x004c3670` that the installer writes at `0x004c3ebf` (live slots read by the
   extended probe `o_bq3`: `+8 / +0xc / +0x10 / +0x14` = `0x004dc4a0 / 0x004dc370 / 0x004dc530 / 0x004dc410`).
   `0x004dc370` is SSE and sums `(z*at + y*up) + (x*right + pos)` (`0x004dc3d2..0x004dc3e7`). The port's stand-in
   `RwV3dTransformPointsCPU` sums `((x*right + y*up) + z*at) + pos`, one ulp off on car 1's wheel 3 (`bf590939` vs
   `bf59093a`). The vectors slot `0x004dc410` sums `(x*right + y*up) + z*at`, which the port already matches.
3. **`MASHED_RW_SSE_POINTS` (default OFF) makes the points exact.** With it the port's spawn-pass and first-call wheel
   points equal the original's bit for bit. With `MASHED_UP_TRINORMAL` too, `+0x9cc` is exact at calls 0 and 1, and so are
   the rotation rows at call 1.
4. **The up vector is not the drift driver.** Car 1's drift is unchanged (1.202e-4 at call 21). At call 1 the velocity
   `+0x9b0` (-0.00108456612 / -0.00108465552), omega `+0x9c0` and the force words `+0x214 / +0x21c` (-2.8865118 /
   -2.88652229, -2385.21582 / -2385.22363) still differ. New U-9760.
5. **A separate lead: the A4 torque ring's phase.** A4 writes `rec + 0x1ac + (DAT_007f101c & 0xf)*4` and `+0x270 + ..`
   (watch `o_w1d4`, `pc 0x00470848 / 0x0047090d`). At call 1 the original holds the value at index 13 and the port at
   index 10 (`g_torqueRingPhase`, `Vehicle/VehicleControl.cpp:158-159`). Its consumer is not identified here.
   Also never set by the port at call 0: `+0x044 / +0x048` (-1 / 0) and `+0x13c / +0x140` (-0.16622 / 0.03475 vs 0).
6. **Default candidate, user decision.** `SSE` keeps the (b) pass set, D2 3/3, 13 tracks 0 RED. Knob stays default OFF.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `c7b40cf2 / 5b9905d8 / f680a529`; `H_DEF` `b40b47bc / fb690fb1 / d3a20f4c` | PASS |
| G-PTS | `UTBQ1SSE` spawn-pass and first-call wheel points and up = `o_bq1` | PASS |
| G-DRIFT | call 21 1.202e-4 (unchanged) | FAIL (next: U-9760) |
| G-B | `B_SSE_K0/K1/K26` `dfa1413f / 8b2c2621 / 202554fa`: A 2/3, B 2/3, C 0/3 | PASS |
| G-D2 | `D2_SSE_1..3`: slip 0.1939 / 0.1941 / 0.1942 and 0.2545, driving-median 1960.31 / 1962.62 / 1963.27, all in; H1 398/400 | 3/3 PASS |
| G-13 | 39 frames, 867 rows, 867 cited, 0 RED, instrument gates PASS (`D:/MashedWT/u9759_data/tracks_sse`) | PASS |
| G-CTRL | `H_SSE` `bb05716c / 47b28f48 / ac62cd14` | reported |

## 2. Code

- `Math/RwV3dTransformPointsCPU.cpp`: `MASHED_RW_SSE_POINTS`, the per-component order of `0x004dc370`. It covers every
  slot-`+0xc` user (wheel points, car-vs-world body points, contact query, `Race/PathNodes.cpp`).
- `Collision/CollisionWorld.cpp` (diagnostic): `MASHED_BATCH_DUMP_SLOT=<n>` and a `vel` column (record `+0x9c8..+0x9d0`).
- `re/frida/scenario_launch.py` (diagnostic): the batch probe's Q row adds a `rows` column (the dst half's 12 rotation
  dwords) and records the live vector-module slots once.

## 3. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9759 RESOLVED; new U-9760 (call-1 velocity, force words, the torque ring phase); hooks.csv notes on `004c3d90`,
`004dc370`, `0046c5f0`; one CHANGELOG line.

## 4. User decision needed

`MASHED_RW_SSE_POINTS` default ON (opt-out at the flip)? The live RW points transform; (b) pass set, D2 and the 13 tracks
hold; G-CTRL would move to `bb05716c / 47b28f48 / ac62cd14`.
