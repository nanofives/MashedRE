# RESULT PORT-AISPLINE-DDKORDER (2026-11-09)

PREREG: `PREREG_AISPLINE_DDKORDER.md` (committed alone, b111eb1b2). Branch `orch/PORT-AISPLINE-DDKORDER` from
`orch/integration` 5873e911e.

## 1. Answer

**GREEN.** The DDK-order body `AiSplineEval_x87_ddk` reproduces the original's 0x00443300 bit for bit: live-seeded
80/80 in X24, X53 and S (57 of those 80 calls are natural PC deltas, where the original at PC_53 differs from the
original at PC_24), and 20000/20000 synthetic inputs in every variant. The matched body (`_match`, today's default)
stays RED in every mode on the same harness (X24 and S 40/80, X53 55/80), which reproduces the audit's finding (a).

The default gates are unchanged with the knob OFF: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3.
**The knob-ON gates are unchanged too, and that is structural, not a measurement gap:** `mashed_re.exe` calls
`AiSplineEval_x87` only through `AiLeaf_414570` (naked `AL_414570` -> `AL_443440`), and that runs only under
`MASHED_AILEAF` (default OFF, `Ai/AiStandalone.cpp:1056`). The exe's race AI evaluates the spline in `CatmullRom`
(`Ai/AiStandalone.cpp:306`), which already uses this order under `AiLutMask() & 4`, and the mask is fixed at 7
(`Ai/AiStandalone.cpp:157`). So the knob changes values only on the `.asi` hook path (AiSplineHooks.cpp) and on the
`MASHED_AILEAF` leaf. With `MASHED_AILEAF=1`, H, B and W are also identical with and without the knob.

## 2. Change

`mashedmod/src/mashed_re/Match/MatchS08_00443300.cpp`:

- The byte-matched source `re/match/s08/00443300.c` is unchanged and compiles as `AiSplineEval_x87_match`.
- `AiSplineEval_x87_ddk` is the same index wrap and clamp, then the polynomial in the order the original's listing
  shows (0x00443373..0x00443427): `((A*t2 + B*t3) + (p2-p0)*t) + (p1+p1)`, times 0.5. Each float operation is its own
  assignment. In the X image every intermediate goes through `fstp dword` / `fld dword` (checked in the disassembly
  of `port_X.exe`, 0x30001060..0x3000115e), so each one is rounded to float, and the value does not depend on the CW.
- `AiSplineEval_x87`, the symbol the naked AiLeafTargets callers and the `.asi` export use, calls `_ddk` when
  `MASHED_AISPLINE_DDK=1` and `_match` otherwise (flag read once with getenv and cached).

`verify/savelink_20261023/run_arms.py`: stack `DDK` = `MASHED_AISPLINE_DDK=1` (arms `H_DDK`, `B_DDK_K<k>`).

rva-lint: `OK -- no NEW duplicate bodies` (the one finding is the existing allowlisted CROSS-TARGET row).

## 3. Arm 1: emulation

### 3a. Synthetic control (`aispline_ddk/fuzz_00443300.py`, `aispline_ddk/AISPLINE_FUZZ.tsv`)

Unicorn runs the original's bytes of 0x00443300 (MASHED.exe.unpatched) and both port bodies of both images on
20000 random inputs (seed 7): 64 points of mixed magnitude, count 4..64, a random segment, and t in [0,1] for 90%
of inputs (the other 10% outside [0,1], to exercise the clamp). The reference is the original at PC_24 (CW 0x007f,
the only CW at this RVA in the audit).

| variant | differs from original @PC_24 |
|---|---|
| original @PC_53 | 14944 / 20000 |
| X `_ddk` @PC_24 | **0 / 20000** |
| X `_ddk` @PC_53 | **0 / 20000** |
| S `_ddk` (SSE2) | **0 / 20000** (CW 0x007f and 0x027f) |
| X `_match` @PC_24 | 14039 / 20000 |
| X `_match` @PC_53 | 14862 / 20000 |
| S `_match` | 14039 / 20000 |

The first run (2000 inputs, seed 1) gave the same pattern: `_ddk` 0 in every variant, `_match` 1441 to 1529.

### 3b. Live-seeded (`aispline_ddk/x87_live_emu.py`, the audit's harness)

Quick-Battle race recipe on the original (hooks OFF), 0x00443300 attached late for 80 consecutive calls, two port
images of this TU only (`aispline_ddk/x87_port_images.py`, X = `/arch:IA32` at 0x30000000, S = SSE2 at 0x34000000,
0 forwarders, 0 int3: the function is a leaf). All 80 calls at CW 0x007f. Emulated O = live call on 80/80 in both
legs (the harness control).

| leg (port entry) | scored | natural deltas (O53 != O) | O24 | X53 | X24 | S |
|---|---|---|---|---|---|---|
| ON (`AiSplineEval_x87_ddk`) | 80 | 57 | GREEN 80 | **GREEN 80** | **GREEN 80** | **GREEN 80** |
| OFF (`AiSplineEval_x87_match`) | 80 | 57 | GREEN 80 | RED 55/80 | RED 40/80 | RED 40/80 |

Tables: `aispline_ddk/AISPLINE_EMU_ON.tsv`, `aispline_ddk/AISPLINE_EMU_OFF.tsv`. Pass bar (PREREG section 3):
X24 GREEN 80/80 and S GREEN 80/80. Met. X53 also GREEN, as section 2 predicts (one float op per assignment).

## 4. Arm 2: default gates, knob OFF

Worktree build `mashed_re.exe` sha256 `4102d73e...` (`MASHED_NO_DEPLOY=1`, rva-lint OK), merge-train runner
`verify/savelink_20261023/run_arms.py`, private root `D:/MashedWT/PORT-AISPLINE-DDKORDER_root`, under test-slot and
`mashed_lock.py run`.

| arm | hashes | ref (`merge_train.json`) |
|---|---|---|
| H_DEF | `8284be46 / 67931834 / 31fe043e` (13962 / 54085 / 27042 lines) | same |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 (762 lines) | same |

**Unchanged.**

## 5. Arm 3: knob-ON race gates (report only)

| arm | stack | hashes |
|---|---|---|
| H_DDK | `MASHED_AISPLINE_DDK=1` | `8284be46 / 67931834 / 31fe043e` |
| B_DDK_K0 / K1 / K26 | `MASHED_AISPLINE_DDK=1` | `1d5bf4f9` x3 |
| H_AL | `MASHED_AILEAF=1` | `8284be46 / 67931834 / 31fe043e` |
| H_ALDDK | `MASHED_AILEAF=1` + DDK | `8284be46 / 67931834 / 31fe043e` |
| B_AL_K0 / B_ALDDK_K0 | AILEAF / AILEAF + DDK | `1d5bf4f9` / `1d5bf4f9` |
| W_AL / W_ALDDK | AILEAF / AILEAF + DDK (Warzone, 3600 frames) | `eeb92153` / `eeb92153` (step.csv, 4663 lines; pontoon `a329ca22`, seek `fa8618df` both) |

RUNLOG.txt confirms `MASHED_AISPLINE_DDK=1` in the DDK arms. The hashes did not move, contrary to the PREREG's
expectation. The reason is in section 1: the default exe never calls `AiSplineEval_x87` (the audit's exe CW probe on
this recipe, `cw_exe_race.json`, counted 0 calls), and with `MASHED_AILEAF=1` the leaf path is enabled but its outputs
do not change on these recipes. This step did not count `AiSplineEval_x87` calls under `MASHED_AILEAF=1` [UNCERTAIN:
whether `AL_443440` reaches its spline calls on Training and Warzone; a call counter on `AiSplineEval_x87` in an
AILEAF arm resolves it]. The stacks `AL` / `ALDDK` were added to run_arms.py as report-only.

## 6. Decision question for the lead

A flip (`MASHED_AISPLINE_DDK` default ON) is **value-neutral on every default-build recipe** (H, B and W unchanged),
because the exe's race AI uses `CatmullRom`, which already has this order. What a flip would change:

- `.asi`: the installed body of the 0x00443300 hook would equal the original on every input (today it differs on
  about 70%, `AISPLINE_FUZZ.tsv`). This matters for Frida diffs and canonical-scenario runs of the hook.
- `MASHED_AILEAF=1`: the replay leaf's spline would equal the original.

Options: (a) flip it ON as a one-line default change, with the hashes of this RESULT as its gate evidence (no
reference moves), (b) leave it OFF. Recommendation **(a)**. It has no measured cost and removes a known
every-mode mismatch.

Separate, for the lead (not in this step's scope): the exe carries two port bodies of 0x00443300.
`Ai/AiStandalone.cpp:306` `CatmullRom` (the one the race runs) and `Match/MatchS08_00443300.cpp`. The
`CatmullRom` DDK arm writes `A * t2 + B * t3` in one expression, so it matches the original only while its caller
holds PC_24 (that is what its comment states, and this step did not measure it). A later D4 dual-copy step could make
`CatmullRom` call `AiSplineEval_x87_ddk`, which is CW-independent.

## 7. Commits

| commit | content |
|---|---|
| b111eb1b2 | PREREG (alone) |
| 5c914ded5 | knob body, run_arms DDK stack, harness copies (`aispline_ddk/`), fuzz control, AISPLINE_FUZZ.tsv, AISPLINE_EMU_ON.tsv |
| this commit | AISPLINE_EMU_OFF.tsv, run_arms AL / ALDDK report stacks, this RESULT |

## 8. Raw files (`D:/MashedWT/PORT-AISPLINE-DDKORDER_data`, not committed)

| file | sha256 |
|---|---|
| emu_on_1.json | 105f220e93fb8828f9769e93e9623df7535cca9a6d142db95ec1e88a5146040b |
| emu_on_1.log | 2668f51d8292a5e568100b283044e39cd4f536d8639003266d6df99d2e30093a |
| emu_off_1.json | fe98a8f870ab2afd432162cd22c8a553c967b78eaaeb429ce7e7116b660e9ca8 |
| emu_off_1.log | c901f8ae19e02925f8b6d1530d85c51e81d4f12d28d83ae8182c9f0d3763821a |
| arms.log | f93893296bcfd0df777e29d71861b55dbf4c9ff6dc474d83e8ab8be90381673c |
| arms_al.log | bd8c795db463a6d1084ec07cdcaf7702a5b3913ea8951e9f6728ee83b09643a2 |
| build.log | bbe4feba04b413e07de4097c0a6a7b7f58fd3e6472f3109a89247abb78b2609b |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/H_DDK.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/B_DDK_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/W_AL.step.csv | eeb921531da3e3086f8c63eb57c2821c163cf4710a67e83d953813ccf84eb955 |
| arms/W_ALDDK.step.csv | eeb921531da3e3086f8c63eb57c2821c163cf4710a67e83d953813ccf84eb955 |
| img/X/port_X.exe | 0ae07165adda70ffbd4efd6e8926ebcaa066a99602810c94b5f8bd8f4ff32923 |
| img/S/port_S.exe | 3df8de336a213c061c9dff96526477ba85a6a20b126d0de14d2351a4d3f82883 |
| worktree mashedmod/build/mashed_re.exe | 4102d73ef0436a1d47771a1d40a2702e9b4b12d544fdd21ea8693e3d96cca785 |
