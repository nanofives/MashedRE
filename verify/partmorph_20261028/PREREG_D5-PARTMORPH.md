# PREREG D5-PARTMORPH: car part 0x12 morph and part 0x14 wobble (D-11077, L-45)

Written before any arm. Branch `orch/D5-PARTMORPH` from `orch/integration` d92bb3a5e.
Hard constraint: the original MASHED.exe does not boot (no audio endpoint, AV 0x005baf48), so no new
original-side capture is taken. Original-side evidence is the static decode (Ghidra pool clone through
`re/tools/decomp_pc.py`, capstone on `original/MASHED.exe.unpatched`) and captures already on disk.

## 1. Static decode (the law to port)

`FUN_0041f770` @ `0x0041f770`, called once per car per frame from `FUN_00420870` (`0x00420870`, call at
`0x0042091e` area, after `FUN_0041f710`), `in_EAX` = the car record (`0x0063d9e0 + v*0x2ac`):

- Gate: body runs only when `rec+0x298 == 0` (`0x0041f770`).
- Morph: `FUN_0046bda0(&s, rec+0x290, 0)` (wheel 0 steer mean: 16 floats summed from 0.0, times
  `[0x005cd058]` = 0.0625). `s *= [0x005cc9a0]` (0.05). Clamp: `s < 1` keeps `s` (then `s < -1` gives
  `[0x005cc33c]` = -1), else `[0x005cc320]` = 1. `pos = clamp * [0x005cc32c] (0.5) + 0.5`.
  `FUN_00544bf0(rec+0x48 = part 0x12 atomic, pos)`.
- `FUN_00544bf0` @ `0x00544bf0` (RpMorph-style atomic time set on the atomic interpolator): plugin slot
  `DAT_007dc66c + atomic`, current interpolator copied into atomic+0x50 start (s16), +0x52 end (s16),
  +0x54 time, +0x58 = `[0x005cc320] / time`, +0x5c = `pos`, flags +0x4c |= 3, `FUN_004c0e50` on the frame.
- Load setup, `FUN_00420420` @ `0x00420420` (`iVar5 == 0x12` branch): `FUN_005449f0(geo, 1)` (one
  interpolator, 0x14 bytes each, at `DAT_007dc670 + geo`), `FUN_00544a70(geo, 0, 0, 1, 1.0f)`
  (start 0, end 1, time 1.0, 1/time 1.0, next = self), `FUN_00544ad0(atomic, 0)` (current = 0,
  position 0). So start = morph target 0, end = morph target 1, recipTime = 1.0.
- RW D3D9 instance path `0x004eaef4..0x004eb05c`: start/end read from atomic+0x50/+0x52 (both
  forced to 0 when either is >= numMorphTargets), `scale = [atomic+0x5c] * [atomic+0x58]`
  (`0x004eaf13`/`0x004eaf16`); when start != end and scale != 0.0 it calls `FUN_004f6870` for the
  positions (morph target +0x14) and, with flag 0x10, the normals (+0x18). `FUN_004f6870` case 2
  (FLOAT3): `dst[k] = (end[k] - start[k]) * scale + start[k]`, per component, normals not renormalised.
- Wobble: `a = FUN_00422470(DAT_0063e4a4, 0xff) - [0x005cc32c]` (0.5), stored float.
  `FUN_004c1520(part 0x14 frame (rec+0x50 -> +4), &DAT_006146fc (0,1,0), a * [0x005cc72c] (30), 0)`
  (REPLACE, so the frame position becomes 0), then
  `FUN_004c1520(same, &DAT_006146f0 (1,0,0), a * [0x005cc9b0] (15), 1)` (PRECONCAT).
  `DAT_0063e4a4 = param_1 * [0x005cd058] (0.0625) + DAT_0063e4a4` (`0x0041f853`/`0x0041f859`, the
  only writer; never reset).
- `param_1` (from `FUN_00420870`, `0x00420950..0x00420990`): `d = |P - prev|^2 * [0x005cc9e0] (25)`,
  P = `param_3` rows 12..14 (the car matrix `FUN_0040da50` gets from `FUN_0046d4a0`), prev =
  `0x0063d8e0 + v*0x40` rows 12..14. `d >= 1` gives 1, `d < 0` (or unordered) gives 0. prev is
  overwritten with the whole 16-dword `param_3` at the end of every `FUN_00420870` call
  (`0x00420b2x` loop), whatever `rec+0x298` is.
- `FUN_00422470` @ `0x00422470` (disassembly, args x at [esp+4], mask at [esp+8]):
  `i = (int)FUN_004a2c48(FUN_004a3090((double)x))` (CRT floor, then `__ftol`);
  `fi = (float)i`, + `[0x005cc94c]` (2^32) when i < 0; `t = (float)(x - fi)`;
  `r1 = FUN_004223f0(ecx = i + 1, eax = mask)`, `r0 = FUN_004223f0(ecx = i, eax = mask)`;
  `return FUN_00422440(r0, r1, t)`.
- `FUN_004223f0` @ `0x004223f0` (ecx = n, eax = mask): `if (mask) n &= mask; e = (n << 13) ^ n;
  c = (e*e*0x3d73 + 0xc0ae5) * e - 0x2df722f3; c &= 0x7fffffff; return (float)c * [0x005cd314] (2^-31)`
  (uint32 arithmetic).
- `FUN_00422440` @ `0x00422440`: `w = (1 - fcos([0x005cd310] (pi) * t)) * 0.5; return (1 - w) * a + w * b`.
- Where `rec+0x298` is 0: writers are `FUN_0041ef60` (setter) and `FUN_00420420` (load, 0x00420482 = 0).
  `FUN_0041ef60` callers: `FUN_00448220` (post-race result camera: all four cars set to 0) and
  `FUN_00446520` (car 0 set to 0 when `FUN_0042f6a0() == 2` or `DAT_007f0fd0` is 5 / 10 / 9; else 1 / 2).
  `FUN_00420870` shows parts 0x12 and 0x14 only when `rec+0x298 == 0` (switch cases 0x12 / 0x14).

Asset facts (all 13 vehicle piz, re-derived by the RESULT's scan script): every livery DFF (78) has
part 0x12 (two morph targets, 504 or 524 vertices) and part 0x14 (one morph target). ADVANTAGE0.DFF:
part 0x12 = atomic frame 23, part 0x14 = atomic frame 82 (frame position about 0; Formula 0.0099 in y).

## 2. Port (one default-OFF knob)

- `MASHED_PARTMORPH=1` (default unset = OFF): per car per frame in the draw loop's `FUN_00420870`
  order, compute `param_1`, run the gated body (morph position for the car's instance, part 0x14 frame
  pose added to the pose list, `DAT_0063e4a4` accumulator), store prev. RaceSubmit lerps the part 0x12
  atomics' vertices and normals per instance (`FUN_004f6870` case 2 form) and re-instances them.
- DffModel keeps morph target 1 (positions and normals) per batch, baked and de-indexed like target 0.
  Parsing it does not change what draws while the knob is OFF.
- The port's record state (`LibRw::CarLodState`) has no state-0 producer (`FUN_00448220`,
  `FUN_00446520`'s car-0 branch), so with the knob ON and nothing else the law never runs.
  Diagnostic only, default unset: `MASHED_DBG_PARTSTATE0=<slot>` forces that slot's record state to 0
  so the law runs on the standalone. `MASHED_DBG_PARTMORPH=<csv>` logs the law's inputs and outputs.
  `MASHED_PARTMORPH_SELFTEST=<out>` writes the `FUN_00422470` port on an x grid once.

## 3. Arms and gates (standalone only)

Runner: `verify/partmorph_20261028/run_arms.py` (copy of `verify/savelink_20261023/run_arms.py`, paths
changed, one stack added). Private root `D:/MashedWT/D5-PARTMORPH_root`, raw data in
`D:/MashedWT/D5-PARTMORPH_data`, every boot under `mashed_lock.py run --label D5-PARTMORPH`.

- G1 default unchanged: `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this branch's exe equal
  `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref` (5943c5f1/364350ae/52052890,
  3a03670c/9fcdbd3a/53778664). FAIL = any hash differs.
- G2 knob ON does not move the sim: `B_PMF_K0` (stack PMF = knob + force slot 0 + log) step hash equals
  `B_DEF_K0`'s. FAIL = differs (the change is render only).
- G3 noise law: the self-test output equals `noise_ref.py` (Python model of the section 1 instruction
  stream; integer hash exact, float32 steps, cos in double) on every grid point to <= 1 ulp. FAIL = any
  point over 1 ulp or any integer mismatch.
- G4 law replay: on every `B_PMF_K0` log row with state 0: `pos` recomputed from the logged steer mean,
  `a` from the logged accumulator, the accumulator step `acc' = acc + moved * 0.0625` and `moved` from
  the logged positions all match to <= 1 ulp; rows with state != 0 leave the accumulator unchanged.
  Logged part 0x12 vertex 0 after the lerp equals `(v1 - v0) * pos + v0` from the DFF to <= 1 ulp.
  FAIL = any mismatch, or fewer than 600 state-0 rows.
- G5 dormancy: `B_PM_K0` (knob only, no force) logs 0 rows with state 0.
- Witness W1 (existing original captures, `verify/librwrest_checks_20261016/orig_*`): count rows where
  `rec+0x298 == 0`; part 0x14's modelling on rows with `rec+0x298 != 0` must be the bind (identity
  rotation). This is a consistency check only: if no state-0 row exists, the captures cannot witness
  the wobble values, and the RESULT says so.
- Visual (illustration, not acceptance): one standalone frame with the force on, knob ON vs OFF, the
  car-0 region cropped.

Verdict GREEN = G1..G5 pass. A new original capture is out of scope (would set the row blocked
"audio endpoint: needs an original capture").
