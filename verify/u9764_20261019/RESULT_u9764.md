# RESULT u9764: car 1's wheel 1 force x `+0x2d8` at call 3 (2026-10-19)

Branch `work/u9764` from `67cdd752f` (default G-CTRL `3b3690f6 / 9a657924 / a386f43b`). PREREG `PREREG_u9764.md` (`3c5f5b021`),
amendment 1 (`96200beea`), amendment 2 (`87eb8dcca`). Exe `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\u9756_root`.
Original references: o_wobj2 (unprobed), o_w2d8 (write watch, matches o_wobj2), o_aq4 (A6a probe, matches o_wobj2). Model
claude-opus-5-5, account2. This file replaces the partial RESULT of `8eb8c0fe0`, whose "axis y" cause is withdrawn (section 1).

## 0. Verdict

1. **Cause found: `RwMatrixRotate`'s axis normalize.** `0x004c4d20` normalizes the axis with the LUT inverse sqrt
   (`call 0x004c3b90` at `0x004c4d5d`). The port's `Math/RwMatrixRotate.cpp` calls `FastInvSqrt`, which in the standalone has no
   RW LUT and returns `1/sqrt(x)` (`Math/RwSqrt.cpp`). In A6a's spin branch (`0x0046805a` rotate about `+0x9bc`, `0x00468072`
   vectors transform, `0x00468077..0x004680ce` `dst0 * f - +0x9b0`) that made the port's `le0` at call 3 `0.0337664969`
   against the original's `0.0337665901`. The port's `dst0 * f` is larger by `1.000026`, the ratio of the LUT length `+0x9e8`
   (`5.62020905e-05`) to the exact `|av|` (`5.62006207e-05`).
2. **`MASHED_ROTATE_LUT=1` (default OFF)** normalizes through `LutInvSqrt` (`Math/RwLutTables.h`, the original's table). Car 1's
   A6a block #4 then equals o_aq4 at every term through call 4, and `+0x2d8` equals o_wobj2 at calls 3 and 4 (`-28.2014942`,
   `-44.7293854`). (b) is unchanged (A 2/3, B 3/3, C 0/3), 13 tracks 0 RED, but **D2 0/3**: slip1500 `0.1826` is under the
   band floor `0.18396`. Not proposed for default.
3. **`MASHED_RW_LUT_ALL=1` (default OFF)** hands every `RwLutGuard` caller (`FastSqrt`, `FastInvSqrt`, `RwV3dNormalize`,
   `Vec3Magnitude`, `RwV2d`) the in-process tables when no engine LUT is live. Car 1's record equals `ROTATE_LUT`'s through call
   7 (H3 refuted at car level), D2 the same 0/3. Not proposed.
4. **Next gap, call 4 (new U-9765):** on `ROTATE_LUT` the original's body matrix `up.y` (`+0x93c`) is `1.00000024` where the
   port's is exactly 1, and `at.x / at.z` (`+0x948 / +0x950`) are 1 ulp off. `recpos.py` is unchanged by either knob
   (velocity differs from call 5, position from call 7), so the call-3 `+0x2d8` gap did not carry into motion.

## 1. Probe correction (amendment 1)

The first A6a probe (`Interceptor.attach` at `0x00468320`, run o_aq1) perturbed the original. A mid-function attach swaps `[esp]`,
which inside A6a is a live local (memory frida-interceptor-is-entry-only). o_aq1's car 1 tilts at call 1 (`+0x92c` `2.66e-08`)
where o_wobj2 and o_w2d8 hold 0, and its `la8` reading was the artifact behind the withdrawn "steered axis y" cause. A
hardware write watch on the axis (`+0x2e4 / +0x2e8`, Phase 0 stores at `0x0046de6e / 0x0046de7a`) froze car 1 from frame 2 in
two runs (velocity 0, 4 distinct payloads), so that watch is unusable there. The fixed probe hooks the entry of `0x004c3ac0`,
filtered on return address `0x00468343` and argument `rec + 0x268 + 0x70`, and reads A6a's frame at entry `esp + 8`. Its run
o_aq4 differs from o_wobj2 only in the torque-ring history block (`+0x1ac..+0x1e8 / +0x270..+0x2ac`), the same pattern as the
watch-only control o_w2d8.

## 2. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` on every build | PASS |
| G-ADD (H1, `MASHED_A6A_LC0`) | `XW1LC` addend at call 3 `30.5746307` (unchanged); `B_LC` `8f2b9c37 / 95317a93 / 2742fa2a`, (b) unchanged | FAIL |
| G-LE0 (H2, `MASHED_ROTATE_LUT`) | `B_XW1RL_K1`: block #4 = o_aq4 through call 4; `+0x2d8` = o_wobj2 at calls 3-4 | PASS |
| G-POS | `recpos.py` against o_wobj2: velocity from call 5, position from call 7 on default, `RL` and `LA` | reported |
| G-B | `B_RL` `be7cf4de / 01cd8271 / 71510349`; `B_LA` `4c79de78 / 01cd8271 / 71510349`: A 2/3, B 3/3, C 0/3 | PASS |
| G-D2 | `D2_RL_1..3` and `D2_LA_1..3`: slip1500 `0.1826` OUT, slip2000 `0.2524` in, drvmed `1946.76` in, H1 398/400, 1635 lines (default: 0.1859 / 0.2534 / 1989) | 0/3 FAIL |
| G-13 | `RL`: 39 frames, 869 rows, 869 cited, 0 RED, instrument gates PASS (`D:/MashedWT/u9764_data/tracks_rl`) | PASS |
| G-CTRL | `H_RL` `9d46fb3c / 5b01b865 / 17c51149` | reported |
| G-REC (H3, `MASHED_RW_LUT_ALL`) | car 1 record = `RL`'s through call 7 | FAIL |

`H_LA` and the `LA` 13 tracks were not run: D2 rules `LA` out as a default candidate.

## 3. Code / tools

- `Math/RwMatrixRotate.cpp`: `MASHED_ROTATE_LUT`. `Math/RwLutGuard.h`: `MASHED_RW_LUT_ALL`. `mashedmod/asi_sources.rsp` links
  `Math/RwLutTables.cpp` into the .asi.
- `Vehicle/Integrate2.cpp`: `MASHED_A6A_LC0` (H1, refuted, kept default OFF); the split log gains `lac / la8 / la4`,
  `le0 / ldc / ld8`, the axis, `s0 / s1`, `lc8 / lc4 / lc0'` (`A6aWheelDump::lc / s`).
- `re/frida/scenario_launch.py`: `--a6a-probe-wheel W` (callee-entry form, section 1).
- Evidence in `D:/MashedWT/u9764_data` (`MOVED_OFF_GIT.txt`).

## 4. Next (U-9765)

Find the writer of car 1's body matrix at call 4 (`+0x93c` `1.00000024` in the original) with `--watch-write 93c` (check the
run against o_wobj2 first, section 1) and compare its arithmetic with the port's.
