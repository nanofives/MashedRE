# HANDOFF MATCH-FIX-45CA30 (2026-11-06 19:55, paused for the LIVING-PC token-leak reboot)

Worktree `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-FIX-45CA30`, branch `orch/MATCH-FIX-45CA30`,
cut from orch/integration `ac2e9ed63`. Ledger row: `working`.

## Done
- Commit `6205e02ef`: the .asi body of 0x0045ca30 moved from `Util/PromoLoop_sessionB.cpp` (old body: 6 floats) to
  `Match/MatchS10_0045ca30.cpp`, which compiles `re/match/s10/0045ca30.c` (the S10 byte-identical source, 36 floats
  0x0088f5e0..0x0088f670). `asi_sources.rsp` has the new TU. It is not in `exe_sources.rsp`, the same as the old body.
- The exe has no body and no caller of 0x0045ca30 (the caller is FUN_0045d250, which the port does not have).
  `exe_sources.rsp` is unchanged. A dependency scan of the `/sourceDependencies` json shows 0 of 630 exe objects
  depend on any changed file, and the asi scan catches exactly the 2 expected TUs. Script:
  `py -3.12 verify/matchfix45ca30_20261106/exedeps.py <wt> ac2e9ed63`.
- `re/frida/hooks_registry.py` `float_clamp3_45ca30`: seeds and observes all 36 floats (3 sets, with the -2 and -4
  boundaries, ulp neighbours, +-0, +-inf, NaN). The old vector observed 6, which is why the 6-float body passed.
- Build OK (`MASHED_NO_DEPLOY=1`). exe sha256 `5d41fe7d...`, asi `c5a3735b...`. Build log
  `D:/MashedWT/MATCH-FIX-45CA30_build.log`.
- Private root made: `D:/MashedWT/MATCH-FIX-45CA30_root` (robocopy of original\).

## Found
- `run_diff.py` refuses this arg_type (FATAL: `near_leaf_seed_multi_obs` has no handler in `diff_template.js`). The
  handler lives in `re/frida/early_window_leaf_diff.py` (pure-leaf lane). That lane's default asi is the SHARED
  `original\mashed_re_dev.asi`, so `MASHED_EW_ASI` must point at the worktree build.

## Next (nothing has run yet: both were only queued on the machine lock, stopped by PID before the reboot)
1. Early-window diff, new body, then the old body as a negative control (pinned copy of the Oct 24 deployed asi
   `D:/MashedWT/MATCH-FIX-45CA30_data/deployed_old.asi`, sha256 906f8601...; expect RED on elements 6..35 and NaN):
   ```
   test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label MATCH-FIX-45CA30-ew --wait 14000 -- bash -c "MASHED_EW_ASI=<wt>/mashedmod/build/mashed_re_dev.asi py -3.12 re/frida/early_window_leaf_diff.py float_clamp3_45ca30; MASHED_EW_ASI=D:/MashedWT/MATCH-FIX-45CA30_data/deployed_old.asi py -3.12 re/frida/early_window_leaf_diff.py float_clamp3_45ca30"' 2>&1 | tee D:/MashedWT/MATCH-FIX-45CA30_data/ew.log
   ```
2. Gates (expected unchanged, exe inputs identical): `A4_OUT=D:/MashedWT/MATCH-FIX-45CA30_data/arms
   A4_ROOT=D:/MashedWT/MATCH-FIX-45CA30_root`, `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` in the same
   test-slot + mashed_lock wrapping (`--label MATCH-FIX-45CA30-gates --wait 14000`). Sha8 of H_DEF.step.csv /
   .gates.csv / .ptrace.log and B_*.step.csv against `merge_train.json` refs (now 8284be46/67931834/31fe043e, B 1d5bf4f9 x3).
3. hooks.csv row 0x0045ca30 via re-classify: file column -> `Match/MatchS10_0045ca30.cpp`, evidence -> the new
   diff, level stays C3 (asi-only body, no exe caller, so no C4 under L-68).
4. RESULT `verify/matchfix45ca30_20261106/RESULT_MATCH-FIX-45CA30.md`, ledger `set ... review`, REPORT.
   Expected verdict GREEN (exe unchanged), unless the gates move.
