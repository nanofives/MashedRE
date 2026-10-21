# RESULT u9777: the player's call-128 split is A5's rubber-band gate reading a zero stand-in (2026-10-20)

Branch `work/u9777` from `5403ba91f` (cpcflip default). PREREG `PREREG_u9777.md` (`4a164c77c`), committed before any knob arm.
Exe `07394eb0` (`MASHED_NO_DEPLOY=1`, copy `D:\MashedWT\u9777_data\exe_07394eb0.exe`), private roots `D:\MashedWT\u9777_root`
/ `u9777_root13`. The final build `a22665c3` (one comment added in `ForceIntegratorStubs.cpp`) repeats the K0 step dumps
(`c4c2ae26` knob unset, `7ab70c5f` knob on). Evidence `D:\MashedWT\u9777_data` (`MOVED_OFF_GIT.txt`). No default changed.

## 1. First diverging operation: A5 (FUN_0046ddb0) Phase 3, the FUN_00442c80 gate

New original capture `o_a5_1` (the `o_bq0_*` recipe + `--a5-probe 1600 --wheelstate-probe`): same Q-row split against the port
as `o_bq0_1` / `o_bq0_2` (port Q row 260). New port marks `a5_in` / `a5_out` on `MASHED_D2SINK` (`Vehicle/VehicleControl.cpp`,
diagnostic only, step dump unchanged `c4c2ae26`). `a5vcmp.py` compares A5's entry / leave +0x9b0..+0x9b8 bit for bit:

- 85 consecutive player A5 calls equal (entry and leave) from the first moving call.
- The 86th (port frame 129, original sdframe 936, the A5 of the frame that produces record 128): entry bit-equal, leave differs
  (z c480cf0a / c480a105). Leave / entry on x and z: 0.998606 original, 0.997213 port. The original's `1 - sigma` = 1.3935e-3 is
  half the port's `l70 * base` = 0.557365 * 0.00500054 = 2.787e-3 (`a5g_diag`). The previous call matched to 1e-9.

From MASHED.exe.unpatched (capstone):
- FUN_00442c80 returns 1 when FUN_0040e350() == 6, DAT_007f0fd0 is not 4 / 8 / 9 and `[car*4 + 0x008989b0] > 7.0`
  (0x00442ca2 `fld`, 0x00442ca9 `fcomp [0x005cc9b8]`, 0x40e00000 = 7.0). A5 then halves local_70 when more than 2 cars are
  active (port `ForceIntegrator.cpp:206-211`). That is the halving measured above.
- FUN_00442ce0 (A5's grip multiplier) tests `[car*4 + 0x008989b0] == 0.0` (0x00442d0f, `0x005d757c` = 0.0) and reads the
  reference car from `[0x008989c8]` (0x00442d2f) for FUN_0046dbe0.
- 0x008989b0 / 0x008989c8 are FUN_00442a60's refdist array and reference car. The port's producer (`Race/SpectatorDistances.cpp`,
  default ON, end of frame) writes those addresses. The port's exe-only copies of the two leaves read `g_rubberBand[16]` /
  `g_rubberRefCar` (`Vehicle/ForceIntegratorStubs.cpp`), zero with no writer: the gate never fires and the multiplier always
  takes the refdist == 0 branch with reference car 0.
- The port's refdist trace (`MASHED_REFDIST_TRACE`) has the player passing 7.0 at producer row 128 (7.025663), which is the
  call-128 frame. The crossing is not near an ulp boundary.

## 2. Knob `MASHED_A5_RUBBER_LIVE=1` (default OFF)

The two leaves read the live 0x008989b0[car] / 0x008989c8 (`ForceIntegrator.cpp` `RubberRefDist` / `RubberRefCar`). Nothing
else changes. With it (`B_P0SR_K0`):

- P1 holds: the player's A5 is bit-equal for 136 calls (was 85); the Q rows hold to port row 443 (was 259).
- Record position and velocity on K0, bit-exact through: player 219 (was 127), car 1 219 (was 179), car 2 203, car 3 203
  (was 179). Car 2 against two new original captures (`o_c2_2`, `o_c2_3`, `o_c3` regime by `regcmp.py`; `o_c2_1` rolled at 62
  and is discarded). Cars 2 and 3 split together at 204, which is the K0 contact schedule (2+3 at 205, memory
  contact-schedule-from-override-onsets). Car 3's refdist first passes 7.0 at row 665, so the 204 split is not this gate.
- The player's next A5 difference (port frame 180) is 1 ulp in a denormal vy at A5 entry (80009504 / 80009503), so it comes
  from frame 179 after A5. Not chased in this lane.

## 3. Gates

| gate | knob unset (G-NOOP, exe `07394eb0`) | `MASHED_A5_RUBBER_LIVE=1` |
|---|---|---|
| G-CTRL | `b9fb3f25 / 8740221b / b2e4b612` (= cpcflip default) | `15594ec8 / 7285d708 / 26a6e4ad` |
| B dumps K0 / K1 / K26 | `c4c2ae26 / b0f27763 / 6c35e33b` (= default) | `7ab70c5f / 15262e4a / d1b4bc64` |
| `flag_a368` per K | 0 / 0,1 / 0,2 | 0 / 0,1 / 0,2 (unchanged) |
| (b) | A 2/3 (car 3 FAIL), B 2/3 (car 2 FAIL), C 3/3 | **A 3/3**, B 2/3 (car 2 FAIL, same values), C 3/3 |
| D2 x3 alone | (cpcflip: 0.1900 / 0.2550 / 1963.95..1964.25) | 0.1900 x3 / 0.2550 x3 / 1965.35, 1964.25, 1964.25, H1 398/400: PASS 3/3 |
| 13 tracks | 868 rows, 0 RED, byte-identical score to cpcflip | 873 rows, all cited, **0 RED**, instrument gates PASS |

P3 holds (D2 inside the band; its scored values equal the default's). P4: (b) A car 3 FAIL -> PASS, nothing else moves in (b).
13-track row changes with the knob: one more cited row (fewer matched triangles) on 01_Egypt 780, 03_Arctic 1051 / 1091,
05_Sands 1084, 07_Roundabout 1091 / 1131; one fewer on 13_Training 959 (21 -> 20, matched 17231 -> 17423). Net +5, all cited.

By the D3 rule the knob passes every gate measured here (A 3/3, B unchanged, C unchanged, D2 3/3, 0 RED, G-CTRL knob-off
unchanged). Flipping it is Mariano's decision; this lane flips nothing.

## 4. Next

- New U-9778: cars 2 and 3 split at call 204 on K0 with the knob (the 2+3 contact at 205); the player and car 1 at 220.
- The player's 1-ulp denormal vy at port frame 180 (after A5 of frame 179).

Scripts: `a5vcmp.py` (A5 entry / leave, `--a5-probe` vs `MASHED_D2SINK` a5 marks), `run_arms.py` (stacks `P0`, `P0S`, `RL`,
`P0SR`, `RLTR`, `RLR1..3`).
