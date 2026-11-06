# RESULT HB-SELFTEST-D2DEFH

Verdict: GREEN.

Problem: `merge_train.ps1 -SelfTest` HB-D2RERUN block hardcoded `D2_DEF_1`/`D2_DEF_2`, but `merge_train.json` `full.d2_arms` is now `D2_DEFH_1..3` (42 s hold). The fake runner never matched, so 3 checks failed on base.

Fix (re/orchestrator/merge_train.ps1, HB-D2RERUN block only): `$A1`/`$A2` are read from `$f.d2_arms[0]`/`[1]`, and the three checks and the fake runner use them. `merge_train.json` untouched.

Evidence: `pwsh -NoProfile -File re\orchestrator\merge_train.ps1 -SelfTest` after the fix prints `SELFTEST done: 0 failure(s)`, including the three D2 rerun checks (short-once, short-always, full-length) with D2_DEFH_2. The pre-fix failure was taken from the lead's ticket (found in verify/harness_20261102), not re-run here.
