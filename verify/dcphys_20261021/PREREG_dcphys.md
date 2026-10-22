# PREREG dcphys: D4 dual-copy burn-down, physics A-chain pairs 0x00467650 (A6a) and 0x0046b540 (A3) (2026-10-21)

Branch `work/dcphys` from `23dbc016c` (race/first-frame-parity, pushed). Worktree `.worktrees/dcphys`, `MASHED_NO_DEPLOY=1`.
Private root `D:\MashedWT\dcphys_root` (fresh robocopy of `original/`: 265 files, 506,063,661 bytes, equal to main). Evidence in
`D:\MashedWT\dcphys_data` (`MOVED_OFF_GIT.txt`). Runner `run_arms.py` = d3close's, paths changed. Committed before any knob arm.

Baseline: this worktree built unchanged = exe `e11f4c2f`, asi `20802cf8` (copies in `D:\MashedWT\dcphys_data\base`).
`H_DEF-base` on it: step / gates / ptrace `f78de789 / 43bbe0fb / 161b1a6a`, the reference trio. (b), (e), D2 and 13-track
references are d3close's (`verify/d3close_20261021/RESULT_d3close.md`), all on the same source.

## 1. What the two bodies differ in (static, cited)

Worker diffs (account2, read-only) re-checked against `original/MASHED.exe.unpatched` (`re/tools/disasm_fn.py`).

### A6a 0x00467650: exe `Vehicle/Integrate2.cpp` vs asi `Vehicle/PhysicsChainHooks.cpp` (naked-x87 verbatim, C4 72/72)

| # | difference | original | verdict |
|---|---|---|---|
| 1 | `+0x478 + gear*4` read clamped `gear < 6` in the exe, unclamped in the asi | `0x004679a0 mov edi,[esi+0x490]`, `0x00467a5a fmul [esi+edi*4+0x478]`, unclamped | never fires: the only writers of `+0x490` are `0x00467982` / `0x0046799a` (bounded by iVar11 <= 4) and `0x0046bc66` (stores 0, `xor edx,edx` at `0x0046babc`); exe made unclamped (dead clamp removed) |
| 2 | spin p[0]: exe guards `den != 0` -> 0 | `0x00467e73..0x00467e83`, `_ftol2` `0x004a2c48`: inf / NaN -> FISTP qword indefinite -> `0x004a2c67 test eax,eax; je` -> EAX 0 | equal (both give 0) |
| 3 | drive-arm normalise: exe exact `Vec3Norm3` unless `MASHED_A6A_DRIVE_NORM_LUT` (default OFF) | `0x00467be2 call 0x004c39b0` (LUT) | existing knob, unchanged here |
| 4 | leaves: exe ports vs asi live `0x004c4d20` / `0x004c3df0` | 22 calls in `0x00467650..0x0046897b`, map 1:1 onto the exe body's | target thunks (P4) |
| 5 | state: `Fi_GameMode` / `Fi_ParticipantCount` / `g_modeCarA/B` / `g_suspScale` stand-ins vs live | `0x0040e350`, `0x0040e340`, `0x0088e668/66c`, `0x0088e5f0` | target thunks (P4) |
| 6 | three 3-term dots, both bodies `(c*z + a*x) + b*y` | drive arm `0x00467be9..0x00467c0c` `(+0x9d8*n1 + +0x9d4*n0) + +0x9dc*n2`; block #5 `0x0046837d..0x00468394` `(d0*f0 + d1*f1) + d2*f2`; clamp #6 `0x00468771..0x00468793` `(+0x9d8*+0x9b4 + +0x9d4*+0x9b0) + +0x9dc*+0x9b8` | **differs; new knob `MASHED_A6A_DOT_ORDER`, default OFF** |

Every other step of block #5 and the tail (`0x0046833a..0x0046897b`: angular integration, blend, linear integration, grip
scaling, both clamp arms, full stop) was read against the exe's default (F32 / LUT / LC0 / GEAR_ORDER / SPIN_ORDER) form and
is in the same order. The original writes nothing outside the record (no absolute store in `0x00467650..0x0046897b`).

### A3 0x0046b540: exe `Vehicle/VehicleInit.cpp` vs asi `Vehicle/PhysicsChainHooks.cpp` (C4 24/24)

| # | difference | original | verdict |
|---|---|---|---|
| 1 | exe A3 calls the hull builder `0x0046b1c0` itself | caller `0x0040ed62 call 0x46b1c0` then `0x0040ed68 call 0x46b540`; A3 never calls it | hull moved to `Vehicle/VehicleContactHull.cpp` (exe only, U-9155 split unchanged) and called by the spawn loop before A3, the original's order |
| 2 | exe checks `slot >= 16` before the handling walk | defaults / selector / walk `0x0046b544..0x0046b5dc`, then `0x0046b5e2 cmp esi,0x10` | original order; slot >= 16 unreachable (caller clamps) |
| 3 | handling globals: exe publishes `0x00613108` only | writes `0x00613108/14/30/3c`; only other readers `0x0046eb97`, `0x0046ec3c` (`0x00613108`) and `0x0046dd80` (`0x0061313c`) | target thunk; exe readers unchanged |
| 4 | selector: exe `MASHED_HANDLING_TYPE` (6) | `0x0046b56c..0x0046b585`: `v = [0x0067f17c]`, `[0x0067e9fc] == 10 ? [0x0067ea94]`, `type = (*(int**)0x005f2770)[v][1]` | standalone state wrapper (no `0x005f2770` array); measured type kept |
| 5 | table: exe 4 rows, tag 18 spring 40000 | initialized `.data`, 13 entries (tags 0..72 step 6) + `-1` at `0x00613244`; tag 18 spring **42000** | exe gets the full image copy; default type 6 unaffected |
| 6 | `_DAT_005cea44`: exe `0.277778f` = `0x3e8e38eb` | `0x3e8e38e4` | exe gets the image bits |
| 7 | `0x00613110..0x0061312c`: exe literals | initialized `.data`, read only at `0x0046b676..0x0046b6ca`, no immediate store anywhere in `.text` | literals equal the image |
| 8 | sum orders / mass reduction / LUT sqrt | exe LUT path follows `0x0046b7f2..`, `0x0046b880..`, `0x0046b93b..`, `0x0046ba49..` (d2settle, 26 outputs bit-exact) | measured by the A/B below |

## 2. Consolidation (knob-free part)

`Vehicle/Integrate2.cpp` and `Vehicle/VehicleInit.cpp` are listed in BOTH rsp files; the .asi's `A6a_Entry` / `A3_Entry`
call `Vehicle_Integrate2` / `VehicleInit`. Target-specific code is only the `A6*` / `A3*` thunks (record base, out-of-record
state, the two A6a leaves with no both-target port, the A3 selector input, the A3 table pointer, the handling-global
sink). The .asi seeds its A6a knob defaults to what its verbatim copy computed (LC0 / GEAR_ORDER / SPIN_ORDER /
DRIVE_NORM_LUT; `FaithDefaults.cpp` is exe only). The verbatim .asi bodies stay only until the A/B below is recorded, then
are deleted with their TEMP allowlist lines; the two CROSS-TARGET entries are removed.

**Predictions, knob-free (exe with every new knob OFF):** G-CTRL `f78de789 / 43bbe0fb / 161b1a6a`; (b) step hashes
`9bd86ffe / 433abcd2 / 1a1ff727`, A 3/3 B 3/3 C 3/3; (e) PASS 3/3 with d3close's values; D2 PASS 3/3; 13 tracks 877 rows
0 RED, score table identical. Any knob-OFF move on TRAINING is a bug in this lane (layout-sensitivity memory: only the
13-track replay may need a no-op control).

## 3. In-process A/B on the original (dev .asi, private root)

`MASHED_ROOT=D:\MashedWT\dcphys_root py -3.12 re/frida/scenario_launch.py --hooks <rva> --cars 4 --hold 40` (track 0, mode 10,
rule 0), `MASHED_A6A_SELFTEST_CAP=20`. Each call compares the full 0xd04 record (A3: + the 4 globals + return) for: the original
run again from the same snapshot (`noo`, the no-op control), the verbatim body (`nov`), the shared body (`nvs` = verbatim vs
shared) and the live body (`ndiff`).

Already measured before this PREREG (they motivated it, not tested by it): A6a, knob OFF, 1440 + 1376 calls: `nvs = 0` on
every call, `noo = 0` on every call, `ndiff` = `nov` != 0 on 174 / 25 calls, `+0x9b0..+0x9c4` only, wheel forces equal.

**Predictions:**
- A6a with `MASHED_A6A_DOT_ORDER=1`: `noo = 0` and `ndiff = 0` on every sampled call (the residual closes); `nov` unchanged.
- A3 (knob-free shared body): `noo = 0`, `ndiff = 0` on every call (4 per race; two races).

## 4. Knob `MASHED_A6A_DOT_ORDER` (default OFF), exe arms

Arms (exe, recipe F): `H_DOT` (G-CTRL), `B_DOT_K0/K1/K26` ((b) + (e)), `D2_DOT_1..3` (alone), 13 tracks with the knob.
**Predictions:** G-CTRL hashes change (the dots run every grounded frame; not predicted to the digit); (b) 9/9; (e) PASS 3/3;
D2 PASS 3/3; 13 tracks 0 RED (pose replay, predicted identical).

**Decision rule.** If the A/B closes (prediction above) and (b), (e), D2, 13 tracks all pass with the knob ON, the
recommendation to Mariano is a default flip (`FaithDefaults.cpp`); the flip is his decision and is not made in this lane.
If the A/B does not close, the knob stays OFF and the residual is filed with the A/B evidence. If a gate fails with the
knob ON, the knob stays OFF and the failing gate is reported.

## 5. Amendment 1 (2026-10-22): default flip of `MASHED_A6A_DOT_ORDER` (Mariano's decision, on this RESULT)

Committed before the flip is measured. Changes: `Core/FaithDefaults.cpp` seeds `A6A_DOT_ORDER=1` (opt-out
`MASHED_NO_A6A_DOT_ORDER=1`, master opt-out `MASHED_NO_FAITH=1`); the .asi's `A6aAsiDefaults` seeds it too and now honours
`MASHED_NO_<name>` for every knob it seeds. Exe arms on the new default build, recipe F, private root.

**Predictions (the knob-ON values measured in section 2 of the RESULT become the default):**

| arm | prediction |
|---|---|
| `H_DEF` x2 | `23ac657a / 7bf2f27f / 3186dd09` (= `H_DOT`) |
| `B_DEF_K0 / K1 / K26` | `0e3790c4 / 55b84dd8 / 2324715b`; (b) 9/9; (e) PASS 3/3, same values |
| `D2_DEF_1..3` alone | PASS 3/3, slip 0.1900 / 0.2550 |
| 13 tracks | the `t13_dot` table (3 frame rows moved vs soflip, 0 RED, all cited). If it differs from `t13_dot`, the flip build's layout is the suspect (`sim-trajectory-is-code-layout-sensitive`): the knob-ON run on the pre-flip exe is the control |
| `H_NODOT` (`MASHED_NO_A6A_DOT_ORDER=1`) | `f78de789 / 43bbe0fb / 161b1a6a`, the pre-flip default |
| `H_NF` (`MASHED_NO_FAITH=1`) | `997f4117 / e6288905 / a63bdcfa`, unchanged (soflip / fflip): the knob is a FAITH knob |
| .asi default A/B (no env), track 0, x20 | A6a 1440/1440 bit-identical, `noo` 0 |
| .asi `MASHED_NO_A6A_DOT_ORDER=1`, same | the residual returns (174/1440 on the same race as run1) |

Then `0x00467650` C2 -> C4 through re-classify on the default body's evidence. If any prediction above fails, stop and report;
no tracker change.
