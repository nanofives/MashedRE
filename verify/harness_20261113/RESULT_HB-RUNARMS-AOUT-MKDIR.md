# RESULT HB-RUNARMS-AOUT-MKDIR

Change: `verify/savelink_20261023/run_arms.py` line 21 (right after `OUT = ...` at line 20) adds
`os.makedirs(OUT, exist_ok=True)`, so A4_OUT exists before any write, also for `A4_PLAN=1`.
Docstring line 14 now says A4_OUT is created at startup. Python harness only: no exe change, no build, no game launch.

Check (A4_OUT = D:/MashedWT/HB-RUNARMS-AOUT-MKDIR_data/fresh_out, did not exist before):
- `py.exe -3.12 -m py_compile verify/savelink_20261023/run_arms.py`: exit 0
- `A4_PLAN=1 py.exe -3.12 verify/savelink_20261023/run_arms.py H_DEF`: exit 0, dir exists afterwards (before: False, after: True)
- Pre-existing SyntaxWarning `invalid escape sequence '\@'` at lines 390-401 (not touched by this row).

Gates: not run, harness only (G-CTRL / B hashes cannot see this file).
