merge_train.ps1 -SelfTest: 156 PASS, 0 failures (8 new checks, was 148); head f10b8e69c binder selftest PASS 0 (every profile)

# RESULT HB-TRAIN-BINDER-SELFTEST (run 56, tool row, no game arms)

Change (re/orchestrator/merge_train.ps1, MERGE_TRAIN.md only; merge_train.json unchanged):
- Test-BinderStep runs porttop_bind.py selftest in Try-Batch right after Apply-Promotions (c4_reclass --apply --strict), before archive lint, build and gates; only for a batch that applied promotions.
- Pass fails on a NEW failure line against the base head's list. Base list is taken from a temp worktree at the base commit, run only when the merged tree has failures. A selftest with no closing PASS/FAIL N line fails the pass.
- Log lines documented in MERGE_TRAIN.md "Binder selftest step".

Checks run:
- merge_train.ps1 -SelfTest: 156 PASS lines, 0 failures (log verify/trainbinder_20261127/mtrain_selftest.txt). New checks: (b) unchanged PASS 0 passes, failures already in base pass, (a) NEW failure fails the pass, the run 55 shape (base PASS 0, after FAIL 1) fails, no closing line fails, placement after Apply-Promotions and before archive lint.
- Head selftest on f10b8e69c (test-slot wrapped, mashed_lock free): PASS 0 (verify/trainbinder_20261127/head_selftest_f10b8e69c.txt).
- Not run: real merge train, G-CTRL/B (tool row). G4 still needs a real drain log line (L0 cites it).
- Decoy: failure match is exact line text, so a failing line whose numbers move counts as NEW (conservative).
- frida-<hash> folders in %LOCALAPPDATA%\Temp now: 4 (not counted before; this run started none).
