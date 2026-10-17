# PREREG u9749: the first A4 frame after a contact skip (2026-10-17)

Branch `work/u9749` from `work/a4order` `99d6e17d5` (the U-9749 measurements need `MASHED_A4_ORDER`, still default OFF
there). Model claude-opus-5-5, account2. Written and committed before any knob arm runs.

## 1. Measured so far (no knob, diagnostic dump only)

New diagnostic `MASHED_RECDUMP` / `MASHED_RECDUMP_A4` (MSD1 record dumps, default OFF). On `B_SA_K1`, with the dump on for
slot 0 and slot 2, the step dump stays `75255391` (= a4order `B_SA_K1`), so the dump changes nothing.

`recdiff.py` / `fieldtrace.py` against `o_pw1.msd` (player record), frames aligned on the first frozen coast (original k = 23
1237.908, port dump index 23 1237.921, shift 0):

- at k = 31 (last coast frame) position and linear velocity match (x 0.3035), but the forward vector does not:
  `+0x9d4` -0.05019 / -0.07133, `+0x9dc` -0.99879 / -0.99745 (orig / port);
- the yaw rate the contact left, `+0x148`, is bit-identical on k = 22..33 (0.09344, x0.9345 per frame), `+0x9bc..+0x9c4` are 0
  on both sides through k = 31;
- the first heading step after the contact is equal (-0.00888 on forward x, k 22 -> 23); from k 23 -> 24 to k 30 -> 31 the
  original turns a constant 0.00264 (+-0.00002) per frame less than the port, about 0.151 deg per frame.

A constant-rate rotation with the yaw rate unchanged is the form of the steer-0 heading assist `FUN_0046c7d0` (rotation of the
live matrix about (0,1,0), `0x0046cb10` -> `FUN_004c4d20`), which `FUN_00424eb0` runs for cars 0..3 between the AI tick
(`0x00425a73`) and the physics (`0x00425a8d`). The port runs it for slots 1..3 only (U-9743: the player form is unported).

## 2. Knob `MASHED_HEADING_ASSIST_PLAYER=1` (default OFF): mask bit 0x2

`Race::HeadingAssist_Player`: `FUN_0046c7d0(0, dt)` with the same gates as the AI loop (`FUN_0040e370`, `FUN_0046c7b0`), the
player's steer bytes of the frame (the bytes `StepCar` maps `io.steer` to), called right before `VehiclePhysics_StepPlayer`.
Everything else in `AssistCar` is the verbatim AI-slot transcription.

## 3. Arms and gates (`run_arms.py`, tags: `P` = the knob alone, `AHP` = A + P, `SAH` = SA + P, `SAHR0` = SAH + slot-0 record dumps)

| gate | measure | pass rule | expectation |
|---|---|---|---|
| G-NOOP | knob off on the new exe: `B_SA_K1` step dump; `H_DEF` G-CTRL | `75255391`; `490c4b2d / 1fdd0ea6 / 46088ac4` | PASS |
| G-HEAD | `B_SAHR0_K1`, player forward x on k = 24..31 | every per-frame step within 2e-4 of the original's; `+0x9d4` at k = 31 within 0.002 of -0.05019 | PASS |
| G-SPEED1 | same, player `+0x9e4` at k = 32 | within 2.0 of 1214.104 (port now 1231.356) | PASS if the heading was the whole cause; not certain |
| G-COAST2 | `B_SAH_K1`, car 2 `rec_9e4` at k = 60..70 | a constant run of >= 8 calls starting at k in [60, 62] (original 1341.140 on 61..69) | not predicted |
| D3 (b) | `P`, `AHP`, `SAH` against `DEF`, `A`, `SA`, K0 / K1 / K26 | default-ON candidate only if no car that passes on DEF turns to FAIL | not predicted |
| D2 | `D2_P_1..3`, each alone | slip [0.18855, 0.19635] / [0.24488, 0.25487], driving-median [1904.70, 1982.44], H1 398/400, >= 1600 lines | not predicted: the D2 recipe holds steer, the assist acts only on steer-0 frames |
| 13 tracks | `run13.py` with P | 0 RED, rows reported against 867 | not predicted |
| G-CTRL | `H_P` | reported | moves |

A D2 run under 1600 lines or a piz-collision AV is re-run, not scored.
