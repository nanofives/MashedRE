# PREREG dcphys2: D4 dual-copy burn-down, physics A-chain part 2: A6b 0x00468980, A5 0x0046ddb0, A4 0x00470670 (2026-10-22)

Branch `work/dcphys2` from `72ec8a7d0` (race/first-frame-parity, pushed). Worktree `.worktrees/dcphys2`, `MASHED_NO_DEPLOY=1`.
Private root `D:\MashedWT\dcphys2_root` (robocopy of `original/`, 265 files, `original\` inside). Evidence in
`D:\MashedWT\dcphys2_data` (`MOVED_OFF_GIT.txt`). Runner `run_arms.py` = dcphys's, paths changed, plus stack `P2` (section 4).
Committed before any exe knob arm.

Baseline: this worktree built unchanged = exe `4db582db`, asi `163cfee5` (`D:\MashedWT\dcphys2_data\base`). `H_DEF-base` on it:
step / gates / ptrace `23ac657a / 7bf2f27f / 3186dd09`, the reference trio. (b), (e), D2 and 13-track references are the
committed dcphys score files (`verify/dcphys_20261021/`), same source.

## 1. What the two bodies differ in (static, cited)

Worker diffs (account2, read-only), every claimed difference re-checked against `original/MASHED.exe.unpatched`
(`re/tools/disasm_fn.py`). Sum association taken from the FADD order, never from the decomp text.

### A6b 0x00468980: exe `Vehicle/AeroStabilize.cpp` vs asi naked `A6b_BodyAsm`

| # | difference | original | verdict |
|---|---|---|---|
| 1 | angle chain: exe `std::acos`, the chain in double, rounded to float once | `0x00468a09` CALL `0x004a3384` (CRT acos, result rounded to double on exit, `Ai/AiLeafTargets.cpp` AL_Acos header), then FMUL qword `0x005ccae0`, FSUB qword `0x005ccad8` / dword `0x005ccad0`, FCHS (pitch), FMUL dword dts, FSTP dword, each op rounding at the race loop's PC_24 (`0x00468a0e..0x00468a22`, `0x00468a8e..0x00468aa0`) | **differs; knob `MASHED_A6B_ACOS_CRT`** (inline x87 under a PC_24 scope; the acos is AL_Acos in the exe, the live `0x004a3384` in the .asi) |
| 2 | at.y / right.y read directly | dots with Y = `0x006146fc` (0,1,0), `(m9*1 + m8*0) + m10*0` (`0x004689b8..0x004689ca`), `(m1*1 + 0*m0) + m2*0` (`0x00468a36..0x00468a4a`) | equal for finite inputs; the knob path writes the dots as the original adds them |
| 3 | state != 0 normalise: exe exact `Vec3Norm3` | `0x00468af1` CALL `0x004c39b0` (LUT) | **differs; knob `MASHED_A6B_NORM_LUT`** (`Math::LutNormalize`, as A6a's `DRIVE_NORM_LUT`) |
| 4 | leaves: exe ports vs asi live `0x004c4d20` / `0x004a3384` / `0x004c39b0` | 3 call sites | target thunks |
| 5 | constants (`0x005cc558`, `0x005cc33c`, `0x005cc320`, `0x005ccae0`, `0x005ccad8`, `0x005ccad0`, `0x005cc9a0`, `0x006146f0`, `0x00614708`) | image bits read 2026-10-22 | equal |

### A5 0x0046ddb0: exe `Vehicle/ForceIntegrator.cpp` vs asi C++ `A5_Body`

| # | difference | original | verdict |
|---|---|---|---|
| 1 | face-normal operands: exe default `(s0, s3, s6)` | `0x0046e6f3` ECX = `0x00881578` (s6), `0x0046e6fe` EAX = `0x00881560` (s0), `0x0046e71d` EDX = `0x0088156c` (s3); callee `0x0046c5f0` takes p1 = ECX (`v = apex - ECX`, `n0 = v.z*u.y - v.y*u.z`, `0x0046c5f3..0x0046c627`) | differs; existing knob `MASHED_A5_RING_ORDER` (d2off: makes the ring equal; the ring has no reader on D2) |
| 2 | steer projection `(r.z*dv.z + r.x*dv.x) + r.y*dv.y` | `(r.y*dv.y + r.x*dv.x) + r.z*dv.z`, wheel 0 `0x0046e419..0x0046e43b`, wheel 1 `0x0046e4d1..0x0046e4f1`; dv = `[esp+0x18/0x1c/0x20]` (`0x0046e39d` / `0x0046e3b3` / `0x0046e3f2`) | **differs; knob `MASHED_A5_DOT_ORDER`** |
| 3 | drafting dot `(dN.z*o.z + dN.x*o.x) + dN.y*o.y` | `(dN.y*o.y + dN.x*o.x) + dN.z*o.z` (`0x0046dff6..0x0046e013`) | **differs; same knob** |
| 4 | not-all-grounded suspension force in double | `0x0046e927` FLD load, FDIV dt, FMUL `0x0088e5f0`, three FMUL / FSTP (`..0x0046e94c`), all at PC_24; 65536 compare on that value (`0x0046e957`) | **differs; knob `MASHED_A5_AIR_F32`** |
| 5 | jitter scale reads `+0x2c` as float bits | `0x0046e824` MOV, `0x0046e82f` FILD (integer), FMUL `[esi]`, FMUL `0x005cc328` | **differs; knob `MASHED_A5_SPIN_FILD`** |
| 6 | per-wheel contact test `pf[-0x17] == 0.0f` | `0x0046e761` CMP dword, EBP = 0 (integer); differs for `0x80000000` only | knob `MASHED_A5_CONTACT_INT` (no sampled call has told the two apart) |
| 7 | `std::memset` of the axis-angle matrix | `m` is read only by the vectors leaf `0x004dc410`, which replaces lane 3 (`0x004dc43a..0x004dc448`) | no output effect |
| 8 | Phase 1 drive product, Phase 4, Phase 6 all-grounded (u9760 `SUSP_LUT`, default ON), drafting LUT (`A5_DRAFT_LUT`, FAITH ON) | `0x0046def4..0x0046df31`; u9760 / u9771 | equal |
| 9 | out-of-record state and leaves (record base, scratch `0x00881560`, gravity `0x00803334..40`, `0x0088e5f0`, `0x0088e610`, race timer `0x007f0ff8`, `0x004c3df0`, `0x004c4d20`, `0x00472650`, `0x00442ce0`, `0x00442c80`, `0x0046c5f0`) | | target thunks; the exe's three leaf ports move verbatim to the exe-only `Vehicle/ForceIntegratorLeaves.cpp` |

### A4 0x00470670: exe `Vehicle/VehicleControl.cpp` vs asi `A4_BodyMath` + `A4_Body`

| # | difference | original | verdict |
|---|---|---|---|
| 1 | slide dot `(9dc*9b8 + 9d4*9b0) + 9d8*9b4` | `(9d8*9b4 + 9d4*9b0) + 9dc*9b8` (`0x004706db..0x00470701`) | **differs; knob `MASHED_A4_DOT_ORDER`** |
| 2 | hold counters `(int)((float)c + dt)` | FILD, FADD dword dt, `_ftol2` `0x004a2c48` (`0x00470737..0x00470746`) | equal (float add = PC_24 add, truncation both) |
| 3 | force chain, clamps, rings, parked damp | `0x00470788..0x00470848`, `0x00470948..0x00470986` | equal |
| 4 | dispatch: matrix for A5 / A6b | original passes `record + [+0x9a8]*0x40 + 0x928` (`0x00470699`, `0x00470918`, `0x0047093b`) | exe keeps its body basis (U-9152, an input); .asi thunks pass the original's pointer |
| 5 | callees, game mode, ring phase | `0x0046ddb0` / `0x00467650` / `0x00468980` with the register ABI of `0x00470914..0x00470943`; `0x0040e350`; `0x007f101c` | target thunks |

## 2. Consolidation (knob-free part)

`AeroStabilize.cpp`, `ForceIntegrator.cpp`, `VehicleControl.cpp` are listed in BOTH rsp files; the .asi entries call
`Vehicle_AeroStabilize` / `VehicleWheelForceIntegrate` / `VehicleControlIntegrate`. Target-specific code is only the thunks of
section 1. The .asi seeds every new knob ON (plus `A5_DRAFT_LUT`, `A5_RING_ORDER`), with `MASHED_NO_<name>` opt-outs, as
`A6aAsiDefaults` does. The old .asi bodies are deleted once section 3 is recorded.

**Predictions, knob-free (exe with every new knob OFF = the default):** G-CTRL `23ac657a / 7bf2f27f / 3186dd09` x2; (b) step
hashes `0e3790c4 / 55b84dd8 / 2324715b`, A 3/3 B 3/3 C 3/3; (e) PASS 3/3 with the reference values; D2 PASS 3/3; 13 tracks 875
rows 0 RED, score table identical to `verify/dcphys_20261021/tracks_def_score.txt`. Any knob-OFF move on TRAINING is a bug in
this lane; a 13-track move with the knobs OFF needs a no-op layout control before it is read.

## 3. In-process A/B on the original (dev .asi, hook live, private root)

`MASHED_ROOT=D:\MashedWT\dcphys2_root py -3.12 re/frida/scenario_launch.py --hooks <rva> --cars 4 --hold 40|60` (track 0 mode 10
rule 0; Warzone `--track 11`; `--boost 2000`), `MASHED_PHYS_SELFTEST_CAP=20`. Each sampled call: snapshot, the original, the
original again (`noo`), the old .asi copy (`nov`, TEMP), the shared body (`ndiff` vs the original). Whole 0xd04 record (A5 / A4:
+ the 12 scratch dwords; A6b: + the 16 matrix floats). The A4 test is now the whole function (the original runs its own dispatch).

Already measured (they motivated the knobs, not tested by this PREREG), logs `D:\MashedWT\dcphys2_data\a6b_ab`:
- A6b, knobs ON: 160 grounded + 158 boosted airborne calls, `noo` 0, `ndiff` 0 apart from the matrix pad words m[7] / m[11] / m[15]
  (stack contents FUN_004c4d20 copies; their reader `0x004dc410` drops lane 3), now counted as `npad`. Knobs OFF: 142 of 147
  airborne calls differ (rotation floats, 1..40 ulp). State != 0 and natural air: not sampled (5 Warzone runs, 0 airborne).
- A5, knobs ON (final set): track 0 960 / boosted 1106 (146 airborne) / Warzone 967 + 960, `noo` 0, `ndiff` 0. The old copy
  misses 18..385 per race. `NO_A5_RING_ORDER`: 960 of 960 differ. Random surface (`rng=1`): 0 samples in 12 runs.
- A4 (whole function), knobs ON: track 0 1832 / Warzone 1788 / boosted 1757, `noo` 0, `ndiff` 0 (boosted: 39 pad-only).
  `NO_A4_DOT_ORDER`: 204 of 1828 differ.

**Predictions (final .asi, old copies deleted):** the same `noo` 0 / `ndiff` 0 on the same recipes.

## 4. Knob stack `P2` (default OFF), exe arms

`P2` = `MASHED_A6B_ACOS_CRT MASHED_A6B_NORM_LUT MASHED_A5_RING_ORDER MASHED_A5_DOT_ORDER MASHED_A5_AIR_F32 MASHED_A5_CONTACT_INT
MASHED_A5_SPIN_FILD MASHED_A4_DOT_ORDER` (all =1). Arms (exe, recipe F): `H_P2` x2 (G-CTRL), `B_P2_K0/K1/K26` ((b) + (e)),
`D2_P2_1..3` (alone), 13 tracks with the stack.

**Predictions:** G-CTRL hashes change (the A4 and A5 dots run every grounded frame; not predicted to the digit) and repeat x2;
(b) 9/9; (e) PASS 3/3; D2 PASS 3/3; 13 tracks 0 RED, every row cited (rows may move: dcphys's one physics knob moved 3 of 39).

**Decision rule.** If section 3 holds on the final .asi and (b), (e), D2, 13 tracks all pass with `P2` ON, the recommendation to
Mariano is a default flip of the stack (`Core/FaithDefaults.cpp`); the flip is his decision and is not made in this lane. If a
gate fails with `P2` ON, the stack stays OFF and the failing gate is reported (then per-knob arms to isolate it).
