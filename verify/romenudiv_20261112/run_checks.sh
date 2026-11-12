#!/bin/bash
# RO-MENU-DIV-43DFD0 knob checks of the default (VS2022) exe, the same set as PROVISIONAL-BODIES, run in ONE lock hold:
#   test-slot py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --wait 14400 --label RO-MENU-DIV-43DFD0 -- bash verify/romenudiv_20261112/run_checks.sh
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-MENU-DIV-43DFD0
R=D:/MashedWT/RO-MENU-DIV-43DFD0_root
D=D:/MashedWT/RO-MENU-DIV-43DFD0_data/checks
mkdir -p $D
export PORTTOP_OUT=$D PYTHONIOENCODING=utf-8
# 1) knob self-test boot (knob path on, provisional 0x0043dfd0 self-test on) and its control
py -3.12 verify/porttop_20261108/knob_boot.py selftest_on $R 200 MASHED_ORIG_TOP=1 MASHED_PROV_43DFD0_SELFTEST=1 MASHED_DET_FRAMES=600
py -3.12 verify/porttop_20261108/knob_boot.py selftest_off $R 200 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600
# 2) menu walk (12 nav-demo captures): knob path, self-test OFF / ON / OFF / ON
export CAPFLAKE_DATA=$D CAPFLAKE_ROOT=$R
for a in 1 2; do
  env -u MASHED_PROV_43DFD0_SELFTEST py -3.12 verify/porttop_20261108/capflake_runs.py walk_off$a ON@dflt
  MASHED_PROV_43DFD0_SELFTEST=1 py -3.12 verify/porttop_20261108/capflake_runs.py walk_on$a ON@dflt
done
echo CHECKS_DONE
