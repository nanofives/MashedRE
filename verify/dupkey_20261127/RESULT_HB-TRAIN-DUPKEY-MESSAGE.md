# RESULT HB-TRAIN-DUPKEY-MESSAGE (meta tool row, text kind plus probe branch, no build, no gates, binds no RVA)

Met: all three deliverables. Branch orch/HB-TRAIN-DUPKEY-MESSAGE, commit fddff1abf on c880ab12e.

1. Enqueue refusal. `roadmap.ps1 enqueue` runs `tracker_merge3.py check-ref <repo> <branch> merge_train.json` before the lock and throws on exit 3. It checks every conflict_keyed file (hooks.csv, re/match/PROVISIONAL.tsv).
   Probe branch probe/HB-TRAIN-DUPKEY-MESSAGE-dup (31c8437e8, 0x004ccef0 added twice more to PROVISIONAL.tsv, the run 56 case, log/orch/run56_drain2.log:5):
   `REFUSED: enqueue refused: DUPLICATE KEY 0x004ccef0 in re/match/PROVISIONAL.tsv on side branch probe/HB-TRAIN-DUPKEY-MESSAGE-dup`. Ledger row stayed `claimed` (nothing enqueued).
2. Train message. tracker_merge3.py now parses all three inputs first and exits 3 with `DUPLICATE KEY <k> on side ours|theirs ...` on stdout (it used to print to stderr, exit 2, so the train showed `[]`). merge_train.ps1 captures 2>&1 and reports any exit other than 1 as "tracker_merge3 duplicate key or unreadable input (exit N, not a same-key conflict): <file> [<output>]". Not run end to end on a real duplicate merge: checked by the two new selftest cases and the unchanged same-key case.
3. scripts/diag.py:439 docstring is `r"""`. py_compile with -W error passes.

Checks run: tracker_merge3_selftest.py 13/13 PASS (2 new cases, merge_train.ps1 selftest line now expects 13/13); `merge_train.ps1 -SelfTest` 0 failures (log in %TEMP%\dupkey_selftest.log); mashed_lock.py status: free. No build, no gates, no frida folders touched (none run).
Note: the probe branch is left in place, not merged. Unfinished scope: none.
Slip: one stray write to the main checkout's scripts/diag.py (wrong cwd); reverted by hand, main checkout clean.
