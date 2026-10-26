# RESULT HB-6: veccap, FUN_00566200 re-transcription, race capture, FUN_005667c0

Verdict: **GREEN for the harness step, no tracker change.** Both rows stay C2. Both are
C3 *candidates* behind a default-OFF knob, and each has one named gap.

## 1. Does veccap's 80-bit capture reuse HB-1's ST0 handlers? No.

- Before this step veccap had **no 80-bit capture**. `record_race.py` read ST0 with
  `fst dword` (32 bits), `capture_vectors.py` with `NativeFunction(..., 'float')`, and
  `replay_c3.py`'s stub ended `fstp dword`.
- HB-1's `st0_ret_global` / `st0_ret_mat3_ptr` / `st0_ret_mat4x3_ptr` are arg_types in
  `re/frida/diff_template.js` (path1 `run_diff.py`). They drain ST0 as a 64-bit `double`
  through libffi (`re/frida/ARG_TYPES.md:66,84,148`). Nothing under `re/tools/veccap/` loads
  `diff_template.js` (one comment in `live_check_veccap1.py:36` names it).
- Added here: kind `x_out_in`. `record_race.py` reads all 10 bytes of ST0 at return with
  `fstp tbyte [eax]; fld tbyte [eax]` (exact, ST0 left as it was, no free register needed).
  `replay_c3.py`'s stub ends `fstp tbyte [RESULT]` and compares the 10 bytes as hex.
  Validated: fidelity leg 0 failures on 1200 live calls (live ST0 == emulated original ST0,
  all 80 bits).

## 2. FUN_00566200 / FUN_005667c0 re-transcribed in disassembly order

Listings read from `original/MASHED.exe.unpatched` with capstone (Ghidra MCP not available).
Decomp from `re/tools/decomp_pc.py` (pool10, read-only).

- `0x00566200`: row 0 and row 1 of both sums go through dword stores (`[esp+0x18]`,
  `[esp+0x1c]`, `[esp]`, `[esp+4]`). Row 2 (`e2`, `r2`) and `hy` (`0x00566249`) stay in ST.
  The sums run `(y + z) + x` (`0x00566287..0x005662fb`) and `(y*q1 + x*q0) + z*q2`
  (`0x00566312..0x00566354`). The decomp prints `x + z + y`. This is VECCAP-2.
- `0x005667c0`: the sum runs `(x*x + y*y) + z*z` (`0x005667cc..0x005667da`), and the decomp
  prints `z*z + y*y + x*x`. Every op runs at the caller's PC, so under PC_24 the SSE2 port's
  53-bit `float10` chain differs.
- New TU `Collision/RwpSolverLeaves1X87.cpp` (in `mashedmod/x87_tus.txt`, `/arch:IA32`):
  `FUN_00566200_x87`, `FUN_005667c0_x87`. The shadow wrappers in `RwpSolverLeaves1.cpp`
  dispatch to them **only under `MASHED_LEAVES1_X87=1` (default OFF)**. The default bodies
  are unchanged.
- `scripts/lint_rva_bodies.py` passes and does **not** count the new TU (no RVA-header comment
  anchors the `_x87` definitions). It is a knob-gated second body for both RVAs. A flip must
  delete the SSE2 bodies so that one body remains.

## 3. Capture scenario

- `0x00566200` has **zero live calls on all 13 tracks**: scenario_launch `--cars 4 --hold 25`,
  first-hit probe, stock original. The positive control `0x00546c50` fired on 11 of 13 tracks.
  Its only caller `0x00575fe0` runs on 9 of 13 (up to 2000/phase). Its call to
  `0x00566200` sits behind a shape-flag test (port `RwpSolverCore17.cpp:569`,
  `(*(byte *)(psVar1 + 0x20) & 4) == 0`) and never fired
  (`firsthit_sweep.txt`). [UNCERTAIN] which scenario, if any, reaches it.
- `record_race.py`'s own Arctic recipe reaches `0x005667c0` 1 to 3 times per run
  (record1/record2 logs). Added `record_race.py --attach-pid N --scenario "<recipe>"`: it
  records inside a race another tool spawned and drives (explicit PID, no kill), plus
  `verify/hb6_20261026/record_attach.py`, which runs scenario_launch then attaches.
  Neustein 4 cars: 45202 calls seen in 30 s (1503/s, above the 1000/s hot-path line. The
  game survived the window and scenario_launch exited 0), 1200 recorded.
- Registry: `replay_only` (kept out of the classic tools) and `bundle_of` (one recording,
  two port bodies). Entries `rwp_aabb_xform_x87`, `rwp_normalize3`, `rwp_normalize3_x87`.
- Pre-existing break fixed: the replay port image no longer linked (Math TUs reference
  `Math/RwLutTables.cpp`'s `RwLutSqrtTable`). The six RW-LUT entries now list that TU.

## 4. Vector results (replay_c3.py, `verdicts.txt`)

| body | input | calls / distinct | CW | verdict | port != original |
|---|---|---|---|---|---|
| FUN_005667c0 default (SSE2) | live Neustein | 1200 / 549 | 0x007f | RED | 1189 |
| FUN_005667c0_x87 | live Neustein | 1200 / 549 | 0x007f | HELD (coverage 1/2) | 0 |
| mutation: default body on x87 entry | live Neustein | 1200 | 0x007f | RED | 1189 |
| FUN_005667c0 default / _x87 | live Highway | 3000 / 410 | 0x007f | RED 2474 / HELD 0 | |
| FUN_00566200 default (clause-8 control) | synth bounded | 513 | 0x007f / 0x027f | RED 370 / 490 | unchanged, CSV byte-identical |
| FUN_00566200_x87 | synth bounded | 513 | 0x007f / 0x027f | HELD (synthetic) | 0 / 0 |
| FUN_005667c0 default | synth | 513 | 0x007f / 0x027f | RED 512 / HELD 0 | |
| FUN_005667c0_x87 | synth | 513 | 0x007f / 0x027f | HELD 0 / 0 | |

- `0x005667c0` x87 body: every clause leg is met except gate 5. The uncovered side is
  `0x005667f3:taken` (zero path, |v|^2 <= FLT_MIN at `0x005ceabc`). The smallest live
  |v|^2 is 2.13e-9 (Neustein) and 4.4e-3 (Highway). [UNCERTAIN] zero path not exercised live.
- `0x00566200` x87 body: synthetic only, so it cannot count toward gate 1.

## 5. Tracker

No change. Neither row meets `re/CONFIDENCE.md` Replay clause gate 5 or gate 1, and the
measured body is not the default body (clause 7: the default install runs the SSE2 body,
which is RED). Candidates for C3:
- `0x005667c0` via `rwp_normalize3_x87`, after the flip decision and either a live
  zero-vector call or a rubric ruling on 1-jcc leaves.
- `0x00566200` via `rwp_aabb_xform_x87`, after a scenario that reaches it, and the flip.
Clause 8 note: the control `0x00566200 --synth` still reads RED (default body). After a flip
it would need to become `--wrong-port` against the retired body.

## 6. Default unchanged

`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this branch's exe (6640ac65), private root:
G-CTRL `5943c5f1/b493262b/09551e6a`, B `3a03670c/e85ea6fb/53778664`. These equal the
`merge_train.json` refs. Both targets build. `.asi` not deployed. Knob ON was not run in game.

## Raw data (D:/MashedWT/HB-6_data, not committed)

| file | SHA-256 |
|---|---|
| bundle_t4_3000.json (Highway bundle) | c08312485553450513afa076af2437a8d7b6e7ac7a2725df30429ad3d796b0d7 |
| 005667c0_rwp_normalize3.t4.replay.csv | daab64b40ac2f694b2170cbf31b856848337164b89fec16447e7ae13205afeed |
| 005667c0_rwp_normalize3_x87.t4.replay.csv | fe5586f2fc1bbba7ba95601515b29a6645738322445afa0b960811ba3c380166 |
| rec_t2_scen.log / rec_t4_scen.log | 309de031... / 1c3f42aa... |
| record1.log / record2.log (Arctic recipe, 3 and 1 calls) | 8680d2cc... / 5b6ed9ef... |
| arms/ (H_DEF, B_DEF_K*), arms.log, fh2_t*.firsthit.tsv, synth/*.csv | see sha8 above |

Committed: `re/replay/veccap/005667c0_rwp_normalize3.json` (Neustein, 1200 calls),
`log/diff/005667c0_rwp_normalize3{,_x87}.replay.csv`, the negctl CSV, `verdicts.txt`,
`firsthit_sweep.txt`.
