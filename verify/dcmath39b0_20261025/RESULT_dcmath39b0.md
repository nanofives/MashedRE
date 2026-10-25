# RESULT dcmath39b0: one body for 0x004c39b0 (D4-DC-MATH-39b0, W0-7)

**Verdict: GREEN, knob default OFF. Flip filed as a decision row.** Every PREREG gate held
(`PREREG_dcmath39b0.md`, commit `7be723a86`, before any arm).

## What changed (commit `58f260580`, base `91e8b9733`)
- `Math/RwV3dNormalize.cpp` holds the only 0x004c39b0 arithmetic (`NormalizeBody`). `Math::LutNormalize`
  is a call into it with the rebuilt tables. The copy in `Math/RwLutTables.cpp` is deleted.
- `MASHED_RWV3D_LUT=1` (default OFF): `RwV3dNormalize`'s no-live-LUT branch runs `LutNormalize` (no
  error report, so no read of `0x005d757c` and no call to `0x004d7ff0` inside the exe image) instead of
  the exact-sqrt fallback. The live-LUT branch (.asi on MASHED.exe) is unchanged.
- `re/tools/dual_copy_allowlist.txt`: key `DUP-IN-TARGET:004c39b0` removed (rva-lint 6 -> 4 findings,
  NEW=0; it was one key, two findings, exe and asi). `run_arms.py` gains stack `V3`.

## S1, static: RwV3dNormalize is unreachable in mashed_re.exe
exe sha256 `fe6dcda61dcbf10b100d2c8a7240a581e3641d8386e75708c2a334168530ed73`. Export RVA `0x000D1D60`
(VA `0x000E1D60`, image base `0x10000`). The full disassembly has one reference to it: `push 0E1D60h` at
`0x000126C5`, the `RH_ScopedInstall` ctor, calling `0x0002BBD0` = `ret 0`
(`Stubs/HookSystemNoOp.cpp` `HookSystem::Register`). No `call 000E1D60`. Extract: `disasm_extract.txt`.

So the allowlist note "keeps its exact-sqrt fallback in the exe because it feeds D2 physics" was wrong for
this body. The exact-sqrt 0x004c39b0 forms that do feed D2 in the exe are inline copies outside this
function: `Collision/ContactDeps.h:Vec3Normalize` (exact unless `MASHED_COLL_LUT`, the U-9775 question)
and the exact stand-ins in `Ai/AiStandalone.cpp:210, 257, 902` (gated by `AiLutMask()` at 249). The
scanner does not see them because they carry no RVA header. They are not touched here.

## G1 / G2: default gates, knob OFF and ON (refs from `re/orchestrator/merge_train.json`)
| arm | step | gates | ptrace | ref |
|---|---|---|---|---|
| H_DEF | ef5f2e10 | 57f7ff83 | b4f4b55a | ef5f2e10/57f7ff83/b4f4b55a = |
| H_V3  | ef5f2e10 | 57f7ff83 | b4f4b55a | = |

| arm | K0 | K1 | K26 | ref |
|---|---|---|---|---|
| B_DEF | c37d4c0c | 0a8f9806 | 7013671d | c37d4c0c/0a8f9806/7013671d = |
| B_V3  | c37d4c0c | 0a8f9806 | 7013671d | = |

G1 also covers the move of `LutNormalize` into another TU: its callers (PathNodes, RaceCamera, RoundGrid,
HeadingAssist, AeroStabilize A6B_NORM_LUT, RwRaceFx) give the same hashes.

## G3: D2, solo (`score_d2_v2.py`, whole-log window, band.json; `score_d2.txt`)
| run | lines | slip1500 | slip2000 | drvmed | H1 |
|---|---|---|---|---|---|
| D2_DEF_1 | 1628 | 0.1900 | 0.2550 | 1964.25 | 398/400, 1372.1 |
| D2_DEF_2 | 1630 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 |
| D2_DEF_3 | 1632 | 0.1900 | 0.2550 | 1965.67 | 398/400, 1372.1 |
| D2_V3_1  | 1631 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 |
| D2_V3_2  | 1630 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 |
| D2_V3_3  | 1631 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 |

All 6 in band (drvmed 1893.63..1997.46). V3 1965.35 x3 inside the DEF spread 1964.25..1965.67.
D2_V3_2's motion_diag is byte-identical to D2_DEF_2's (`1c1d5b1b`).

## Decision filed
`MASHED_RWV3D_LUT` default ON? Expected gates with it ON: G-CTRL `ef5f2e10/57f7ff83/b4f4b55a`, B
`c37d4c0c/0a8f9806/7013671d` (unchanged, measured above). Recommendation: A (flip), or C (delete the
exact fallback and the knob). Both are gate-neutral in the exe because the branch is unreachable there.

## D3-CF-U9775 coordination
Nothing on that row's path changed: `ContactDeps.h` and `MASHED_COLL_LUT` are untouched, and
`LutNormalize`'s result is the same (G1). U-9775 stays the open question for the D2-feeding collision copy.

## Raw files (not committed, `D:/MashedWT/D4-DC-MATH-39b0_data`)
| file | sha256 |
|---|---|
| arms/H_DEF.step.csv, arms/H_V3.step.csv | ef5f2e105f649cdbf1a900340eb128ac8317fce986d36538248e3b686d1178ae |
| arms/H_DEF.gates.csv, arms/H_V3.gates.csv | 57f7ff835cd9b7d774b939f08e091cf4b5faeca58cdcbd7fad0b41563fe134c6 |
| arms/H_DEF.ptrace.log, arms/H_V3.ptrace.log | b4f4b55a20e57e3a37b1643703bfa8e06d6973561e943bdaf369112aaefeb28f |
| arms/B_DEF_K0.step.csv, arms/B_V3_K0.step.csv | c37d4c0cfbdd5843f946feec67c0d3559ac17613d2520cf86ac53f6f79935f84 |
| arms/B_DEF_K1.step.csv, arms/B_V3_K1.step.csv | 0a8f9806be171e483c1626fc0f985716a6412dd803e33587f408024e3870f1ec |
| arms/B_DEF_K26.step.csv, arms/B_V3_K26.step.csv | 7013671dfea95c72b49f484ddae75cefeed87cb0f95d34495a3ba59aec6e6dc0 |
| arms/D2_DEF_1.motion_diag.log | 9f351ebc3c62f9425fb19c0424c33fb8d961cae5dc0df2357d9eed339cd78f2c |
| arms/D2_DEF_2.motion_diag.log, arms/D2_V3_2.motion_diag.log | 1c1d5b1bd9faccbbd7bf35fa851a779896ac41b0f6213debd07edbb6f0b242b6 |
| arms/D2_DEF_3.motion_diag.log | 54a7c537f8bc1f4b6260a771c4aba182d1bb44002313e208d532ad31b1d20d14 |
| arms/D2_V3_1.motion_diag.log, arms/D2_V3_3.motion_diag.log | aacb6a3c1e1d0a308d9a0619547a9a6ebf28c6ab64f387f9c74a21d506786314 |
| exe_disasm.txt (dumpbin /disasm) | de9e1ebf0a05cc360971eba8a058d4423d45daa6efd3d0289e4521b44eb9c0d0 |
| exe_exports.txt (dumpbin /exports) | 430aa8253263d2d05778a4aeffebbaaeecb312752b237d33f627d278d6b1abf2 |

Committed here: this RESULT, the PREREG, `score_d2.txt`, `RUNLOG.txt` (hash list), `disasm_extract.txt`.
