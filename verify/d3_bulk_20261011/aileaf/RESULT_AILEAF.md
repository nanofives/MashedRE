# D3 bulk Phase 1, G-AILEAF: RESULT (2026-10-11)

Verdict: the three leaves are ported verbatim behind `MASHED_AILEAF` (default OFF, also needs
`MASHED_PATHPROG`). The replay against the original's bytes is 6,000/6,000 exact at PC_24 + PC_53 and
6,000/6,000 at PC_64. The negative control catches 3/3 corrupted rows. G-CTRL with the knob OFF is unchanged:
`ae67b13a / c8317ce2 / 31db9384`. The knob has NOT been switched on (Phase 2, `PREREG_AILEAF.md`).
G-EVQ was dropped before code (PLAN.md §0).

## 1. What was ported

**Spec and generated bodies**
- Spec: `re/tools/naked_specs/aileaf_g1.json`.
- Generated: `mashedmod/src/mashed_re/Ai/AiLeafTargets_naked.inc`.
- Host TU: `Ai/AiLeafTargets.cpp` (exe-only, `exe_sources.rsp`).

**Verbatim bodies (12)**
- The three wired leaves: 0x00414570 (821 B), 0x00415880 (1,137 B), 0x0046d570 (228 B).
- Private copies: 0x00443440 (2,254 B; the inventory says 2,225, but the body ends at 0x00443d0e),
  0x00443300, 0x004233e0, 0x00408af0, 0x00408a50, 0x00426bb0, 0x0040e470, 0x004c39b0, 0x004c3ac0.

**Hosts, per the TU header**
- Rebound record readers. `AL_FloatAsInt` replaces `VehicleFloatFieldAsInt`, which reads the blank
  0x00881f80 pad in the exe.
- CRT acos transcribed as `AL_Acos`:
  - CW = (caller & 0x300) | 0x7f;
  - fpatan path; ±1 → fldz / fldpi;
  - exit through 0x004a713b / 0x004a7047, including the qword round trip when PE is set and the CW is
    not 0x27f.
  - `[0x00773990] == 0` in the original's race: 65/65 samples, `../peek2_orig.json`.
- `AL_Ftol`.

**Patches**
- `0x004158c4`: the indexed `.data` table 0x005f2fc0 is read from an exe-safe copy.
- `0x0046d588`: record base + `g_al_recdelta`.
- The RW LUT base.

**Wiring:** `Ai/AiStandalone.cpp`, before the TGT3 arm.
- 0x00414570 accept + LOS → `mode = 1`, `look = cand` (0x00416319..0x00416329).
- 0x00415880 accept + LOS is COUNTED and not committed. The exe has no FUN_00415d00 body
  (`AiTargeting.cpp` is .asi-only).
- The TGT3 arm gained `mode == 0`, because the original's mode-1/2 accepts jump to 0x0041645e, past
  0x00414a70.

**Not wired:** 0x0046d570. The exe's 0x00417640 port documents its result as a dead operand. Its other
live caller, 0x0040ca00, has no exe body.

## 2. Gates

| gate | result | file |
|---|---|---|
| G-AILEAF-REPLAY (PC_24 + PC_53 rows) | 6000/6000 exact. ret1: 570 121, 880 103, d570 568 | `replay_synth.txt` |
| G-AILEAF-REPLAY (PC_64 rows) | 6000/6000 exact | `replay_synth_pc64.txt` |
| negative control (3 corrupted ret) | 5997/6000, ret_bad=3, nothing else | `replay_negctl.txt` |
| FPATAN hook self-test (mechanics only: TOP, tag, pop) | 1000/1000 | `aileaf_unicorn.py --selftest` |
| G-CTRL, knob OFF, final build | `ae67b13a / c8317ce2 / 31db9384` | `verify/d3_gatefire_20261008/AL_CTRL.*` |
| caller scan (rel32) | 0x00414570 / 0x00415880 also called from 0x00416a30 / 0x00417da0: both 0 hits in the census (`../not_executed.tsv`); 0x0046d570 from 0x0040ca00 (runs, no exe body) and 0x00414f00 (0 hits) | `re/tools/callsites.py` |

**Arm coverage of the synthetic rows** (`synth_gen.txt`):
- 0x00415880 reaches both latch branches with return 1 (33 fresh latches, 70 held), and changes the AI
  block on 1,416 rows.
- 0x0046d570 covers acos ±1 (parallel and anti-parallel inputs) and the zero vector.

## 3. Findings

1. **Unicorn's FPATAN is not the x87's.**
   - Unicorn 2.1.4 (QEMU softfloat) disagreed with the host CPU on 2000/2000 probes, in the low mantissa bits.
   - The harness now runs the two FPATAN sites, 0x004233fb and 0x004a33b6, on the host through a
     ctypes x64 thunk.
   - Unicorn does honour PC_24 for FMUL (probe: mantissa rounded at 24 bits).
2. **This replay passes with the oracle fixed or not, because no row reaches FPATAN with an
   input where the two differ after the float store.** Finding 3 was the real cause of the first failure.
   The host-FPATAN hook stays, because it is the correct oracle.
3. **The first replay ran at the wrong control word.**
   - `_controlfp_s` takes MSVC's abstract encoding (`_PC_24` = 0x20000, `_RC_UP` = 0x200). Passing it
     the hardware word 0x027f selected round-up at PC_64, and 0x007f selected PC_64.
   - That run gave 5,778/6,000, with +1 ULP bias.
   - The replay now loads the hardware word with `fldcw`.
   - Exact arithmetic confirmed the Unicorn side: π × 57.2957992554 rounds to 180.00006103515625 at
     every precision.
4. **`Ai/HeadingAtan2.cpp` (an exe TU) reads `ds:[0x005ccae0]` and the other .rdata constants
   directly.** In the exe those read 0, because only 0x005cd5a8 and 0x005cc560 are restored.
   - It has no exe caller today: `SplineCurvature` uses the C++ `Heading004233e0`. So it has no live effect.
   - Recorded for the tracker; not changed here.
5. **Integration (predict-a-port's-output).**
   - Without `MASHED_PATHPROG`, the progress 0x008a96e8 + v*0x30c and the node count 0x0066d6d8 have no
     exe writer. 0x00414570's "car ahead in race" test would then admit every live car.
   - The AI factor 0x0089a4c0 + v*0x74 is 0 in the exe (original non-zero, `../inputs_live.json`), so a
     return-1 `cand` collapses to `look`.
   - So `AiLeaf_Enabled()` requires `PathProg_Enabled()`.
   - 0x0089a4c0 stays an unproduced input. Its original writers are 0x00414208 (FUN_00414180, behind
     `MASHED_SUBSTATE`) and 0x00417ca0 (FUN_004177b0).
6. **0x00415880's state writes in the exe.** The countdown `0x0089a508 -= 0x007f1008` is a no-op,
   because the exe has no writer for 0x007f1008 (the original runs 50, the frame dt). Its first gate
   reads `table[0x0089a374 + ftol(0x0089a360) * 5]` = `table[0 + 2*5]` = 0 → return 0. That is the
   same on both sides in regime A.

## 4. Confidence

No promotion. These are exe-only bodies with exe host bindings (rule: exe-only bodies stay below C3).
The C3 path would need .asi shared-TU bodies + `run_diff` / `run_verify_hook`; it is not started.
The tracker mutation (exe_file, notes) goes through re-classify in Phase 2.

## 5. Reproduce

```
py -3.12 re/tools/aileaf_unicorn.py verify/d3_bulk_20261011/aileaf/synth.tsv
py -3.12 re/tools/aileaf_unicorn.py <scratch>/synth_pc64.tsv --cw 37f
py -3.12 re/tools/aileaf_unicorn.py <scratch>/synth_corrupt.tsv --corrupt 3
pwsh -File verify/d3_bulk_20261011/aileaf/run_replay.ps1 -In <tsv> -Tag <tag>
pwsh -File verify/d3_gatefire_20261008/run_h4.ps1 AL_CTRL
```

# Phase 2 (2026-10-11): knob ON, per PREREG_AILEAF.md

Verdict: **G-AILEAF-INERT FAILS as registered; the port is faithful to its inputs.**
- FUN_00414570 returns 1 on 19 of 17,466 calls in AL_ON: car 3, step frames 2021..2039, all committed as mode 1.
- G-AILEAF-LIVE re-ran the exe's exact inputs through the original bytes: 368/368 exact, including the
  same 19 returns of 1 with identical `cand`.
- So the failed prediction was the PREREG's evidence base, not the transcription.
- Decision (as registered): the knob path is NOT accepted. `MASHED_AILEAF` stays default OFF and is not
  stacked into other arms. No default change.

## Arms (`verify/d3_gatefire_20261008/run_h4.ps1`)

| arm | step | gates | ptrace | trace: ret1 14570 / 15880 |
|---|---|---|---|---|
| AL_CTRL (knob OFF) | ae67b13a | c8317ce2 | 31db9384 | n/a |
| AL_PP (PATHPROG base) | 4e250237 | 22cceec1 | dc72ab09 | n/a |
| AL_ON | 7018f1b0 | 8e5cdccb | 5589b6f8 | 19 / 0 (of 17,466 calls each) |
| AL_ON2 (repeat) | 7018f1b0 | 8e5cdccb | 5589b6f8 | 19 / 0 |
| AL_ON53 (`MASHED_AILEAF_PC=53`) | 7018f1b0 | 8e5cdccb | 5589b6f8 | 19 / 0 → G-AILEAF-PC passes |
| AL_SEED (diagnostic: AI factor 0x0089a4c0 = 2) | 439d8f20 | eae86863 | 2bf268fe | 19 / 0 → H-4C0 not sufficient |
| AL_LIVE (AL_ON + input capture) | = AL_ON | = AL_ON | = AL_ON | 19 / 0 |

**Mode-2 accepts:** none (0x00415880 returned 0 on every call).

## Why the prediction failed

The inputs at the 19 calls (`AL_ON.aileaf.tsv` `#in14570` lines):
- **Target:** car 3's only eligible target is car 0, the player. It is the only car with slot-state 1, from the
  exe's default-ON player slot seed.
- **Gates passed:**
  - car 0 is ahead by 0.25 progress (nodes = 30);
  - both speeds are above 2500 (2825 and 3218);
  - the gap is 1.07 units;
  - the angle gate passes.
- **Why the original never showed this:** the "0 of 6,696" evidence (PREREG_P3.md:44) comes from
  `scenario_launch` captures where the player mostly sits still (`vel=[0,0,0]` in the launch logs). There the
  target-speed gate cannot pass.
- **Conclusion:** the PREREG compared the knob against a scenario that never reaches this arm.

**H-4C0 (the blank AI factor) is not the cause.** Seeding the original's value (2) moved `cand`
(1.26 → 1.36) but not the decision.

## G-AILEAF-LIVE (`re/tools/aileaf_unicorn.py x --check <liverows.tsv>`)

- **Input:** `AL_LIVE.liverows.tsv`, every return-1 call plus every 50th call, as full input rows. The rows hold
  the exe's spline bytes, the 4 cars' rebound inputs, the globals, the AI block, and the hardware CW at the call.
- **Result:** 368/368 exact. (exe_ret, orig_ret) = (0,0) 349, (1,1) 19.
- **Negative control:** 3 flipped expectations (1 ret, 2 cand LSBs) are caught 3/3.

## Report-only

**(b) per regime:** AL_PP and AL_ON are identical (0/3 regime A, same items and values). The 19 calls fall after
the 220-step window. The PATHPROG base arm alone is 0/3, against the default build's 2/3; that is a PATHPROG
effect, not this knob.

**Collateral** (`collateral.py --mode paired`, A = AL_PP, B = AL_ON, floor-B = AL_ON2, so the floor is 0;
`collateral_AL_PP_vs_AL_ON.csv`):
- car 3 diverges from step frame 2021 (ai_mode, then c1 at 2024, then its physics state from 2025);
- car 2 from 3231, through interaction;
- car 1 never.
- **D2 watch list:** `rec_4a4` is not divergent on any car. No D2 REOPEN CANDIDATE.

## What would move this group

- **An original capture with a moving player near an AI car.**
  - Concretely: `scenario_launch` with the player driven, plus an entry/leave probe on 0x00414570 with the same
    input columns as `MASHED_AILEAF_LIVEROWS`.
  - It would show whether the original reaches the return-1 arm in a scenario like the exe's.
  - This is the real equivalence test. G-CTRL scenarios differ between the two sides (the exe's player is
    driven; the original's is not).
- **A user decision** on whether faithful-to-inputs + replay-exact is enough to keep the knob available for
  stacking.
