# PREREG u9776: car 3 past call 157 and A car 2 with MASHED_WCS_TRINORMAL (default ON since trinflip) (2026-10-20)

Branch `work/u9776` from `3880860f1`. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`.

Step 1 (measurement, no knob): original `--tri-probe` (new: entry + exit of FUN_004c52f0 from 0x00470004, callee-entry only;
count gate `o_tqc_1` 53066 calls, 4 hits, no error) on the B recipe car 3, checked against `o_c3` with `origcmp.py`
(`o_tq_1` rolled at 61, discarded; `o_tq_2` equal through 399). Port: `MASHED_WCS_TRILOG` now also writes `<path>.tq.csv`, the
same fields at the same point. Arms `B_TR3_K0/K1/K26` (car 3 records + log) on the default. Compare every alignment call of car 3
(record calls 149 and 163 on `o_tq_2`) field by field: angle, local_e0, axis, local_ac, m, ring before, ring after.

Already measured on `o_tq_2`: the angle is the 2.5 clamp (`0x40200000`, `0x0046ffbd`) at every alignment call of every car, so
the acos result does not reach the matrix on these calls.

Prediction: the first differing field at call 149 names the step (local_e0 = FUN_0046c5f0 / the wheel points; axis = the cross;
m = FUN_004c4d20; ring_post with equal m and ring_pre = FUN_004c52f0). A knob for a fix comes with its own amendment.

## Amendment 1 (after step 1, before any knob arm)

Step 1 result: car 3's first alignment (record call 149, `o_tq_2` frame 992 vs `B_TR3_K26` row 0): angle, local_e0, axis,
local_ac, m and ring_pre bit-equal; ring_post +1 ulp at words 8, 9 (the at row x / y). On K26 car 3's position and velocity
are bit-equal through call 157 (`recpos.py`); the rotation rows carry the 1-2 ulp from call 149 and wheel 0's contact flips at
158. FUN_004c52f0 op 2 calls the device multiply [slot+8] = 0x005cb2a0 (x87, per-op rounding at the caller's precision). The
port's `Math::RwMatrixMultiplyCPU` is a verbatim naked transcription, run at the standalone's x87 PC_53; the original runs at
PC_24 (memory track-load-runs-at-pc24).

Knobs (default OFF): `MASHED_WCS_COMBINE_PC24` (PC_24 around the alignment's FUN_004c52f0 call at 0x0046ffff only) and
`MASHED_MATMUL_PC24` (PC_24 inside the exe's device multiply for every RwpSolverMath2 caller, 0x004c4600 / 0x004c52f0).
Arms: `B_TC3_K26` / `B_TM3_K26` (car 3 records + log), then `B_TC` / `B_TM` K0/K1/K26 for (b), then D2 x3 for any candidate.
Predictions: P1 with either knob, car 3's ring_post at call 149 equals the original's and car 3's record holds past 157 on
K26; P2 the narrow knob changes nothing before the first alignment of any car (G-CTRL step rows before it equal).

## Amendment 2 (after the amendment-1 arms, before any FI_RAND arm)

Amendment-1 arms: `MASHED_WCS_COMBINE_PC24` and `MASHED_MATMUL_PC24` both make car 3's ring at call 149 equal the original's
(`tqcmp.py`); the step hashes equal each other (`6c35e33b` K26, `c4c2ae26` K0). K26 rolls `flag_a368` = 2 at row 61, the
original `o_c3`-regime captures keep 0, so the matching arm is K0. On K0 with the knob cars 1 and 3 are bit-exact through
call 179 (car 3 split at 159 without it).

Chain behind the 180 split (all measured, `--lt-probe` / `MASHED_LT_LOG`, `--batch-probe` / `MASHED_BATCH_DUMP_SLOT=0`):
call 180's AI decisions (car 1 leaves mode 7, car 3 brakes) follow FUN_004148b0 (the leader timer) after FUN_00442a60's
reference switches from car 1 to car 3 in the original; the port's player (slot 0) distance is 9.41 vs 9.33, so the switch
does not happen. The player's record is bit-exact through call 127 (`recpos0.py`, both original variants `o_bq0_1` / `o_bq0_2`
agree); at 128 its position at frame 987's first classifier call differs. FUN_0046e6xx at 0x0046e78c..0x0046e7c8: a wheel on
surface key 0xffff32ff draws `FUN_00472650(-r, r)` twice (r = +0x9e4 * [0x5cea68] 5e-6) into +0xb00 / +0xb08; FUN_00472650 =
`lo + ((FUN_00534870() & 0x7fffffff) * 2^-31) * (hi - lo)` (0x00472650..0x00472682). The port's `Fi_RandRange` returns `lo`
and draws nothing from the ring (Vehicle/ForceIntegratorStubs.cpp).

Knob `MASHED_FI_RAND=1` (default OFF): `Fi_RandRange` calls `Ai::Ai_RandRange` (AiRand, FUN_00472650 on the verbatim
FUN_00534870 ring). It shifts the shared ring, so every later draw (the regime roll included) moves.
Arms: `B_FR0_K0` (player record), `B_FR3_K0`, `B_FR1_K0`, then (b) K0/K1/K26 and D2 x3 for the stack TC + FR.
Predictions: P3 with FR the player's record holds past 127 against `o_bq0_2`; P4 if the regime roll moves, the K that keeps
`flag_a368` = 0 changes (report it, do not pick K by the result).
