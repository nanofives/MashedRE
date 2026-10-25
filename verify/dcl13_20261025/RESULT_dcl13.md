# RESULT D4-DC-39b0-L13: delete the 0x004c39b0 exact-sqrt fallback and the 0x004c4680 std::sqrt opt-out

**GREEN.** Pure deletion for two user decisions (log/orch/DECISIONS.md L-13 = C, L-14 = A). Every default gate equals the
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

## Gates (edited tree, exe sha256 prefix `d6b04d02`, build OK `mashed_re.exe` + `mashed_re_dev.asi`, NO_DEPLOY)
rva-lint (in build.bat): `1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0`, OK.

| arm | hashes (first 8 of sha256) | ref | result |
|---|---|---|---|
| H_DEF step / gates / ptrace | ef5f2e10 / 57f7ff83 / b4f4b55a | ef5f2e10 / 57f7ff83 / b4f4b55a | equal |
| B_DEF K0 / K1 / K26 | c37d4c0c / 0a8f9806 / 7013671d | c37d4c0c / 0a8f9806 / 7013671d | equal |

D2 solo, 3 runs, `score_d2_v2.py port` (`score_d2.txt`): 1637 lines each (min 1600), slip1500 0.1900, slip2000 0.2548, drvmed 1970.58,
all marked in band, H1 398/400 median 1372.1. The three motion_diag logs are byte-identical (sha256 below).

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
