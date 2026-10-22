# RESULT dcphys: physics A-chain dual copies 0x00467650 (A6a) and 0x0046b540 (A3) are one body each (2026-10-21)

PREREG `verify/dcphys_20261021/PREREG_dcphys.md` (commit `9c67e3aec`, before any knob arm). Branch `work/dcphys` from `23dbc016c`.
Private root `D:\MashedWT\dcphys_root`; evidence `D:\MashedWT\dcphys_data` (`MOVED_OFF_GIT.txt`). Baseline (this worktree, no
change) exe `e11f4c2f` / asi `20802cf8`; final exe `ac04ba48` / asi `2c83d795` (`D:\MashedWT\dcphys_data\final`).

## Verdicts

| pair | faithful body | what changed | allowlist |
|---|---|---|---|
| A6a `0x00467650` | the exe body (`Vehicle/Integrate2.cpp`): bit-identical to the .asi's C4 verbatim copy on every sampled call; with the new `MASHED_A6A_DOT_ORDER=1` bit-identical to the ORIGINAL on every sampled call, which neither copy was | one TU in both rsp files; .asi verbatim body + 14 naked shims deleted; dead `+0x478` clamp removed; new knob default OFF | `CROSS-TARGET:00467650` removed |
| A3 `0x0046b540` | the exe body (`Vehicle/VehicleInit.cpp`), bit-identical to the original on every call (full record + 4 globals + return) | one TU in both rsp files; .asi verbatim body deleted; hull builder moved to the spawn loop (original order); full 13-entry handling table from the image; `_DAT_005cea44` image bits | `CROSS-TARGET:0046b540` removed |

`scripts/lint_rva_bodies.py`: 23 findings, all allowlisted, 0 NEW (was 25 with these two). `0x0046b1c0` (U-9155) stays a
deliberate split; only its exe file changed (`Vehicle/VehicleContactHull.cpp`, moved verbatim).

## 1. In-process A/B on the original (dev .asi, hook installed live, canonical recipe unless noted)

Each sampled call: snapshot, run the ORIGINAL (trampoline), run the ORIGINAL again from the snapshot (`noo`, the no-op control
added in this lane), run the port, compare the full 0xd04 record (A3: + `0x00613108/14/30/3c` + return).
`MASHED_A6A_SELFTEST_CAP=20` (x20 quotas). Logs in `D:\MashedWT\dcphys_data\{a6a_v1,ab2,ab_final}`.

| run | body | A6a calls / bit-identical | A3 calls / bit-identical | noo |
|---|---|---|---|---|
| run1, track 0 | shared, exe default knobs | 1440 / 1266 | - | (field not yet logged) |
| run2, track 0 | verbatim .asi copy (control) | 1440 / 1266, log byte-identical to run1 | - | - |
| run3, track 0 | three-way | 1376 / 1351; verbatim vs shared differ on 0 of 1376 | - | - |
| run4, track 0 | three-way + noo | 1376 / 1351; verbatim vs shared 0 of 1376 | - | 0 on 1376 |
| r1, track 0 | shared, `DOT_ORDER=1` | 1440 / **1440** | 12 / **12** | 0 |
| r2, track 11 | shared, `DOT_ORDER=1` | 1180 / **1180** | 12 / **12** | 0 |
| r3, track 11, 90 s | shared, `DOT_ORDER=1` | 1225 / **1225** | 16 / **16** | 0 |
| r4, track 0, `--boost 300` | shared, `DOT_ORDER=1` | 1266 / **1266** | - | 0 |
| f1, track 0, final asi (verbatim deleted) | shared, `DOT_ORDER=1` | 1440 / **1440** | 12 / **12** | 0 |

Reading: the residual of run1..run4 is common to both ported bodies (the verbatim copy misses the same dwords on the same calls),
lies only in `+0x9b0..+0x9c4` (wheel forces equal), and closes completely with the original's association of A6a's three 3-term
dots (PREREG section 1, row 6). The old C4 sample (72/72) used the default quotas, which fill within the first ~100 calls; every
call up to #723 of run1 is clean too. **Coverage gap:** no airborne A6a call was sampled in any run (`air=0` on all, Warzone
45 s / 90 s and `--boost 300` included); the airborne path skips block #4's suspension force and clamp #6 and runs a subset of
the code exercised here. The verbatim copy's airborne 20/20 (2026-07-01) covered the old body only.

## 2. Gates on the exe (recipe F)

Arms on the frozen final exe `ac04ba48` (`run_arms.py`, recipe F, private root); arm logs and scores in
`D:\MashedWT\dcphys_data\arms`. The committed source rebuilds to `.text` / `.data` / `.reloc` byte-identical to it (2 bytes of
`.rdata` differ: the link timestamp). Reference = d3close / soflip on the same base.

| gate | scorer | reference | knob OFF (default) | `MASHED_A6A_DOT_ORDER=1` |
|---|---|---|---|---|
| G-CTRL | sha8 step / gates / ptrace (`H_*`, x2) | `f78de789 / 43bbe0fb / 161b1a6a` | **`f78de789 / 43bbe0fb / 161b1a6a` x2, equal** | `23ac657a / 7bf2f27f / 3186dd09` x2 (step 17861 rows) |
| (b) step hashes | `B_*_K0 / K1 / K26` | `9bd86ffe / 433abcd2 / 1a1ff727` | **equal** | `0e3790c4 / 55b84dd8 / 2324715b` |
| (b) envelopes | `vsum.py` | A 3/3, B 3/3, C 3/3 | **9/9** | **9/9** (flag_a368 {0} / {0,1} / {0,2}) |
| (e) | `re/tools/ai_speed_env.py --check` on K0 | PASS 3/3, launch 1425.7 / 2052.5 / 2055.0, ft_median_m0 2551.6 / 2052.5 / 2278.1 | **PASS 3/3, same values** | **PASS 3/3, same values** |
| D2 | `score_d2_v2.py port`, whole log, 3 runs alone | PASS 3/3 (slip 0.1900 / 0.2550, drvmed 1965.67 / 1965.67 / 1970.58, H1 398/400) | **PASS 3/3**: slip 0.1900 / 0.2550 x3, drvmed 1965.35 / 1965.35 / 1965.67, H1 398/400 x3 | **PASS 3/3**: slip 0.1900 / 0.2550 x3, drvmed 1965.35 / 1965.35 / 1966.66, H1 398/400 x3 |
| 13 tracks | `run13.py run` + `score` vs `cites_rres.json` | 39 frames, 877 rows, 0 RED, gates PASS (soflip) | **score table identical to soflip's**, gates PASS | 39 frames, 0 RED, every row cited, gates PASS; **3 of 39 frame rows moved** vs soflip (07_Roundabout 1091 matched 312 vs 314, 11_Warzone 584 326 vs 324 / 24 vs 25 rows, 13_Training 884 215 vs 213 / 20 vs 21 rows) |
| lint | `scripts/lint_rva_bodies.py` | 25 findings / 24 entries | **23 findings, 0 NEW, 22 entries** | - |

Every knob-OFF value equals the reference, as PREREG section 2 predicted, including the 13-track replay (no layout move, so no
no-op control was needed). The knob-ON values match PREREG section 4 on every pass/fail gate; G-CTRL moves and repeats. **One prediction missed:** PREREG
section 4 predicted the 13-track table identical with the knob ON (pose replay). It is not: 3 frame rows moved, all still cited
and 0 RED. Same exe for both runs, so the knob is the cause; this replay does not isolate A6a. Diff kept in
`D:/MashedWT/dcphys_data/t13_dot_vs_soflip.diff.txt`.

## 3. Decision needed (Mariano)

1. **Flip `MASHED_A6A_DOT_ORDER` default ON?** DECIDED 2026-10-22 (Mariano): flipped, section 5. For: the knob-ON body is bit-identical to the original on 6551/6551 sampled
   calls where the default body misses 25..174 per race; every gate passes with it ON. Cost: new references, G-CTRL
   `23ac657a / 7bf2f27f / 3186dd09`, B `0e3790c4 / 55b84dd8 / 2324715b`, and 3 moved 13-track rows (0 RED). If flipped, the
   same change belongs in the .asi default seed (`PhysicsChainHooks.cpp` `A6aAsiDefaults`), and `0x00467650` can go C2 -> C4
   on the evidence already recorded here. Not flipped in this lane.
2. **A3 `0x0046b540` C2 -> C4** is applied on the lane (`apply_trackers.py`, re-classify gate check below). The exe's selector
   input stays the measured type 6 for every car (U-9783, filed); that is the standalone's missing producer of
   `*(int**)0x005f2770`, not A3's body.
3. **The .asi's A6a knob defaults** are now seeded in the .asi (LC0 / GEAR_ORDER / SPIN_ORDER / DRIVE_NORM_LUT) so the dev
   harness keeps computing what its deleted verbatim copy computed. A dev-only choice made in this lane; say if the .asi should
   instead follow the exe's defaults exactly.

Re-classify gate check: A3 C4: shared TU `Vehicle/VehicleInit.cpp` hooked by `RH_ScopedInstall(A3_Entry, 0x0046b540)`, callees
`0x00430790` / `0x0042f6a0` / `0x00431d70` / `0x0040ce80` C3 and `0x004c3b30` C4, plate
`re/analysis/bucket_vehicle_00453f30_00482030/0046b540.md`, hook live in the canonical scenario, clean full-state A/B, no stub
in the body, its one `[UNCERTAIN]` filed as U-9783 (input provenance, not the body). A6a: refused above C2 at the default
(the default body is not clean against the original); the C4 evidence is for the knob-ON body.

## 4. Noted, not acted on

- `Ai/AiStandalone.cpp:1642` `GearConstGet()` reads the absolute `0x0061313c` in the exe (`F32`, `Ai/AiState.h:97`), which no
  exe TU writes (A3 publishes only `_DAT_00613108` there). It belongs to the `0x004177b0` allowlist entry's input problem.
- `0x0046b1c0` (U-9155) could fold the same way (one TU + record-base thunk); left split as the kickoff asked.
- Allowlist after this lane: 22 entries (9 CROSS-TARGET, 13 DUP-IN-TARGET keys; the lint counts 23 findings). The other three
  physics A-chain rows of ROADMAP §D4 are NOT on the allowlist, but `hooks.csv` still has `exe_file != file` for each, so the lint
  under-counts them as it does the AI copies: `0x00468980` A6b C4 (`AeroStabilize.cpp`), `0x0046ddb0` A5 C2
  (`ForceIntegrator.cpp`), `0x00470670` A4 C3 (`VehicleControl.cpp`), all against `PhysicsChainHooks.cpp`. Same recipe applies.

## 5. Amendment 1 (2026-10-22): `MASHED_A6A_DOT_ORDER` default ON (Mariano's decision)

PREREG section 5 (commit `d1cc91a48`, before the flip was measured). `Core/FaithDefaults.cpp` seeds `A6A_DOT_ORDER=1`; the
.asi's `A6aAsiDefaults` seeds it too and now honours `MASHED_NO_<name>`. Flip build exe `3a2c122a` / asi `a2d8d6ba`
(`D:\MashedWT\dcphys_data\flip`); arms in `D:\MashedWT\dcphys_data\arms_flip`. **Every prediction held.**

| arm | prediction | measured |
|---|---|---|
| `H_DEF` x2 | `23ac657a / 7bf2f27f / 3186dd09` | equal, x2 |
| `B_DEF_K0 / K1 / K26` | `0e3790c4 / 55b84dd8 / 2324715b`; (b) 9/9; (e) 3/3 same values | equal; 9/9; PASS 3/3, launch 1425.7 / 2052.5 / 2055.0 |
| `D2_DEF_1..3` alone | PASS 3/3 | PASS 3/3: slip 0.1900 / 0.2550 x3, drvmed 1965.67 / 1963.95 / 1964.25, H1 398/400 x3 |
| 13 tracks | the `t13_dot` table | identical to `t13_dot`: 39 frames, 875 rows, 0 RED, all cited, gates PASS (no layout effect) |
| `H_NODOT` | `f78de789 / 43bbe0fb / 161b1a6a` | equal |
| `H_NF` | `997f4117 / e6288905 / a63bdcfa` | equal |
| .asi default, no env, track 0, x20 | A6a 1440/1440, noo 0 | A6a 1440/1440, noo 0 on 1440; A3 12/12 |
| .asi `MASHED_NO_A6A_DOT_ORDER=1` | residual returns | 1266/1440 (the 174 of run1) |

`0x00467650` C2 -> C4 through re-classify (`apply_trackers2.py`): hook live, canonical scenario, the default body bit-identical
to the original on every sampled call, no stub in the body. Its exe-side game-mode input is a stand-in (constant 6 except the
d2engage hold tick); filed as U-9784 with A4 / A5 / `BodyOrientationIntegrate.cpp`, which read the same word. The airborne
branch is still unsampled (section 1).
