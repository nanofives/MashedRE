# RESULT D4-DC-39b0-L13: delete the 0x004c39b0 exact-sqrt fallback, the 0x004c4680 std::sqrt opt-out and the 0x004c4d20 camera stand-in

**GREEN.** Pure deletion for two user decisions (log/orch/DECISIONS.md L-13 = C, L-14 = A, L-15 = B). Every default gate equals the
`merge_train.json` refs on the edited tree. Evidence for the deletions: `verify/dcmath39b0_20261025/RESULT_dcmath39b0.md`,
`verify/dcmath4680_20261025/RESULT_dcmath4680.md`.

## Edits
- `Math/RwV3dNormalize.cpp` (0x004c39b0): `NormalizeBody` keeps only the LUT form (the `!sqrtRoot || !invRoot` exact-sqrt branch is gone).
  `RwV3dNormalize` with no live RW LUT calls `Math::LutNormalize` always. `RwV3dLut()` and `MASHED_RWV3D_LUT` deleted.
  `Collision/ContactDeps.h` Vec3Normalize and `Ai/AiStandalone.cpp` stand-ins untouched.
- `Vehicle/BodyOrientationIntegrate.cpp` (0x004c4680): `MatrixOrthoNormalize` (std::sqrt) and `OrthoLut()` deleted; the call site runs
  `Math::RwMatrixOrthoNormalizeLut(m, m)` always. `MASHED_ORTHO_LUT` / `MASHED_NO_ORTHO_LUT` are no longer read.
- `Core/FaithDefaults.cpp`: `{ "ORTHO_LUT", "1" }` removed from `kFaith` (that row was the only place `MASHED_NO_ORTHO_LUT` had meaning;
  `MASHED_NO_FAITH` handling is generic and unchanged).
- `verify/savelink_20261023/run_arms.py`: `STACK['V3']` and `STACK['NOOL']` emptied (knobs gone). `STACK['OL']`/`RLO` (sets the now-unread
  `MASHED_ORTHO_LUT`) left as is, a harmless env var. Older `verify/*/run_arms.py` copies are historical and untouched.
- `re/tools/dual_copy_allowlist.txt`: comment text for 004c39b0 / 004c4680 updated (no key change).

- `Race/RaceCamera.cpp` (0x004c4d20, L-15 = B): `RotateAboutAxis` always runs `RwMatrixRotate(m, axis, deg, 0)` +
  `Math::RwV3dTransformVectorsCPU(v, v, 1, m)` (the original's FUN_004c4d20 + FUN_004c3df0 pair, sites 0x00441892/0x004418ab,
  0x004418cc/0x004418da, 0x00441968/0x00441976, 0x00447557/0x00447570). The std::cos/std::sin Rodrigues stand-in, `CamRotateRw()` and
  `MASHED_CAM_ROTATE_RW` are deleted. `Vec3Norm` stays (other camera callers). `Race/RoundGrid.cpp` untouched.
  `run_arms.py` `STACK['CR']` emptied.

## Gates (second pass after the L-15 edit; the first-pass table and logs are in `score_d2.txt` / `RUNLOG.txt`. Edited tree, exe sha256 prefix `d6b04d02` first pass, build OK `mashed_re.exe` + `mashed_re_dev.asi`, NO_DEPLOY)
rva-lint (in build.bat): `1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0`, OK.

| arm | hashes (first 8 of sha256) | ref | result |
|---|---|---|---|
| H_DEF step / gates / ptrace | ef5f2e10 / 57f7ff83 / b4f4b55a | ef5f2e10 / 57f7ff83 / b4f4b55a | equal |
| B_DEF K0 / K1 / K26 | c37d4c0c / 0a8f9806 / 7013671d | c37d4c0c / 0a8f9806 / 7013671d | equal |

D2 solo, 3 runs, `score_d2_v2.py port` (`score_d2.txt`): 1637 lines each (min 1600), slip1500 0.1900, slip2000 0.2548, drvmed 1970.58,
all marked in band, H1 398/400 median 1372.1. The three motion_diag logs are byte-identical (sha256 below).

Second pass (final tree incl. L-15, build OK, rva-lint NEW=0, `RUNLOG_final.txt`): H_DEF and B_DEF hashes are the same eight values as above,
equal to the refs. D2 solo x3 (`score_d2_final.txt`): 1631..1637 lines, slip1500 0.1900, slip2000 0.2548..0.2550, drvmed 1970.58 / 1970.58 / 1965.35,
all in band, H1 398/400 median 1372.1 (the 4d20 lane's D2_CR reads were 1965.35..1965.67, same band).

### G-CAM / G-POSE (L-15 extra gate; `cam_scores_L15.txt`)
Replay of `verify/d1r2_20261016/orig2_trace_v3.csv` through `verify/dcmath4d20_20261025/run_replay.py`, scored with `score_d1r2.py pose`
(175 joined racing rows). Arm `CAM_L15` on the default exe: `cam_dump.csv` sha8 **7467ebf4** and `librw_dump.csv` sha8 **6d26f891**, byte-identical to the 4d20
lane's knob-ON arm `CAM_CR` (7467ebf4 / 6d26f891). pos median 7.95543e-05, p95 7.87615e-04, max 1.75851e-03, at median 3.97552e-04 deg,
p95 2.31646e-03 deg, max 6.90082e-03 deg, right median 2.09131e-06 deg: all equal to CAM_CR. G-POSE PASS.

Not run: unit tests. The tree has no unit-test target built by `mashedmod\build.bat`; the build, rva-lint and the arms above are the gates.

## Raw files (kept out of git, `D:/MashedWT/D4-DC-39b0-L13_data/arms/`, full list in `../hashes.txt`)
| file | sha256 |
|---|---|
| H_DEF.step.csv | ef5f2e105f649cdbf1a900340eb128ac8317fce986d36538248e3b686d1178ae |
| H_DEF.gates.csv | 57f7ff835cd9b7d774b939f08e091cf4b5faeca58cdcbd7fad0b41563fe134c6 |
| H_DEF.ptrace.log | b4f4b55a20e57e3a37b1643703bfa8e06d6973561e943bdaf369112aaefeb28f |
| B_DEF_K0.step.csv | c37d4c0cfbdd5843f946feec67c0d3559ac17613d2520cf86ac53f6f79935f84 |
| B_DEF_K1.step.csv | 0a8f9806be171e483c1626fc0f985716a6412dd803e33587f408024e3870f1ec |
| B_DEF_K26.step.csv | 7013671dfea95c72b49f484ddae75cefeed87cb0f95d34495a3ba59aec6e6dc0 |
| D2_DEF_1..3.motion_diag.log | 0ecd557eaee1ea08f2498af67fd9dbef3ba1965a24fe944b7abe611da42faf45 (all three) |

Committed here: this RESULT, `score_d2.txt`, `RUNLOG.txt`.

Second-pass raw files (`D:/MashedWT/D4-DC-39b0-L13_data/`, list in `hashes2.txt`): H_DEF / B_DEF hashes identical to the table above;
`cam/CAM_L15/cam_dump.csv` 7467ebf471621dd5d8f8b8a5e147975c1d9a3dcacd18973b18fe920ddf448c5e,
`cam/CAM_L15/librw_dump.csv` 6d26f8915c7b114a1621f0752521338a7d36e8106fb433e7d1ae933e303d0244,
D2_DEF_1 0ecd557eaee1ea08f2498af67fd9dbef3ba1965a24fe944b7abe611da42faf45, D2_DEF_2 b7b66701ebd6a5a924291fe388ac459ec90980dcef1c23595c3f214ea86644d6,
D2_DEF_3 aacb6a3c1e1d0a308d9a0619547a9a6ebf28c6ab64f387f9c74a21d506786314.
Also committed: `score_d2_final.txt`, `cam_scores_L15.txt`, `RUNLOG_final.txt`.
