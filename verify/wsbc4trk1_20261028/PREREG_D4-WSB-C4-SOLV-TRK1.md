# PREREG D4-WSB-C4-SOLV-TRK1

Committed alone, before any arm. Branch `orch/D4-WSB-C4-SOLV-TRK1` off `orch/integration` @ `da3604323`. No source
change on this branch.

Build: this branch's own (`cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` in the worktree). The shared
STACKONLY build (`779bc9c3` / `c6a264a3`) is **not** used: `git diff --stat 0d1285dc7 da3604323 -- mashedmod/` lists
20 files (D5 render / pickup / RfxEmitter / exe_main merges), so that build is not this branch's source. Hashes of the
built `mashed_re.exe` / `mashed_re_dev.asi` go in the RESULT. Private root `D:/MashedWT/D4-WSB-C4-SOLV-TRK1_root`
(robocopy of `D:/MashedWT/D4-WSB-STACKONLY-C3_root`, its `original/mashed_re_dev.asi` replaced by this branch's
`.asi`; the main checkout's `original/` is not touched). Raw data: `D:/MashedWT/D4-WSB-C4-SOLV-TRK1_data`.

## Rows (scope: `verify/wsbc4scope_20261027/RESULT_D4-WSB-C4-SCOPE.md`, group SOLV-TRK1)

All 8 are C3/impl, wrapper `ShadowAB::RunTracked` (`RwpSolverLeaves1.cpp:885/896/907`, `RwpSolverCore9.cpp:333`,
`RwpSolverCore11.cpp:937/948/959`, `RwpSolverIntegrate6.cpp:637`), out-memory leg B.

| row | census | SCOPE outside-frame stores |
|---|---|---|
| `0x005675d0` | EXEC | 16 ptr |
| `0x00567c00` | EXEC | 7 ptr |
| `0x00567c60` | EXEC | 25 ptr, 16 slot |
| `0x00567f00` | EXEC | 16 ptr, 4 slot |
| `0x005684c0` | EXEC | 26 ptr |
| `0x00568560` | EXEC | 21 ptr |
| `0x005685f0` | **NOT-EXEC** | 48 ptr, 1 str |
| `0x0056bdf0` | EXEC | 6 ptr |

`0x005685f0` is NOT-EXEC in the census (`verify/wsbcensus_20261026/wsb_census.tsv`). It runs through both legs like
the others, and it is reported NOT-EXEC / not observed whatever the legs print unless both legs observe it. It is
never promoted by this step on a leg that saw no call.

## Arms (one hook installed alone per boot, each row in its own solo window under `mashed_lock.py`)

Harness copies of RGN's (`dcc0f2bba`), data path / label / row list changed only: `drive_trk1.py` (copy of
`drive_rgn.py`), `run_shadow7.py` (copy of `run_shadow6.py`), `run_bvh7.py` (copy of `run_bvh6.py`). The lock wait is
outside the runner's timer (`drive_*` takes the solo window, the runner is admitted inside it).

1. **Leg B (8 rows), heap-tracked write-set shadow** (the SOLV-WS leg H recipe): census (`--track 0 --mode 10
   --cars 4`, `HOLD=60`), `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, `SAMPLES_DONE=24`,
   `SHADOW_TAG=heap`. Page counts per row by a copy of `verify/wsbsolvws_20261027/heap_pages.py` (data path changed).
   - Fallback **arm N**, only for a row whose leg B read-out is CLEAN-NO-PAGES (0 pages compared): the same recipe plus
     `MASHED_SHADOW_NEED_PAGES=1`, `HOLD=150`, `SHADOW_TAG=needpages` (SOLV-WS addendum 1 recipe).
2. **Racediff (8 rows)**: `verify/wsbsolvws_20261027/c4_racediff_v7.py` unchanged, via `run_bvh7.py`, Quick Battle
   window A4 300..1200, OFF `--hooks __none__` / ON the row alone. A row Quick Battle does not observe (0 calls both
   sides) gets one census run (`C4_SCENARIO=census`, `C4_HOLD=400`, `C4_WINDOW="1 3000"`).

Audio rule before every launch: `drive_trk1.py` runs `Get-PnpDevice -Class AudioEndpoint -PresentOnly` inside the
window before the runner and after it, and reads the Application log's MASHED.exe Event 1000 records. No endpoint, or a
fault at `0x001baf48` (`0x005baf48`): stop, BLOCKED audio endpoint, no retry. U-9677 offsets (`0x001509b4`,
`0x000e66db`): re-run the row (max 2 re-runs), not a finding. Only PIDs this step spawned are killed.

## Read-out per row (fixed now)

- **Leg B PASS (HEAP-CLEAN)**: n >= 1 counted samples (target 24), ndiff 0 on every sample, every counted sample
  `stack=0 noise=0 ovf=0 ret=0`, `PATCHBYTE ... installed=e9 ... A/B-IS-REAL`, no NOISY / UNTRACKED / OTHER-EXC line for
  the row, and heap pages compared > 0 (sum over samples, leg B or arm N).
  - CLEAN-NO-PAGES after arm N, or CLEAN-IMAGE-ONLY: not PASS, row stays C3.
  - DIVERGENT (any counted sample ndiff > 0): not PASS, row stays C3, values reported.
  - NO_LOG / 0 counted samples: not observed, row stays C3.
- **Racediff PASS**: `C4-CLEAN` (installed `0xE9`, same signature set both sides). HOLD with 0/0 calls = not observed.
  A HOLD whose only difference is a pointer label that moves with heap placement (SOLV-WS section 3 LAYOUT) is
  reported as such and the row stays C3. It is not re-labelled clean.
- **C3 -> C4** through re-classify (Edit tool on the target rows only) when leg B and the racediff both PASS.
  Otherwise the row stays C3 with a note.

Default gates on this branch's exe: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`
(`A4_EXE` = this branch's `mashed_re.exe`), under the lock and the test slot. Must match `re/orchestrator/merge_train.json`
(`gctrl.ref` / `b.ref`; at PREREG time G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/9fcdbd3a/53778664`). A gate
mismatch makes the step RED (no source change on the branch, so it would be a build or harness fault, reported).
