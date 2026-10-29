# PREREG D4-WSB-C4-SOLV-TRK2

Committed alone, before any arm. Branch `orch/D4-WSB-C4-SOLV-TRK2` off `orch/integration` @ `34235b24c`. No source
change on this branch.

Build: this branch's own (`cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` in the worktree). Hashes of the
built `mashed_re.exe` / `mashed_re_dev.asi` go in the RESULT. Private root `D:/MashedWT/D4-WSB-C4-SOLV-TRK2_root`
(robocopy of `D:/MashedWT/D4-WSB-C4-SOLV-TRK1_root`, its `original/mashed_re_dev.asi` replaced by this branch's
`.asi`; the main checkout's `original/` is not touched). Raw data: `D:/MashedWT/D4-WSB-C4-SOLV-TRK2_data`.

C4 rule: `re/CONFIDENCE.md` "Write-set leg for C4" (L-38 / L-40 / L-54). All 12 rows store outside their own frame
(SCOPE), so none takes the L-54 static-empty route. Each needs a heap-tracked shadow leg that is CLEAN with more than
0 heap pages compared, plus a clean race diff.

## Rows (scope: `verify/wsbc4scope_20261027/RESULT_D4-WSB-C4-SCOPE.md`, group SOLV-TRK2)

All 12 are C3/impl, wrapper `ShadowAB::RunTracked`, census EXEC (SCOPE lines 111-124), out-memory leg B.

| row | wrapper | SCOPE outside-frame stores |
|---|---|---|
| `0x0056c0a0` | `RwpSolverIntegrate6.cpp:659` | 19 ptr |
| `0x0056c310` | `RwpSolverIntegrate6.cpp:670` | 19 ptr, 1 slot |
| `0x0056c580` | `RwpSolverCore7.cpp:625` | 12 ptr, 21 slot, 1 str |
| `0x0056c8e0` | `RwpSolverCore7.cpp:636` | 6 ptr |
| `0x0056caa0` | `RwpSolverCore7.cpp:647` | 67 ptr, 4 str |
| `0x0056d070` | `RwpSolverCore7.cpp:668` | 38 ptr, 6 slot |
| `0x0056dd40` | `RwpSolverCore8.cpp:756` | 16 ptr |
| `0x0056e680` | `RwpSolverCore8.cpp:777` | 32 ptr, 12 slot |
| `0x0056ef30` | `RwpSolverCore9.cpp:268` | 26 ptr |
| `0x0056efc0` | `RwpSolverCore9.cpp:279` | 4 ptr |
| `0x0056f020` | `RwpSolverCore9.cpp:290` | 7 ptr |
| `0x0056f1f0` | `RwpSolverCore9.cpp:312` | 27 ptr, 1 slot |

## Boot check (first leg)

One plain launch of the original from the private root under `mashed_lock.py`, `MASHED_HOOK_ONLY=__none__`, endpoint
checked right before launch. PASS = process alive at 25 s and no AV at `0x005baf48`. Otherwise BLOCKED audio
endpoint, stop, no retry.

## Arms (one hook installed alone per boot, each row in its own solo window under `mashed_lock.py`)

Harness: copies of SOLV-TRK1's (`971940ec1`, on `orch/D4-WSB-C4-SOLV-TRK1`), data path / label / runner names
changed only: `drive_trk2.py`, `run_shadow8.py`, `run_bvh8.py`, `heap_pages.py`.

1. **Leg B (12 rows), heap-tracked write-set shadow**: census (`--track 0 --mode 10 --cars 4`, `HOLD=60`),
   `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, `SAMPLES_DONE=24`, `SHADOW_TAG=heap`. Page counts
   per row by `heap_pages.py`.
   - Fallback **arm N**, only for a row whose leg B read-out is CLEAN-NO-PAGES (0 pages compared): the same recipe plus
     `MASHED_SHADOW_NEED_PAGES=1`, `HOLD=150`, `SHADOW_TAG=needpages`.
2. **Racediff (12 rows)**: `verify/wsbsolvws_20261027/c4_racediff_v7.py` unchanged, via `run_bvh8.py`, Quick Battle
   window A4 300..1200, OFF `--hooks __none__` / ON the row alone. A row Quick Battle does not observe (0 calls both
   sides) gets one census run (`C4_SCENARIO=census`, `C4_HOLD=400`, `C4_WINDOW="1 3000"`).

Audio rule before every launch: `drive_trk2.py` runs `Get-PnpDevice -Class AudioEndpoint -PresentOnly` inside the
window before the runner and after it, and reads the Application log's MASHED.exe Event 1000 records. No endpoint, or a
fault at `0x001baf48` (`0x005baf48`): stop, BLOCKED audio endpoint, no retry. U-9677 offsets (`0x001509b4`,
`0x000e66db`): re-run the row (max 2 re-runs), not a finding. Only PIDs this step spawned are killed.

## Read-out per row (fixed now)

- **Leg B PASS (HEAP-CLEAN)**: n >= 1 counted samples (target 24), ndiff 0 on every sample, every counted sample
  `stack=0 noise=0 ovf=0 ret=0`, `PATCHBYTE ... installed=e9 ... A/B-IS-REAL`, no NOISY / UNTRACKED / OTHER-EXC line for
  the row, and heap pages compared > 0 (sum over samples, leg B or arm N).
  - CLEAN-NO-PAGES after arm N, or CLEAN-IMAGE-ONLY: not PASS, row stays C3 (L-40).
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
