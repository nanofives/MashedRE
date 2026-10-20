# RESULT u9776: car 3's call-149 ulp is RwMatrixCombine at PC_53; the next split is the player's call 128 (2026-10-20)

Branch `work/u9776` from `3880860f1` (trinflip default). PREREG `PREREG_u9776.md` + amendments 1-2, each committed before its arms.
Exe `095e90ca` (last build), `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`. Evidence `D:\MashedWT\u9776_data`
(`MOVED_OFF_GIT.txt`). No default changed.

## 1. Car 3's 1-2 ulp at call 149: FUN_004c52f0's device multiply runs at PC_53

New probe `scenario_launch.py --tri-probe`: entry + exit of FUN_004c52f0 from 0x00470004 (the 3-wheel alignment's combine at
0x0046ffff), callee-entry only; count gate 53066 calls / 4 hits, no error. Port twin: `MASHED_WCS_TRILOG` `.tq.csv`.
On `o_tq_2` (equal to `o_c3` through call 399) vs the port (`tqcmp.py`):

- the angle is the 2.5 clamp (`0x40200000`, 0x0046ffbd) on every alignment of every car, so the acos never reaches the matrix;
- call 149: angle, local_e0, axis, local_ac, m and the ring before the combine are bit-equal; the ring after is +1 ulp
  (at row x / y);
- FUN_004c52f0 op 2 calls the device multiply [slot+8] = 0x005cb2a0 (x87, `(a0*b0 + a1*b4) + a2*b8` per element, each op at the
  caller's precision). The port's `Math::RwMatrixMultiplyCPU` is its verbatim naked copy, run at the standalone's PC_53; the
  original runs at PC_24.

Knobs (default OFF): `MASHED_WCS_COMBINE_PC24` (PC_24 around the alignment's combine only) and `MASHED_MATMUL_PC24` (PC_24
inside the exe's device multiply for every RwpSolverMath2 caller). Both give car 3's ring at 149 bit-equal and identical step
hashes (K0 `c4c2ae26`, K1 `b0f27763`, K26 `6c35e33b`).

## 2. Which arm matches the original: K0

K26 rolls `flag_a368` (0x0089a368) = 2 at row 61, K1 = 1, K0 keeps 0; every `o_c3`-regime original capture keeps 0. So K0 is the
comparable arm past row 61 (K26 was the u9773 comparison). On K0 with `MASHED_WCS_COMBINE_PC24` cars 1 and 3 are bit-exact in
position and velocity through call 179 (car 3 split at 159 without the knob).

## 3. The call-180 chain, measured

- car 1 leaves mode 7 and car 3 brakes at 180 in the original: FUN_004148b0 (leader timer; new `--lt-probe`, port
  `MASHED_LT_LOG`) returns 1 for the reference car while the player (slot 0, the only slot state 1) is > 6.5 behind. The reference
  is FUN_00442a60's (refdist 0x008989b0), and it switches from car 1 to car 3 at 180 in the original, not in the port.
- The original's refdists match exact-sqrt distances of the aistep positions to 4 decimals. The port's player distance is 9.41
  vs 9.33, so the player is where they differ.
- The player's record is bit-exact through call 127 against both original variants (`recpos0.py`). At frame 987's first wheel
  classifier call its position and body up `+0x9c8` differ (up.y 0x3f7ffb82 / 0x3f7ffb9b); the second classifier call of frame 986
  is still equal (`--batch-probe` / `MASHED_BATCH_DUMP_SLOT=0`, `bqcmp.py`). The record at 127 differs only in `+0x9d4` (dump lag)
  and the `+0xaf4` / `+0xb2c..` rings (written at 0x0046e709..0x0046e74f, FUN_0046c5f0 on the scratch points 0x881560..; the port's
  entries are sign-flipped). `MASHED_A5_RING_ORDER` does not move the 128 split.

## 4. Surface jitter RNG (faithful, inert here)

0x0046e78c..0x0046e7c8: a wheel on surface key 0xffff32ff takes `FUN_00472650(-r, r)` twice (r = +0x9e4 * [0x5cea68] 5e-6) into
+0xb00 / +0xb08. FUN_00472650 = `lo + ((FUN_00534870() & 0x7fffffff) * 2^-31) * (hi - lo)` (0x00472650..0x00472682) on the shared
ring. The port's `Fi_RandRange` returned lo and drew nothing. `MASHED_FI_RAND` (default OFF) calls `Ai::Ai_RandRange` (AiRand).
On this recipe it changes no hash (no wheel reaches that surface before the splits).

## 5. Gates with `MASHED_WCS_COMBINE_PC24`

| gate | measured |
|---|---|
| (b) | A 2/3 (cars 1, 2; car 3 FAIL `c1_distinct 71 [21,70]`, `steer_distinct 97 [33,96]`), B 2/3 (cars 1, 3), C **3/3**. Default: A 1/3 B 2/3 C 2/3. A car 3 PASS -> FAIL; A car 1, A car 2, C car 2 FAIL -> PASS |
| D2 | 3/3: 0.1900 / 0.2550 / 1963.95..1965.35, H1 398/400; two logs byte-identical to the default's |
| G-NOOP | every exe, knobs unset: `B_TR3_K26` `040ce640`, `B_TR3_K0` / `B_LR3_K0` `28a76cf2` (= the trinflip default) |
| 13 tracks, G-CTRL | not run (not a flip) |

Not a default under the rule (A car 3 fails); user decision.

## 6. Other findings

- 2 of 8 original B captures (`o_lt1_1`, `o_bq0_1`) split from the others at car 1 call 150 / car 2 151 / car 3 159 with the same
  signature; the player is equal in both variants through 195. The original varies run to run here (U-9775 class).
- The refdist array differs by hundreds of ulp on cars whose positions are equal: the port's FUN_00442a60 uses std::sqrt where the
  original runs FUN_004c3ac0 (registered deviation, `Race/SpectatorDistances.cpp`). The 180 switch does not depend on it.

## 7. Next

New U-9777: the player's call-128 split (what differs between frame 986's second classifier call and frame 987's first, on slot 0:
the solver's second pass, the integrate, A5 / A6a / A6b of frame 987). Then A car 3 with the knob.
