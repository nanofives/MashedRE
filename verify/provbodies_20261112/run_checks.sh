#!/bin/bash
# PROVISIONAL-BODIES checks, run inside ONE mashed_lock hold:
#   py -3.12 re/orchestrator/mashed_lock.py run --wait 14400 --label PROVISIONAL-BODIES -- bash verify/provbodies_20261112/run_checks.sh
R=D:/MashedWT/PROVISIONAL-BODIES_root
D=D:/MashedWT/PROVISIONAL-BODIES_data
export PORTTOP_OUT=$D PYTHONIOENCODING=utf-8
# 1) the knob self-test boot (default exe, knob path on, self-test on)
py -3.12 verify/porttop_20261108/knob_boot.py selftest_on $R 200 MASHED_ORIG_TOP=1 MASHED_PROV_43DFD0_SELFTEST=1 MASHED_DET_FRAMES=600
# 2) the same boot without it (control)
py -3.12 verify/porttop_20261108/knob_boot.py selftest_off $R 200 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600
# 3) menu walk (12 nav-demo captures): knob path, self-test OFF / ON / OFF / ON
export CAPFLAKE_DATA=$D CAPFLAKE_ROOT=$R
for a in 1 2; do
  env -u MASHED_PROV_43DFD0_SELFTEST py -3.12 verify/porttop_20261108/capflake_runs.py walk_off$a ON@dflt
  MASHED_PROV_43DFD0_SELFTEST=1 py -3.12 verify/porttop_20261108/capflake_runs.py walk_on$a ON@dflt
done
