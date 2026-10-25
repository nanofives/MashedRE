# PREREG dcmath39b0: one body for 0x004c39b0, knob MASHED_RWV3D_LUT (D4-DC-MATH-39b0, W0-7)

Committed alone, before any arm. Branch `orch/D4-DC-MATH-39b0`, code commit `58f260580` on base `91e8b9733`.

## Change under test
- `Math/RwV3dNormalize.cpp` holds the only 0x004c39b0 arithmetic (`NormalizeBody`). `Math::LutNormalize`
  is a call into it with the rebuilt tables (`Math/RwLutTables.cpp`); the copy there is deleted.
- `MASHED_RWV3D_LUT=1` (default OFF): `RwV3dNormalize`'s no-live-LUT branch runs `LutNormalize` instead
  of the exact-sqrt fallback. The live-LUT branch (.asi on MASHED.exe) is unchanged.

## Static fact recorded before the arms (S1)
`dumpbin /exports` of the built `mashed_re.exe` (sha256 prefix `fe6dcda61dcbf10b`): `RwV3dNormalize` at
RVA `0x000D1D60` (VA `0x000E1D60`, image base `0x10000`). `dumpbin /disasm` has exactly one reference to
`0E1D60h`: `push 0E1D60h` at `0x000126C5`, the `RH_ScopedInstall` ctor, whose callee `0x0002BBD0` is
`ret 0` (`Stubs/HookSystemNoOp.cpp` `HookSystem::Register`). No `call 000E1D60`. So the knob's branch is
unreachable in the exe.

## Predictions and gates
- **G1 (default, knob OFF)**: `H_DEF` = G-CTRL `ef5f2e10/57f7ff83/b4f4b55a`, `B_DEF_K0/K1/K26` =
  `c37d4c0c/0a8f9806/7013671d` (refs from `re/orchestrator/merge_train.json`). Pass = exact. This also
  checks that moving `LutNormalize` into another TU did not change its result for its callers
  (PathNodes, RaceCamera, RoundGrid, HeadingAssist, AeroStabilize A6B_NORM_LUT, RwRaceFx).
- **G2 (knob ON)**: `H_V3`, `B_V3_K0/K1/K26` hashes equal G1's exactly (consequence of S1).
- **G3 (D2, solo)**: `D2_DEF_1..3` and `D2_V3_1..3` scored with `verify/d2rebase_20261019/score_d2_v2.py`
  (whole-log window, band.json). Each run needs >= 1600 motion_diag lines (re-run up to 2 times otherwise).
  Pass for the knob = every V3 run in band, and the V3 set not outside the DEF set's spread on drvmed
  (D2 is wall-clock bound, so per-run values differ between identical exes; hashes are not compared).
- Any G1 miss = RED (the refactor changed default behaviour). A G2 miss with G1 green refutes S1 and
  the RESULT reports it as a finding.

## Decision this feeds
The flip of `MASHED_RWV3D_LUT` to ON is filed as a decision row, not done here. If G1-G3 hold, the
expected G-CTRL / B hashes with it ON equal today's refs.

## Raw data
`D:/MashedWT/D4-DC-MATH-39b0_data/arms` (not committed). RESULT lists cited raw files with SHA-256.
Runs under `mashed_lock.py run --solo --label D4-DC-MATH-39b0`, private root `D:/MashedWT/D4-DC-MATH-39b0_root`.
