# PREREG u9754: the spawn per-wheel ground normal (2026-10-18)

Branch `work/u9754` from `race/first-frame-parity` `e7bc39c3b` (default G-CTRL `b1f49e7a / b1be0054 / d3a20f4c`). Model
claude-opus-5-5, account2. Private root `D:\MashedWT\u9754_root`. Written before any knob arm.

## 0. Measured before this PREREG

- Original timeline (`nseed.py` on `o_wobj2` and the new `o_seed1`, car 1): frame 1 after spawn all four wheel normals
  `+0x200..+0x208` (stride 0xc4) = (0, 1, 0); frame 2 = (0, 0.999462068, -0.0312347524) on all four; held through the
  countdown; the front pair changes at window call 15, the rear pair at 22, as each wheel meets a new triangle.
- Writer (`o_seed1`, `scenario_launch.py --watch-write 204,95c` on car 1, `o_seed1.msd.watch.csv`): the seed is written
  at `0x0046cff8` (inside the classifier `FUN_0046cc40`, which copies the batch's face normal), return chain ending
  `0x0046f827` (after the walk call `0x0046f817` in `FUN_0046f6c0`), value 0.9994620680809021. So the seed is the walk's
  face normal for the triangle under the car at spawn.
- The port's walk replica gives the original's normal bit for bit on later contacts (frames 15 / 22, lk21 `nrm.py`).
- Port timeline (lk21 `B_SPLSBUTR1_K1.rec.msd`, `pseed.py`): (0, 0.999512017, -0.0312363114) on all four from call 0,
  the same contact-refresh frames as the original afterwards. That value is the port-only triangle-soup probe
  `ProbeGround`'s face normal: `SolveWheelContacts` (`Vehicle/VehiclePhysicsRun.cpp`) fills `g_cNormal` from it, runs the
  solver, then `ReassertContacts` writes `g_cNormal` over the four wheel normals whenever `g_cGrounded`.
- On the original, scaled per call: the port's exact normal and the original's differ by a uniform factor on later
  triangles too (frame 37: 0.99996734; frame 53: 1.0000212), from the LUT normalize in the walk.

## 1. Hypothesis

H1: `ReassertContacts`'s normal write at spawn replaces the classifier's walk normal. Without it the port seeds the
original's (0, 0.999462068, -0.0312347524), and car 1's force / position drift from call 1 (lk21) shrinks.

## 2. Knob and arms

`MASHED_REASSERT_KEEP_NORMAL=1` (default OFF): `ReassertContacts` keeps the state / grounded-count writes and skips the
three normal writes. Arms (1200 frames, K0 / K1 / K26), on lk21's `SPL` (`S` + `MASHED_PATHPROG_LATE`): `SPLKN`, `SPLKNR1`
(+ car-1 record dump); on the default: `KN`.

## 3. Gates

| gate | rule |
|---|---|
| G-NOOP | knob off: `B_DEF_K0/K1/K26` `e332af0c / 01ad5975 / 772f9fb7`, `B_SPL_K1` `a8f11851` |
| G-SEED | `SPLKNR1` car 1 wheel normals at call 0 = (0, 0.999462068, -0.0312347524) bit for bit, all four |
| G-FORCE | `SPLKNR1` car 1 `+0x210..+0x21c` at call 1 equal o_wobj2 (lk21: ratio 1.00005) |
| G-DRIFT | `SPLKN` K1 car 1 x/z gap at call 21 < 1e-5 (lk21: 1.2e-4); first look difference later than 21 |
| G-B | (b) on `KN` (default + knob): no car that passes on the default turns to FAIL; `SPLKN` reported |
| G-D2 / G-13 | only for a default proposal: D2 3/3, 13 tracks 0 RED (the knob touches the player too) |

If G-SEED passes and G-DRIFT fails, report the next first-differing record field (`newdiff.py`), no default proposal.

## Amendment 1 (2026-10-18, after the section-2 arms, before these)

Section-2 result (exe `eb8c3e44`): G-NOOP PASS. G-SEED PASS (`SPLKNR1` car 1 call 0: (0, 0.999462068, -0.0312347524) on
all four wheels, refresh at 15 / 22 as the original). At call 1 `+0x210` is now exact and `+0x21c` 0.0078 off (lk21:
0.126); the call-1 words at half the original's value (`+0x39c`, `+0x460`, `+0xad4`) trace to the spawn-basis words
`+0x220 / +0x2e4 / +0x3a8 / +0x46c` (8.74e-8 / 4.37e-8), which lk21's `MASHED_SPAWN_BASIS_RW` fixes. New arms, same gates:
`SPLSBKN` (`SPL` + `MASHED_SPAWN_BASIS_RW` + this knob), `SPLSBKNR1` (+ car-1 record), `SBKN` (default + both knobs).

## Amendment 2 (2026-10-18, after the amendment-1 arms, before any D2 / 13-track arm)

Amendment-1 result: `SPLSBKN` G-DRIFT FAIL (call 4 2.4e-7, call 21 1.2e-4); next call-0 candidate the spawn y `+0x95c`
(7 ulp, `FUN_0046d400`) and the port's single-pose `+0x968` half. G-B: `KN` alone fails B car 1 (`c1#` 16 [17, 46]), so
it is not a default candidate; `SBKN` (both knobs) keeps the default's pass set (A cars 2, 3; B cars 1, 2; C 0/3). The
default candidate is `SBKN`: D2 `D2_SBKN_1..3` (each alone, bounds of d2close / d2ref: slip 1500-2000 [0.18855, 0.19635],
2000-2600 [0.24488, 0.25487], driving-median [1904.70, 1982.44], H1 >= 200/400, >= 1600 motion_diag lines) and the 13
tracks (`run13.py`, 0 RED against `cites_rres.json`), then G-CTRL `H_SBKN`.

## Amendment 3 (2026-10-18, after D2_SBKN_1..3, before these)

`D2_SBKN_1..3`: slip 0.1890 / 0.2544 in band, H1 398/400, driving-median 1997.89 / 2002.84 / 1996.54 OUT (> 1982.44) on
3/3, so `SBKN` is not a default candidate. Attribution only (no default claim): `D2_SB_1` and `D2_KN_1`, one run each.
