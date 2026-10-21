# PREREG u9777: the player's (slot 0) call-128 split on the B recipe, K0 (2026-10-20)

Branch `work/u9777` from `5403ba91f` (cpcflip default). Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, private root
`D:\MashedWT\u9777_root`. Evidence `D:\MashedWT\u9777_data` (never committed). Runner `run_arms.py` (copy of u9776's).

## Step 1 (measurement, no knob) - done before this PREREG

Original `o_a5_1`: the B recipe of `o_bq0_*` (`--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-car 0
--statediff-aistep --hold 25 --batch-probe`) plus `--a5-probe 1600 --wheelstate-probe`. Its Q rows equal `o_bq0_1` / `o_bq0_2`
against the port: same first differing row (port Q row 260, cx bea593c5 / bea59385).

Port `B_P0S_K0`: the default plus the slot-0 logs (record, batch dump, A5GDIAG, A6a tail / vz / force split, WCS poslog) and
`MASHED_D2SINK` with two new marks `a5_in` / `a5_out` around A5 (`Vehicle/VehicleControl.cpp`, diagnostic only). Step dump
`c4c2ae26` = the K0 default, so the logs do not move the trajectory.

`a5vcmp.py` (A5 entry / leave +0x9b0..+0x9b8, original hex vs port float-exact): 85 consecutive A5 calls of the player equal
in entry and leave from the first moving call. At the 86th (port frame 129, original sdframe 936, the A5 of the frame that
produces record 128) the entry velocity is bit-equal and the leave differs (z c480cf0a / c480a105). x and z leave / entry is
0.998606 in the original and 0.997213 in the port: the original's `1 - sigma` (1.3935e-3) is half the port's (`l70 * base`
= 0.557365 * 0.00500054 = 2.787e-3, `a5g_diag`). The previous call matched to 1e-9.

Mechanism, from the binary (MASHED.exe.unpatched, capstone):
- FUN_00442c80 (A5's gate): returns 1 when FUN_0040e350() == 6, DAT_007f0fd0 not in {4, 8, 9} and
  `[car*4 + 0x008989b0] > 7.0` (0x00442ca2 `fld`, 0x00442ca9 `fcomp [0x005cc9b8]` = 0x40e00000 = 7.0). A5 then halves
  local_70 when more than 2 cars are active (port `ForceIntegrator.cpp:206-211`).
- FUN_00442ce0 (A5's grip multiplier): tests `[car*4 + 0x008989b0] == 0.0` (0x00442d0f, constant 0x005d757c = 0.0) and reads
  the reference car from `[0x008989c8]` (0x00442d2f) for FUN_0046dbe0.
- 0x008989b0 / 0x008989c8 are FUN_00442a60's refdist array and reference car. The port's producer (`Race/SpectatorDistances.cpp`,
  default ON) writes them at those addresses. The port's exe copies of the two leaves read `g_rubberBand[16]` /
  `g_rubberRefCar` (`Vehicle/ForceIntegratorStubs.cpp:38-39`), which are all zero and have no writer. So the port's gate never
  fires and its grip multiplier always takes the refdist == 0 branch with reference car 0.

## Knob (default OFF)

`MASHED_A5_RUBBER_LIVE=1`: `RubberBandGate` (0x00442c80) and `RubberBandGrip` (0x00442ce0) read the live 0x008989b0[car] and
0x008989c8 instead of `g_rubberBand` / `g_rubberRefCar`. Nothing else changes. No flip in this lane.

## Arms

1. `B_P0SR_K0`: the step-1 logs + the knob. Then `B_RL_K0`, `B_RL_K1`, `B_RL_K26` (knob only) for (b) with `flag_a368` per K.
2. If the player holds past 128: the next split of the player and of cars 1 / 3 on K0 (`a5vcmp.py`, `bqcmp.py`, `recpos0.py`).
3. D2 x3 alone (`D2_RL_n`), 13 tracks (`run13.py`), G-CTRL `H_RL` x1.
4. G-NOOP: the new exe with the knob unset gives G-CTRL `b9fb3f25 / 8740221b / b2e4b612` and B `c4c2ae26 / b0f27763 / 6c35e33b`.

## Predictions

- P1: with the knob the player's A5 leave at port frame 129 equals the original's, and the player's Q rows hold past port row
  260 against `o_a5_1`. Risk named in advance: the port's refdist uses std::sqrt (registered deviation, FUN_004c3ac0 in the
  original), so if refdist[0] sits within its ulp error of 7.0 at that frame the crossing can land one frame off. Report the
  crossing frame on both sides if P1 fails.
- P2: no step-dump change before the first frame where some car's refdist passes 7.0 or some car's contact count (record +8)
  becomes nonzero, since before that both readings give gate 0 and multiplier 1.
- P3: D2 (one car, MEASURE_SOLO) unchanged: with one car no refdist passes 7.0. If D2 moves, the rubber-band branch is reached
  there and that is a finding, not noise.
- P4: (b) and G-CTRL change (the AI cars also cross 7.0 behind the leader). No direction is predicted.
