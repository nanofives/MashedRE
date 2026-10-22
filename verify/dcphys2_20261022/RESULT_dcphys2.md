# RESULT dcphys2: physics A-chain dual copies 0x00468980 (A6b), 0x0046ddb0 (A5), 0x00470670 (A4) are one body each (2026-10-22)

PREREG `verify/dcphys2_20261022/PREREG_dcphys2.md` (commit `1bc51b495`, before any knob arm). Branch `work/dcphys2` from
`72ec8a7d0`. Private root `D:\MashedWT\dcphys2_root`; evidence `D:\MashedWT\dcphys2_data` (`MOVED_OFF_GIT.txt`). Baseline (this
worktree, no change) exe `4db582db` / asi `163cfee5`; final exe `38623436` / asi `088160e4` (`D:\MashedWT\dcphys2_data\final`).

## Verdicts

| pair | faithful body | what changed | default (knobs OFF) vs original |
|---|---|---|---|
| A6b `0x00468980` | the shared body with `MASHED_A6B_ACOS_CRT` + `MASHED_A6B_NORM_LUT` | one TU (`Vehicle/AeroStabilize.cpp`) in both rsp files; leaf thunks (acos `AL_Acos` / live `0x004a3384`, rotate port / live `0x004c4d20`); the naked .asi copy deleted | not clean: 142 / 147 boosted airborne calls differ (rotation floats, 1..40 ulp) |
| A5 `0x0046ddb0` | the shared body with `MASHED_A5_RING_ORDER` + `DOT_ORDER` + `AIR_F32` + `SPIN_FILD` (+ `CONTACT_INT`) | one TU (`Vehicle/ForceIntegrator.cpp`) in both rsp files with A5* thunks; the three exe-only leaf ports moved verbatim to `Vehicle/ForceIntegratorLeaves.cpp` (exe only; 2 allowlist keys renamed); the .asi C++ copy deleted | not clean: 960 / 960 differ on track 0 (ring normal sign, `A5_RING_ORDER` OFF) |
| A4 `0x00470670` | the shared body with `MASHED_A4_DOT_ORDER` | one TU (`Vehicle/VehicleControl.cpp`) in both rsp files; the .asi calls A5 / A6a / A6b at their live RVAs with the original's register ABI and passes the original's +0x928 matrix; the .asi copy deleted | not clean: 204 / 1828 differ (slide `+0xb0c` and what follows) |

Every new knob is default OFF in the exe (FAITH not touched) and seeded ON in the .asi (`A6aAsiDefaults`, `MASHED_NO_<name>`
opt-outs). `scripts/lint_rva_bodies.py`: 23 findings, all allowlisted, 0 NEW (unchanged count; the lint never saw these three).

## 1. Static findings (PREREG section 1), the ones that needed a knob

All re-checked against `original/MASHED.exe.unpatched`; association from the FADD order.

- **A6b angle chain** (`0x00468a09..0x00468a22`, `0x00468a89..0x00468aa0`): the CRT acos result (rounded to double on exit) is
  multiplied by a double and rounded to 24 bits once per op at PC_24. A C++ double chain rounds twice. Knob `A6B_ACOS_CRT`: the
  chain in inline x87 under a PC_24 scope. State != 0 normalises through the LUT `0x004c39b0` (`A6B_NORM_LUT`).
- **A5**: face-normal operands `(s0, s6, s3)` (`0x0046e6f3..0x0046e728`; callee p1 = ECX, `0x0046c5f3..0x0046c627`): existing
  `A5_RING_ORDER`. Steer projection `(r.y*dv.y + r.x*dv.x) + r.z*dv.z` (`0x0046e419..`, `0x0046e4d1..`) and drafting dot
  `(dN.y*o.y + dN.x*o.x) + dN.z*o.z` (`0x0046dff6..0x0046e013`): `A5_DOT_ORDER`. Not-all-grounded suspension force in float
  steps (`0x0046e927..0x0046e94c`): `A5_AIR_F32`. Jitter scale reads `+0x2c` as an integer (`0x0046e82f` FILD): `A5_SPIN_FILD`
  (the port read float bits; `+0x2c = 50` gave 0 where the original stores 0.125, Warzone). Contact test is an integer compare
  (`0x0046e761`): `A5_CONTACT_INT`, **no sampled call told the two forms apart**.
- **A4** slide dot `(9d8*9b4 + 9d4*9b0) + 9dc*9b8` (`0x004706db..0x00470701`): `A4_DOT_ORDER`.

Five of these seven are 3-term sums written in Ghidra's print order (the dcphys A6a finding, again), two are integer-vs-float
reads, one is double rounding.

## 2. In-process A/B on the original (dev .asi, hook installed live, canonical recipe unless noted)

Each sampled call: snapshot (record; A5 / A4: + scratch `0x00881560..0x0088158f` + the PRNG read cursor, put back before every
run; A6b: + the 16 matrix floats), the ORIGINAL, the ORIGINAL again (`noo`), the old .asi copy (`nov`, until deleted), the shared
body (`ndiff`, whole record). `MASHED_PHYS_SELFTEST_CAP=20`. A4's test is now the whole function (the original runs its own
dispatch); the old test stopped at `0x00470914` and compared 13 dwords. Logs `D:\MashedWT\dcphys2_data\a6b_ab`.

**Matrix pad words.** `FUN_004c4d20` fills the three pad words of the rotated matrix (m[7] / m[11] / m[15]) from its own
stack (values seen: `0x001af78c`, a stack address; `0x004a7070`, a return address in the CRT acos exit path), so they follow
the caller's stack depth. Their one reader in the chain, `0x004dc410`, replaces lane 3 of every row (`SHUFPS 0xa4` at
`0x004dc43a` / `0x004dc441` / `0x004dc448`) and stores three lanes. Counted as `npad`, not `ndiff`; `noo` still counts them.

| run | body | calls | `noo` | `ndiff` | old copy (`nov`) |
|---|---|---|---|---|---|
| A6b track 0 | knobs ON | 160 grounded | 0 | 0 | 0 |
| A6b track 0 `--boost 2000` | knobs ON | 160 + 158 airborne | 0 | 0 (158 pad-only) | 0 |
| A6b track 0 boost | `NO_A6B_*` (exe default form) | 160 + 147 airborne | 0 | **142** | 0 |
| A6b Warzone x5 (60 / 120 s) | knobs ON | 160 each, 0 airborne | 0 | 0 | 0 |
| A5 track 0 (build a5_v1: RING_ORDER + DRAFT_LUT) | | 960 | 0 | 0 | 385 |
| A5 track 0 boost (a5_v1) | | 1062 (102 airborne) | 0 | 3 (steer projection, 1..2 ulp) | 384 |
| A5 track 0 boost (a5_v2: + DOT_ORDER) | | 1072 (112) | 0 | 0 | 326 |
| A5 Warzone (a5_v2) | | 967 (7) | 0 | 7 (1 ulp, partial-ground suspension) | 25 |
| A5 Warzone x2 (a5_v3: + AIR_F32) | | 982 (22) / 978 (18) | 0 | 19 / 10 (`pf[0..2]` 0 vs 0.125) | 37 / 37 |
| A5 Warzone x2, track 0 boost (a5_v4: + CONTACT_INT) | | 979 (19) / 974 (14) / 1106 (146) | 0 | 11 / 1 / 0 (same `pf[0..2]` words) | 38 / 23 / 381 |
| A5 Warzone x2 (a5_v5: + SPIN_FILD, final set) | | 967 (7) / 960 (0) | 0 | **0** | 25 / 18 |
| A5 track 0 | `NO_A5_RING_ORDER` | 960 | 0 | **960** | 385 |
| A4 track 0 | knobs ON | 1832 | 0 | 0 | 213 |
| A4 Warzone | knobs ON | 1788 | 0 | 0 | 208 |
| A4 boost | knobs ON | 1757 | 0 | 0 (39 pad-only) | 156 |
| A4 track 0 | `NO_A4_DOT_ORDER` | 1828 | 0 | **204** | 204 |

**Final .asi (old copies deleted, asi `088160e4`):**

| run | calls | `noo` | `ndiff` |
|---|---|---|---|
| A6b track 0 boost | 160 + 242 airborne | 0 | 0 (pad-only on the airborne) |
| A5 track 0 / boost / Warzone | 960 / 1280 (320 airborne) / 974 (14 natural partial-ground) | 0 | 0 |
| A4 track 0 / Warzone / boost | 1830 / 1783 / 1790 (25 airborne) | 0 | 0 |

**Coverage gaps:** A6b's state != 0 leg (velocity align) and natural air were not sampled (5 Warzone runs, 0 airborne calls);
the airborne A6b samples are all `--boost` (contrived, state 0). A5's random-surface branch (`rng=1`) was not sampled in any of
14 runs. `A5_CONTACT_INT` changed nothing on any sampled call.

**Harness defects found and fixed in this lane** (dev .asi only):
1. Past their quota, `A6a_SelfTest` and `A5_SelfTest` returned **without running the function at all**, so in self-test runs
   every later sample started from a record that had skipped A6a / A5 on the frames in between. Each admitted call was still
   compared from its own snapshot, so the dcphys A6a verdicts stand, but their late samples came from off-nominal state. Both
   now run the shared body past quota.
2. The old A4 test ran the body math a second time after the test, so `+0xb24` / `+0xb28` advanced twice on every sampled call.
3. The first whole-function A4 test gave `noo` != 0 on 1636 / 1792 calls. Cause: `RunOrigFull` was inlined, and MSVC kept `dt`
   in an XMM register across its `__asm` `CALL` into the original chain, whose SSE leaf `0x004dc410` overwrites xmm0..xmm5; the
   second original run read `dt = 0` (hold counter 50 vs 0 on identical input bytes and x87 state). `__declspec(noinline)` on
   it (and on A6b's `A6bAngleCrt`, same pattern) closed it: `noo` 0 on every call since.

`re/frida/phys_c4_bodydiff.py` (June) called the old copy's `A4_BodyMath` export, which no longer exists; it is superseded by
the in-process whole-function test.

## 3. Gates on the exe (recipe F), frozen final exe `38623436`

| gate | scorer | reference | default (knobs OFF) | `P2` stack ON |
|---|---|---|---|---|
| G-CTRL | sha8 step / gates / ptrace (`H_*`, x2) | `23ac657a / 7bf2f27f / 3186dd09` | **equal, x2** | `c3bfcef3 / 4ed4849b / 4c3548a1` x2 (step 20168 rows) |
| (b) step hashes | `B_*_K0 / K1 / K26` | `0e3790c4 / 55b84dd8 / 2324715b` | **equal** | `4985eb3b / b6e3d073 / 6717c15e` |
| (b) envelopes | `vsum.py` | 9/9 | **9/9** | **9/9** |
| (e) | `ai_speed_env.py --check` K0 | PASS 3/3, launch 1425.7 / 2052.5 / 2055.0, ft_median_m0 2551.6 / 2052.5 / 2278.1 | **PASS 3/3, same values** | **PASS 3/3, same values** |
| D2 | `score_d2_v2.py port`, 3 runs alone | PASS 3/3 | **PASS 3/3**: slip 0.1900 / 0.2550 x3, drvmed 1963.95 x3, H1 398/400 | **PASS 3/3**: slip 0.1900 / 0.2550 x3, drvmed 1965.35 / 1964.25 / 1965.35, H1 398/400 |
| 13 tracks | `run13.py` + `cites_rres.json` | 875 rows, 0 RED | **score table identical to dcphys's** (39 frames, 875 rows, 0 RED), gates PASS | 39 frames, 878 rows, 0 RED, every row cited, gates PASS; **3 of 39 frame rows moved**, all 13_Training (884 / 924 / 959: matched 215 / 211 / 209 -> 213 / 209 / 207, rows 20 -> 21 each) |
| lint | `lint_rva_bodies.py` | 23 / 22 entries | 23, 0 NEW | - |

`P2` = `A6B_ACOS_CRT A6B_NORM_LUT A5_RING_ORDER A5_DOT_ORDER A5_AIR_F32 A5_CONTACT_INT A5_SPIN_FILD A4_DOT_ORDER`.
Every knob-OFF value equals the reference, as PREREG section 2 predicted, including the 13-track replay (no layout move, so no
no-op control was needed). With `P2` ON every prediction of PREREG section 4 held; G-CTRL and B move and repeat.

## 4. Trackers (re-classify)

`hooks.csv`: `file = exe_file` for all three (one body). Levels at the default (the body that ships, knobs OFF), following the
dcphys precedent for A6a: the default body is not clean against the original, so **A6b C4 -> C2, A4 C3 -> C2, A5 stays C2**. The
former C4 / C3 described the deleted .asi copies. The knob-ON body has C4-grade evidence recorded here (hook live, canonical
scenario, clean full-state A/B, no-op control 0, no stub in the body); with a flip the three rows go C4 on it.
New rows: U-9785 (the exe's random draw for A5's random-surface branch is a stand-in), U-9786 (coverage: A6b state != 0 and
natural air, A5 random surface, `A5_CONTACT_INT` unobserved).

## 5. Decision needed (Mariano)

1. **Flip the `P2` stack default ON** (`Core/FaithDefaults.cpp`)? For: the knob-ON bodies are bit-identical to the original on
   every sampled call (A6b 402, A5 3214, A4 5403 on the final .asi) where the default forms miss 142 / 960 / 204 per race; every
   gate passes with it ON. Cost: new references, G-CTRL `c3bfcef3 / 4ed4849b / 4c3548a1`, B `4985eb3b / b6e3d073 / 6717c15e`,
   and 3 moved 13-track rows (13_Training, 0 RED, all cited). If flipped, the three rows go C4.
2. `A5_CONTACT_INT` has no measured effect. Keep it in the stack (it is what `0x0046e761` does) or drop it?
3. Not merged; the shared `original/mashed_re_dev.asi` is not redeployed.
