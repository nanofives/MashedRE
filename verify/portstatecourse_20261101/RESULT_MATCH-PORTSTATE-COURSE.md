# RESULT MATCH-PORTSTATE-COURSE (2026-11-01)

Verdict: **GREEN.** With `MASHED_PORTSTATE_COURSE` (default OFF, exe only) the exe holds the original's course
table: records 0x005f2570, pointers 0x005f2728, hook table 0x005f33f8, count 0x005f37a0, and `0x0063d7e4`
selected by the matched 0x0041e870. All four PREREG predictions hold. No gate hash moved, knob OFF or ON, so
there is no decided-flip decision. The re-check of T04 / T05 / T13 clears the course-table read in every row
that named it. One row now has an exe call (0x0041e870, knob ON only). The other rows still have a second blocker,
listed below.

PREREG: `verify/portstatecourse_20261101/PREREG_MATCH-PORTSTATE-COURSE.md`, committed alone at `2a949b773`
before any arm ran. L-78 (c) (Mariano 2026-11-01), port-state migration 1/3.

## Change (`099f60d3d`)

- `mashedmod/src/mashed_re/Track/CourseTableState.{h,cpp}` (new, exe only). `CourseTableState_OnCourseLoad`
  returns at once with the knob OFF. With the knob ON it copies the four ranges from a source table that
  `gen.py` (scratch) transcribed from `original/MASHED.exe.unpatched`. It then calls `TrackNodeRecordScan`
  (0x0041e870, original call site 0x00426eaf) with the `COURSE.LUA` `Course_Id`, and logs one
  `[portstate-course]` witness line. The slot dwords stay the original `.text` addresses, and nothing calls
  through them.
- `D3d9Render/TrackRenderer.cpp`: one call right after the `Ai_BridgeLoad` block (which seeds `0x005f2770`).
- `mashedmod/exe_sources.rsp`: `Track\CourseTableState.cpp`, `Match\MatchS04_0041e870.cpp` (the matched
  selector was asi-only; now one body in both targets).
- `verify/savelink_20261023/run_arms.py`: `STACK['PSC'] = ['MASHED_PORTSTATE_COURSE']`.
- `verify/portstatecourse_20261101/fnv_ranges.py`: P2 witness (file-side FNV-1a, log comparison).

Build: `MASHED_NO_DEPLOY=1`, `=== Build OK ===` both targets, `[rva-lint] 3 finding(s) ... allowlisted=3 NEW=0`,
`[asm-lint] OK`.

## Predictions

| id | prediction | measured | holds |
|---|---|---|---|
| P1 | knob OFF: G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3 | H_DEF 8284be46 / ba407a16 / 31fe043e, B_DEF_K0/K1/K26 1d5bf4f9 x3 | yes |
| P2 | knob ON: logged FNV-1a = file FNV-1a of the same ranges | file recs=bbba0607 ptrs=b1872d64 hook=6064cc66 count=3c1ab248; all four `_PSC` logs equal on all four (`fnv_check.txt`, rc 0) | yes |
| P3 | knob ON, Training (`Course_Id` 30): `0x0063d7e4` = 0x005f33f8 | `sel=005f33f8 name=training` in all four `_PSC` logs | yes |
| P4 | knob ON: same hashes as P1 | H_PSC 8284be46 / ba407a16 / 31fe043e, B_PSC_K0/K1/K26 1d5bf4f9 x3 | yes |

The same file hashes come from `original/MASHED.exe` (patched). No boot patch touches these ranges.

Gate runner: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSC B_PSC_K0 B_PSC_K1
B_PSC_K26`, `A4_ROOT=D:/MashedWT/MATCH-PORTSTATE-COURSE_root`, exe copied out first, wrapped in `test-slot` +
`mashed_lock.py run --label MATCH-PORTSTATE-COURSE`. Exit 0 for all eight arms. H arms 13962 step lines each, B
arms 762.

## Re-check of the blocked rows (knob ON)

Witness line (H_PSC; the three B_PSC lines are byte-identical, `witness_H_PSC.txt`):
`course=30 ... sel=005f33f8 name=training slot14=00451280 slot44=00000000 tab=005f2728 e9fc=0 v=0 ce80=6
ba78=0 rec78=005f2570 rec78d0=30`.

| row | rva | course-table read, knob ON | remaining blocker | outcome |
|---|---|---|---|---|
| T05 | 0x0041e870 | the scan selects the original's record (P3) | none for the knob-ON exe path | **exe caller added** (`CourseTableState_OnCourseLoad`), runs only with the knob ON. The body does not execute in the default build, so it is not a C4 candidate yet (strict C4 needs the body to execute, 0x0040b9a0 / 0x0040ba60 precedent 2026-10-16) |
| T05 | 0x0041e9d0 / 0x0041ea90 | `[0x0063d7e4]+0x14` = 0x00451280, `+0x44` = 0 (the training record's slot 0 / slot 12, as in the file) | no exe call site; the original callers are in 0x00426e10's dispatch, which has only an asi body (S06) | left |
| T05 | 0x0041e8b0 / 0x0041e970 | the jump target is now the original's slot dword | the target is a `.text` address in the original. In the exe it is callable only where a B16 thunk exists. No exe call site | left |
| T05 | 0x004053d0, 0x004095a0, 0x00409680, 0x00409710, 0x00412050, 0x004260e0, 0x004715a0 | not a course-table read | unchanged from `RESULT_MATCH-TOPDOWN-T05.md` (clip module, LED table, scenario tables, the unmatched 441 B connector 0x0040d270) | left |
| T13 | 0x0040ce80 | `*(*(int**)0x005f2770 + v) + 4` reads 6, the value the exe's constant `HandlingTypeIndex()` already uses on this scenario | the index. At course load the exe has `[0x0067e9fc]` = 0 (the original has 10 there, `Race/SubStateMachine.cpp:32`), so the matched selector would take the `[0x0067f17c]` arm, not `[0x0067ea94]`. Both read 0 on Training, which is why the result agrees. The exe does not produce either index the original's way (`0x0067f17c` is a frontend frame / row counter in `Frontend/FrontendDispatch.cpp:86`, `SpriteDispatch.cpp:97`), so on any other track the swap would read the Training row. Whether `VehicleInit` 0x0046b540 runs after this course-load hook in the exe was not measured | left; index producer = GameFlow migration (3/3) |
| T04 | 0x00410b30 | `*PTR_PTR_005f2770[DAT_0063ba78]` = 30 (`rec78` 0x005f2570, dword 0); `[0x0063ba78]` = 0 in the exe, and no exe TU writes it | RW device slot +0x20 (Im2D bridge), `DAT_0063ba98` / `0x007f0f30` / `0x007f0f3c` / `0x0063b920`, about 30 render callees on port state (`RESULT_MATCH-TOPDOWN-T04.md`) | left |
| T04 | the other 15 rows | not a course-table read | unchanged | left |
| T13 | the other 11 left rows | not a course-table read (car record block 0x00881e80 / 0x0063d9e0 / 0x008995bc, held word 0x0088fc88, 0x0063c8d0) | record-block migration (2/3) | left |

Summary: the course-table reads in all three batches are live with the knob ON. The next blockers are the
per-subsystem state that L-78 already orders: the record block (2/3) for T13, GameFlow / frontend indices (3/3) for
0x0040ce80, and the RW device plus render tables for T04.

Scope limits: P2 to P4 were measured on Training only. On other tracks the selector input is the `COURSE.LUA`
`Course_Id` (`course_id_`), the same value the existing hook-slot model keys on (`Render/TrackHookSlots.cpp`
`HookTrackFor`), but no non-Training arm was run. The default build's code layout changed (one call plus a
new TU). The memory note on layout-sensitive trajectories names Rouabout / Storm / Highway, not Training, so
the train's `-Full` pass (13 tracks) covers that. This branch touches `D3d9Render/` and `Track/`, not
`full_set_paths`.

## Re-classify

None. 0x0041e870 gains an exe caller that runs only with the knob ON. No row changes level.

## Decision

None filed. No hash moved with the knob ON. A flip would be inert today, because no default-build exe code
reads the seeded ranges or `0x0063d7e4`. It becomes worth proposing once a swap that reads them lands.

## Raw data (D:/MashedWT/MATCH-PORTSTATE-COURSE_data, not committed)

```
a7f4913f8130d8bdbee8bf99777632c9eadb66882442effda093e7082c799ef4  exe_g1/mashed_re.exe (this branch, 099f60d3d)
edca8ccb431fb491dd82330655a9e83c5c8f3f6b8f03e0237ebbcb880904d74a  build1.log
8ca62706b33dcc39b8d88958c1e3085ab91850c6a5eda864e74cad9678fb4d0b  gates_g1.log
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv  (= H_PSC.step.csv)
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv (= H_PSC.gates.csv)
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log (= H_PSC.ptrace.log)
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_{DEF,PSC}_K{0,1,26}.step.csv (all six)
e23e50c4606cd9467cb23d9530e7156c84adf45b9aa9beeca4dc08e19a0dee92  arms_g1/H_DEF.log
971045f4f578932c45397cc9db7c98545ce0a50022974aaaea36e75b710463fe  arms_g1/H_PSC.log
a0dc0608d04c869ccbc1457398d0c46e7f49604692c7f5581cc2531aacb5a71d  arms_g1/B_PSC_K0.log
417dd67f8d22aa1dd5809272bd8440ec21d0fc3cc62a23e5121c2f00af9a076b  arms_g1/B_PSC_K1.log
26340890f290e96bf1819dd8c855e9c78344b692cd6c82bed1c8131d015d0b97  arms_g1/B_PSC_K26.log
```

Committed extracts: `fnv_check.txt`, `witness_H_PSC.txt`.
