# PREREG dcmath4680: one body for 0x004c4680 (D4-DC-MATH-4680, W0-8)

Committed alone, before any arm. Branch `orch/D4-DC-MATH-4680`, code commit `6c80a548a` on base `23b3a5b2a`.
Exe sha256 `03c7edb9366cacf4e8d1f7be3716c7ebe03c6aed19c568268ca4870ae2e49824` (`MASHED_NO_DEPLOY=1`).

## Change under test
- `Math/RwMatrixOrthoNormalize.cpp` (new, exe-only, SSE2) holds the only 0x004c4680 arithmetic
  (`RwMatrixOrthoNormalizeLut(dst, src)`, the listing `0x004c4680..0x004c4a41`).
- `Track/WarzonePontoon.cpp` `OrthoNormalize` is a call into it (its transcription deleted).
- `Vehicle/BodyOrientationIntegrate.cpp`: `MASHED_ORTHO_LUT=1` calls it in place (`MatrixOrthoNormalizeRw`, the second
  transcription, deleted). The std::sqrt `MatrixOrthoNormalize` stays for the opt-out, with no RVA header.
- `re/tools/dual_copy_allowlist.txt`: key `DUP-IN-TARGET:004c4680` removed (rva-lint 2 findings, NEW=0).

## Static facts recorded before the arms
- **S1, the knob is already ON in the exe default.** The step row says "MASHED_ORTHO_LUT (u9765, default OFF)". That is
  stale: `Core/FaithDefaults.cpp:30` (in `exe_sources.rsp:287`) seeds `ORTHO_LUT=1` before any knob read (faithflip
  2026-10-20, `verify/faithflip_20261020/RESULT_faithflip.md:39`, opt-out `MASHED_NO_ORTHO_LUT`). So the default exe runs the
  listing body in the D2 integrate step, and the std::sqrt stand-in runs only under `MASHED_NO_ORTHO_LUT=1` or
  `MASHED_NO_FAITH=1`. There is no "flip to ON" left to file. The decision this lane files is the stand-in's deletion.
- **S2, both callers are reachable** (`dumpbin /disasm` + `mashed_re.map`, extract `disasm_extract.txt`): the body
  `0x001220F0` is called from `TrackBody` (`0x0003F8C2`), `RunReplay` (3 sites) and `BodyOrient_IntegrateStep`
  (`0x00083B05`, knob arm). The stand-in `0x00084090` is called from `BodyOrient_IntegrateStep` (`0x00083B0F`), which
  `StepSubstep` calls at `0x0008AFE8` (the D2 physics substep).
- **S3, the pontoon caller moved from an x87 TU to an SSE2 TU.** `WarzonePontoon.cpp` is in `x87_tus.txt`; its two
  callers run under `Pc24`. At PC_24 each x87 multiply / add rounds to a 24-bit significand as SSE single does, so the
  prediction is no change. G-PON checks it.

## Predictions and gates
- **G1 (default)**: `H_DEF` = G-CTRL `ef5f2e10/57f7ff83/b4f4b55a`, `B_DEF_K0/K1/K26` = `c37d4c0c/0a8f9806/7013671d`
  (refs from `re/orchestrator/merge_train.json`). Pass = exact. This also checks the D2 knob arm's move into another TU.
- **G-PON (pontoon replay)**: `W_PR` (Warzone, rule 8, 3600 frames, `MASHED_PONTOON_REPLAY` on
  `verify/pontoon_20261016/OP3.msd.pontoon.csv`). Pass = the replay CSV ends `# rows 1073 rows_with_diff 0` and its rest
  line reads `ortho(mts)-vs-original 1` (as `verify/pontoon_20261016/arms/R5_replay.csv`). A miss = RED (S3 refuted).
- **G3 (D2, solo)**: `D2_DEF_1..3` scored with `verify/d2rebase_20261019/score_d2_v2.py` (whole-log window, band.json).
  Each run needs >= 1600 motion_diag lines (re-run up to 2 times otherwise). Pass = all 3 in band.
- **R-OPT (report only, no pass/fail)**: `H_NOOL`, `B_NOOL_K0/K1/K26`, `D2_NOOL_1..3` with `MASHED_NO_ORTHO_LUT=1`: what
  the stand-in gives on today's default. Prediction: G-CTRL / B hashes differ from G1 (the stand-in is not the listing).

Any G1 or G-PON miss = RED.

## Decision this feeds
Delete the std::sqrt stand-in and the `MASHED_NO_ORTHO_LUT` opt-out (one 0x004c4680 path in the exe)? With it deleted the
default is unchanged, so expected G-CTRL / B = today's refs (G1). Filed as a decision row, not done here.

## Raw data
`D:/MashedWT/D4-DC-MATH-4680_data/arms` (not committed). The RESULT lists cited raw files with SHA-256.
Runs under `mashed_lock.py run --solo --label D4-DC-MATH-4680`, private root `D:/MashedWT/D4-DC-MATH-4680_root`.
