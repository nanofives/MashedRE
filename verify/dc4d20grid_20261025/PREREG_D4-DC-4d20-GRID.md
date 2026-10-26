# PREREG D4-DC-4d20-GRID: the grid spawn basis through the one RwMatrixRotate body

Committed alone, before any arm. Base `orch/integration` @ `4ff9c8468`.

## 1. Change under test
`Race/RoundGrid.cpp` `GridDir_PortYaw` builds the spawn basis' at row (`at.x = sin r`, `at.z = 1 - omc`) in double
std::sin / std::cos and hands it to `BodyOrient_NoteExactBasis` (read by `BodyOrient_Init` under
`MASHED_SPAWN_BASIS_RW`, seeded ON in `Core/FaithDefaults.cpp:28`). That is a headerless inline form of `0x004c4d20`
(`verify/dcmath4d20_20261025/RESULT_dcmath4d20.md` S5). The original computes it with
`FUN_004c4d20(m, &DAT_006146fc = (0,1,0), deg, 1)` at `0x0047061c` / `0x0047062e`, on a matrix that
`FUN_004c51a0` mode 0 (`0x004705fe` / `0x0047060a`; replace arm `0x004c5285..`) left as the identity 3x3 with flags
`0x20003`, so `FUN_004c4a50`'s mode-1 arm copies R.

Knob `MASHED_GRID_ROTATE_RW=1`, **default OFF**: the at row comes from the shared `RwMatrixRotate`
(`Math/RwMatrixRotate.cpp`, LUT inverse sqrt + PC_24 already seeded ON) on a scratch matrix, mode 0, reading m[8] / m[10].
The yaw `GridDir_PortYaw` returns is unchanged (it is the key the basis is noted under).
Diagnostic only: `MASHED_GRID_ROTATE_TRACE=<file>` writes, per call, the degree bits and both at rows (inline / rw)
with SAME / DIFF. It never changes a value.

## 2. Arms (`verify/savelink_20261023/run_arms.py`, stacks `GR`, `GT`, `GRT`; private root, `mashed_lock.py run --solo`)
- Default: `H_DEF`, `B_DEF_K0`, `B_DEF_K1`, `B_DEF_K26`, `D2_DEF_1..3`.
- Knob ON: `H_GR`, `B_GR_K0`, `B_GR_K1`, `B_GR_K26`, `D2_GR_1..3`.
- Witness: `D2_GT_1` (knob OFF + trace), `D2_GRT_1` (knob ON + trace), `H_GT` (G-CTRL + trace).
- 13 tracks: `verify/rsky_20261016/run13.py` on the default exe, scored with `cites_rres.json` against
  `verify/rpct_20261022/tracks_flip_score.txt`.
- Dual-copy scanner (`scripts/lint_rva_bodies.py` via build.bat): allowlist count at base = 1 key.

## 3. Gates
- G1 (default unchanged, required for GREEN): `H_DEF` = G-CTRL `5943c5f1/b493262b/09551e6a`, B
  `3a03670c/2516d903/53778664` (`re/orchestrator/merge_train.json`). D2_DEF 3/3 in band (`score_d2_v2.py`, whole log,
  each motion_diag >= 1600 lines). 13-track rows identical to the reference, INSTRUMENT GATES PASS. Allowlist does not
  grow, rva-lint NEW=0.
- G-RUN (the knob reaches the code): the trace files carry `gridrot` lines in `D2_GT_1` / `D2_GRT_1`; `D2_GRT_1` lines
  read `on=1`. If no `gridrot` line appears in an arm, that arm does not execute the grid basis and its ON hashes
  cannot move from this change.
- G-BITS (reported, no pass/fail): count of SAME / DIFF lines. Prediction: [UNCERTAIN] which; the LUT inverse sqrt of
  len2 = 1.0 and x87 FSIN/FCOS at PC_24 against CRT double sin/cos rounded to float can differ by 1 ulp.
- G2 (knob ON, feeds the decision): `H_GR` / `B_GR` hashes and `D2_GR` scores. If every line is SAME, the ON hashes must
  equal the DEF hashes (a mismatch then is a harness fault, re-run). If any line is DIFF, ON hashes may move: that is the
  decision input, not a RED.

## 4. Verdict rules
- GREEN: G1 PASS, G-RUN PASS, and G2 equal to G1 (no behaviour change measured). A decision row is filed for the flip
  (seed `GRID_ROTATE_RW` and delete the inline form), since flipping a default is the user's call.
- DECISION: G1 PASS and G2 moves any gate hash or D2 score. The RESULT reports the measured ON hashes and the cause
  (G-BITS lines); no merge is asked for the flip.
- RED: G1 fails on the default exe (the change leaked into the OFF path).
