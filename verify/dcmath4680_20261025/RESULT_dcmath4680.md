# RESULT dcmath4680: one body for 0x004c4680 (D4-DC-MATH-4680, W0-8)

**Verdict: GREEN. The default is unchanged and the 0x004c4680 allowlist key is gone. One decision row is filed
(delete the std::sqrt opt-out stand-in).** Every PREREG gate held (`PREREG_dcmath4680.md`, commit `ab7b76dd4`, committed
before any arm).

## What changed (commit `6c80a548a`, base `23b3a5b2a`)
- `Math/RwMatrixOrthoNormalize.cpp` (new, exe-only, SSE2) holds the only 0x004c4680 arithmetic:
  `RwMatrixOrthoNormalizeLut(dst, src)`, the listing `0x004c4680..0x004c4a41`.
- `Track/WarzonePontoon.cpp` `OrthoNormalize` is a call into it. Its transcription is deleted.
- `Vehicle/BodyOrientationIntegrate.cpp`: `MASHED_ORTHO_LUT=1` calls the shared body in place. `MatrixOrthoNormalizeRw`,
  the second transcription, is deleted. The std::sqrt `MatrixOrthoNormalize` stays for the opt-out only. It has no RVA
  header (port-only arithmetic).
- `re/tools/dual_copy_allowlist.txt`: key `DUP-IN-TARGET:004c4680` removed. rva-lint now reports 2 findings
  (`004c4d20`, `004177b0`), NEW=0. `verify/savelink_20261023/run_arms.py` gains stacks `NOOL` and `PR`.

## S1: the knob was already ON. The row's "default OFF" is stale.
`Core/FaithDefaults.cpp:30` (`exe_sources.rsp:287`) seeds `MASHED_ORTHO_LUT=1` before any knob read. This is the faithflip
of 2026-10-20 (`verify/faithflip_20261020/RESULT_faithflip.md:39`, opt-out `MASHED_NO_ORTHO_LUT`). So the default
exe already runs the listing body in the D2 integrate step. The std::sqrt stand-in runs only under `MASHED_NO_ORTHO_LUT=1`
or `MASHED_NO_FAITH=1`. There is no flip to ON left to file. The decision filed is whether to delete the stand-in.

## S2: both callers are reachable in mashed_re.exe (`disasm_extract.txt`)
exe sha256 `03c7edb9...e49824`, image base `0x10000`. The body `0x001220F0` is called from `TrackBody` (`0x0003F8C2`),
from `RunReplay` (`0x0003E0B8`, `0x0003E5DA`, `0x0003E636`) and from `BodyOrient_IntegrateStep` (`0x00083B05`, the knob
arm). The stand-in `0x00084090` is called from `BodyOrient_IntegrateStep` (`0x00083B0F`). `StepSubstep` calls
`BodyOrient_IntegrateStep` at `0x0008AFE8` (the D2 physics substep). This is unlike 39b0, where the body was unreachable:
here both paths are live code.

Other inline 0x004c4680 forms without RVA headers: a source grep for ortho-normalize forms found none outside these
files. The 0x004c39b0 inline copies that feed D2 (`Collision/ContactDeps.h`, `Ai/AiStandalone.cpp`, the 39b0 RESULT)
are a different RVA and are not touched.

## G1: default gates (refs from `re/orchestrator/merge_train.json`)
| arm | step | gates | ptrace | ref |
|---|---|---|---|---|
| H_DEF | ef5f2e10 | 57f7ff83 | b4f4b55a | ef5f2e10/57f7ff83/b4f4b55a = |

| arm | K0 | K1 | K26 | ref |
|---|---|---|---|---|
| B_DEF | c37d4c0c | 0a8f9806 | 7013671d | c37d4c0c/0a8f9806/7013671d = |

## G-PON: the pontoon caller's move from an x87 TU to SSE2 (S3)
`W_PR` replays `verify/pontoon_20261016/OP3.msd.pontoon.csv` through the port: **`# rows 1073 rows_with_diff 0`**, and the
rest line reads `ortho(mts)-vs-original 1`. Both match `verify/pontoon_20261016/arms/R5_replay.csv`. Committed as
`W_PR_replay.csv` (28 KB). PASS.

## G3: D2, solo (`score_d2_v2.py`, whole-log window, band.json; `score_d2.txt`)
| run | lines | slip1500 | slip2000 | drvmed | H1 |
|---|---|---|---|---|---|
| D2_DEF_1 | 1633 | 0.1900 | 0.2550 | 1965.67 | 398/400, 1372.1 |
| D2_DEF_2 | 1634 | 0.1900 | 0.2550 | 1966.66 | 398/400, 1372.1 |
| D2_DEF_3 | 1636 | 0.1900 | 0.2549 | 1970.58 | 398/400, 1372.1 |

All 3 are in band. The DEF reads match the 39b0 lane's DEF (1964.25..1965.67, same slips, same H1). PASS.

## R-OPT (report only): the opt-out `MASHED_NO_ORTHO_LUT=1` on today's default
| arm | hashes |
|---|---|
| H_NOOL | d3d33df8 / 8de69cfb / 3526a7b3 |
| B_NOOL K0/K1/K26 | a4f9bb5c / 274a106f / 49eac6c2 |

| run | lines | slip1500 | slip2000 | drvmed | H1 |
|---|---|---|---|---|---|
| D2_NOOL_1 | 1635 | 0.1867 | 0.2500 | 1923.16 | 398/400, 1413.4 |
| D2_NOOL_2 | 1634 | 0.1867 | 0.2500 | 1923.16 | 398/400, 1413.4 |
| D2_NOOL_3 | 1636 | 0.1867 | 0.2500 | 1923.21 | 398/400, 1413.4 |

As predicted, the opt-out moves every G-CTRL and B hash. It stays in the D2 band. The B regimes were not scored per
car (report only).

## Decision filed
Delete the std::sqrt stand-in `MatrixOrthoNormalize` and the `MASHED_NO_ORTHO_LUT` opt-out, so the exe has one
0x004c4680 path? The default already runs the shared body, so the expected G-CTRL / B with it deleted are today's refs:
`ef5f2e10/57f7ff83/b4f4b55a`, `c37d4c0c/0a8f9806/7013671d` (G1). Options: A, delete it (the `ORTHO_LUT` FAITH entry and the
knob read go with it). B, keep the opt-out as a bisect aid. Recommendation: A. The opt-out only reproduces a pre-faithflip
state, and its hashes are recorded above.

## Tracker follow-up (not done here: trackers only via re-classify)
U-9741 (DEFERRED D-11079): with this lane, its 0x004c4680 half is one body. Its 0x00532b80 half was already resolved by kfdot.
The row's resolution path ("switch each caller to the listing body behind a knob, measure D2 and the Dump rows") is met
for 0x004c4680. This unblocks D3-CF-D11079.

## Raw files (not committed, `D:/MashedWT/D4-DC-MATH-4680_data`)
| file | sha256 |
|---|---|
| arms/H_DEF.step.csv | ef5f2e105f649cdbf1a900340eb128ac8317fce986d36538248e3b686d1178ae |
| arms/H_DEF.gates.csv | 57f7ff835cd9b7d774b939f08e091cf4b5faeca58cdcbd7fad0b41563fe134c6 |
| arms/H_DEF.ptrace.log | b4f4b55a20e57e3a37b1643703bfa8e06d6973561e943bdaf369112aaefeb28f |
| arms/B_DEF_K0.step.csv | c37d4c0cfbdd5843f946feec67c0d3559ac17613d2520cf86ac53f6f79935f84 |
| arms/B_DEF_K1.step.csv | 0a8f9806be171e483c1626fc0f985716a6412dd803e33587f408024e3870f1ec |
| arms/B_DEF_K26.step.csv | 7013671dfea95c72b49f484ddae75cefeed87cb0f95d34495a3ba59aec6e6dc0 |
| arms/W_PR.replay.csv (= committed W_PR_replay.csv) | 8af824a0f340480b03c3cd67f03130f5bee7a42900fd99ff25223339b3c3cd68 |
| arms/W_PR.step.csv | d29c714570caa3dbe85bf246d1e238aeb4d2518f71edf7de9b6a214353eb50f1 |
| arms/H_NOOL.step.csv | d3d33df8eb813fc929866e17e49323d31b17c1fbf00057c56a5303c8779de07e |
| arms/H_NOOL.gates.csv | 8de69cfbfa47778b169b8a54576b982a9da42e4ad52e23eb03c5b0b29410c6e4 |
| arms/H_NOOL.ptrace.log | 3526a7b360ca942348a9ae7423cf883ef60b60c72950c26c17ed1698b8de84e4 |
| arms/B_NOOL_K0.step.csv | a4f9bb5c30c4832f928c922dd7cdada67f945236568f34715fc4e2034e4047df |
| arms/B_NOOL_K1.step.csv | 274a106f0b1b889852f95e436ef1a1c9a8426c2e0d97d4f55e44b15d5674fb5c |
| arms/B_NOOL_K26.step.csv | 49eac6c2d12ae395aac323e874a1feb090b92820856a5acc1fca09860a14ed63 |
| arms/D2_DEF_1.motion_diag.log | b9d24cab9ade807e784a0f400d827c9e0eea29e6a4eb52dfbfad723e92011d98 |
| arms/D2_DEF_2.motion_diag.log | 7f0e420aa83936dce1b6cd4fe3966355c3eb0356809245fc72c5a536e2355669 |
| arms/D2_DEF_3.motion_diag.log | b7b66701ebd6a5a924291fe388ac459ec90980dcef1c23595c3f214ea86644d6 |
| arms/D2_NOOL_1.motion_diag.log | 8e64151955b65fe03b85ee2aed3356ec5369a9d90743db7e147cacfc0aac31b4 |
| arms/D2_NOOL_2.motion_diag.log | b4969640e4b5887472c906b534a2a6fc4d5c5b16c44ff10f0e80b53694d2a433 |
| arms/D2_NOOL_3.motion_diag.log | 655dcdeb1b76b495215b27443d9bd6596b8f53e34883d894819130c2d48398a0 |
| exe_disasm.txt (dumpbin /disasm) | 1d5e53d8ac2b8c0581837e88911d5ebd2eb44ec0d19c8c2246a5739148fdece0 |
| mashed_re.map | 05eaa11780239b1b31d923fb82b6d72f6dd780463e6eddc651eeea024eefe2e0 |
| mashed_re.exe | 03c7edb9366cacf4e8d1f7be3716c7ebe03c6aed19c568268ca4870ae2e49824 |

Committed here: this RESULT, the PREREG, `score_d2.txt`, `RUNLOG.txt` (hash list), `disasm_extract.txt`, `W_PR_replay.csv`.
Runs: `mashed_lock.py run --solo --label D4-DC-MATH-4680`, private root `D:/MashedWT/D4-DC-MATH-4680_root`.
