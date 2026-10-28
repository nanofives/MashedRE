# RESULT D5-PARTMORPH: car part 0x12 morph and part 0x14 wobble (D-11077, L-45)

Verdict: **GREEN** on the PREREG gates (G1..G5), with one named residual: the law is ported but
dormant in default play, because the standalone has no producer of record state 0 (section 4).

Branch `orch/D5-PARTMORPH` from `orch/integration` d92bb3a5e. PREREG `9b54680f5` (committed alone,
before any arm). Code `3e35b8a7f`. Exe `mashedmod/build/mashed_re.exe` SHA-256
`4329f07cbea994b2c5b8f76d9a7dc9f453e2920d6e90f9fa0f48a2ec7decabe9` (built from the worktree,
`MASHED_NO_DEPLOY=1`, rva-lint NEW=0). No original-side capture was taken (the original does not boot:
no audio endpoint). Original-side evidence is the static decode plus the captures already on disk.

## 1. What was decoded (static, Ghidra pool clone through `re/tools/decomp_pc.py` + capstone)

The full law is in `PREREG_D5-PARTMORPH.md` section 1, with an address for every constant. The two
blockers D-11077 named:

- `FUN_00422470`'s chain is decoded: floor (`FUN_004a3090`), `__ftol` (`FUN_004a2c48`), the integer
  hash `FUN_004223f0` (`(n<<13)^n`, `*0x3d73`, `+0xc0ae5`, `*e`, `-0x2df722f3`, `&0x7fffffff`,
  `*2^-31`, mask 0xff), and the cosine lerp `FUN_00422440` (`x - floor` as `t`, `hash(i)`,
  `hash(i + 1)`). It is 1D value noise with period 256 on `DAT_0063e4a4`.
- The morph: `FUN_00420420` sets part 0x12's geometry to one interpolator (start target 0, end target 1,
  time 1.0). `FUN_00544bf0` writes the atomic interpolator (position = the morph value), and the RW
  D3D9 instance path (`0x004eaf13`) lerps `(end - start) * position * recipTime + start` through
  `FUN_004f6870` case 2, positions and normals. All 78 livery DFFs carry part 0x12 with two morph
  targets (504 / 524 vertices) and part 0x14 with one. ADVANTAGE0: part 0x12 = frame 23, part 0x14 =
  frame 82, matching the D-11077 witness frames.
- The `moved` input is `clamp(((dy*dy + dx*dx) + dz*dz) * 25, 0, 1)` between this call's car matrix and
  the previous call's (`0x0063d8e0 + v*0x40`, copied at the end of every `FUN_00420870`).

## 2. What was ported (one knob, default OFF)

- `MASHED_PARTMORPH=1` runs `FUN_0041f770`'s gated body per car per frame in the draw loop, in
  `FUN_0040da50`'s slot order (`TrackRenderer.cpp`: `PartMorphStep`, `PartMorphNoise`, `PartMorphHash`).
  It sets the part 0x12 morph position for that car's copy and adds part 0x14's frame pose (Y by 30a
  REPLACE, then X by 15a PRECONCAT, position 0). `DAT_0063e4a4` and the previous-position table are
  file statics, zero at boot, never reset, like the .data.
- `DffModel` keeps morph target 1 (baked and de-indexed like target 0). `BuildClump` returns it per
  atomic in the same space as target 0. `RaceSubmit` lerps the part 0x12 atomics for each copy that
  draws and re-instances them (librw `Geometry::lock`), only when the position changes.
- Diagnostics, all default unset: `MASHED_DBG_PARTSTATE0=<slot>` (forces record state 0),
  `MASHED_DBG_PARTMORPH=<csv>` (law inputs and outputs, plus `<csv>.vtx` with vertex 0 of each lerp),
  `MASHED_PARTMORPH_SELFTEST=<out>` (the noise on a 6041-point grid).

## 3. Gates (all arms on exe 4329f07c, `verify/partmorph_20261028/run_arms.py`, summary `GATES.txt`)

| Gate | Arm(s) | Result |
|---|---|---|
| G1 default unchanged | H_DEF, B_DEF_K0, B_DEF_K1, B_DEF_K26 | G-CTRL 5943c5f1 / 364350ae / 52052890, B 3a03670c / 9fcdbd3a / 53778664 = `merge_train.json` refs. PASS |
| G2 knob does not move the sim | B_PMO_K0, B_PMOF_K0 (+ B_PMOF1_K0, B_SHON_K0, B_SHOFF_K0) | step hash 3a03670c on all five = B_DEF_K0. PASS |
| G3 noise law | B_PMOF_K0 self-test vs `noise_ref.py` | 6041 points, max 0 ulp. PASS |
| G4 law replay | B_PMOF_K0 (slot 0 forced) | 4072 rows, 1018 state-0 rows, 0 mismatches on prev, moved, morph, a, phase step / hold / chain; 2036 lerps exact; vertex 0 (v0, v1) = a DFF part 0x12 target pair to 3.3e-10. PASS |
| G5 dormancy | B_PMO_K0 (knob only) | 4072 rows, 0 with state 0. PASS |
| W1 captures | `verify/librwrest_checks_20261016/orig_*` (8 arms) | 5008 car-rows, `rec+0x298` is 1 or 2 on every row (0 never), part 0x14 modelling identity on every row (consistent with the gate). The captures cannot witness the wobble or morph values. `W1_captures.txt` |

Deviations from the PREREG, stated plainly:

- The stacks are named PMO / PMOF in the runner (`STACK['PM']` already existed as `MASHED_POS_MODEL`).
- G4 checks the logged `(v0, v1)` against the DFF to 1e-5, not 1 ulp: the logged vertices are the
  un-baked copy (RwSceneBuild's frame-chain inverse), so the round trip is not bit-exact. Measured 3.3e-10.
- Added after G4 (not pre-registered): B_PMOF1_K0, slot 1 forced. On slot 0 (the player) the steer mean
  stays 0 for the whole recipe, so the morph sat at 0.5. Slot 1 gives 117 distinct morph positions
  (0.341..0.504) and `a` in -0.293..0.141, G4 PASS. The ±1 clamp is not reached at run time
  (steer extreme -6.35, times 0.05 = -0.32), so that branch is only exercised by the static decode.
- The illustration pair (B_SHON_K0 / B_SHOFF_K0, `MASHED_DBG_BBDUMP=900`) wrote no backbuffer in this
  recipe, so there is no frame image. It was illustration only, not acceptance.
- Precision: the noise takes `cos` in double and rounds once in `1 - cos`, where the original keeps
  `fcos` at x87 extended. That can differ by 1 ulp in rare cases. G3 compares against a model with the
  same cos, so it does not measure this.

## 4. Residual: no record-state-0 producer in the standalone

`rec+0x298 == 0` is written only by `FUN_00448220` (post-race result camera: a loop of
`FUN_0041ef60(i, 0)` over the four cars) and by `FUN_00446520` for car 0 when `FUN_0042f6a0() == 2` or `DAT_007f0fd0` is 5, 10 or 9. The
port's `LibRw::CarLodState` returns only 1 or 2 (distance rule), so with `MASHED_PARTMORPH=1` alone the
law never runs (G5) and parts 0x12 / 0x14 stay hidden, as they are in every captured original row (W1).
In the original the parts and this law only show at the result cameras. Porting that producer belongs to
the result-camera director (`FUN_00446520`, ported behind `MASHED_CHASE_RIG`; `FUN_00448220`). That is
a separate step.

The default flip of `MASHED_PARTMORPH` is a decision row. Flipping it today changes nothing visible
until state 0 has a producer.

## 5. Raw data (not committed), `D:/MashedWT/D5-PARTMORPH_data/arms`

| File | SHA-256 |
|---|---|
| H_DEF.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| H_DEF.gates.csv | 364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 |
| H_DEF.ptrace.log | 520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 |
| B_DEF_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| B_DEF_K1.step.csv | 9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f |
| B_DEF_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |
| B_PMO_K0.step.csv / B_PMOF_K0.step.csv / B_PMOF1_K0.step.csv / B_SHON_K0.step.csv / B_SHOFF_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| B_PMO_K0.pm.csv | dda37cf8c2ac50621f61589514ff0e4529a981206f7afc7a359e062ee34e6b44 |
| B_PMOF_K0.pm.csv | b7bd579ec37591cb397b9b35687e76cc6a8c06d608a380801b3cf9cb10d37aec |
| B_PMOF_K0.pm.csv.vtx | cbf9078c84b72d3b7d45f9bf11fe565cfc4735977c7b18cecc2212bb16576cb1 |
| B_PMOF_K0.selftest.csv | aff95f1fc83ceb4f93b292d5507058a596bd8e46e01b613c1a9bf14d0bdd23b3 |
| B_PMOF1_K0.pm.csv | ef5c346588ab914f7d1d25c9161be9a3b59d21b66a5b3315f23287a8ab759fbd |
| B_PMOF1_K0.pm.csv.vtx | 005b0a9d13005f23d18cf3b61364607c677e1b5b0035a844218148793bd9dd13 |
| RUNLOG.txt | 46606f66d2b90a03e780219a7efb40401a2ba52539272bed8c53e81d79719344 |

## 6. Trackers

None touched. Hooks.csv rows for `0x0041f770`, `0x00422470`, `0x004223f0`, `0x00544bf0` and the
DEFERRED D-11077 row are left for re-classify at the lead's call. The evidence here is standalone law
replay against a static decode, not a Frida diff, so it supports no C-level promotion by itself.
