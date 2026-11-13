#!/bin/bash
# PORT-TOP-08 checks, run inside ONE mashed_lock hold (test slot outside):
#   test-slot bash -c 'py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --wait 14400 --label PORT-TOP-08 -- bash verify/porttop08_20261112/run_checks.sh'
# Every arm is MASHED_ORIG_TOP=1. "store" = this row (slot types stored at 0x007e96fc); "nostore" = MASHED_TOP08_NO_SLOTSTORE=1
# (the state before this row). Raw data in D:/MashedWT/PORT-TOP-08_data.
R=D:/MashedWT/PORT-TOP-08_root
D=D:/MashedWT/PORT-TOP-08_data
export PORTTOP_OUT=$D PYTHONIOENCODING=utf-8
QB='MASHED_NAV_KEYS=e,d,e,e,e,e,x*1200'
n=0; N=14
step() { n=$((n+1)); echo "[$n/$N] $1"; }
# 1) knob boot, 600 frames, with the provisional-body self-test (0x0043dfd0, now with 0x0043c000 bound)
step boot_store; py -3.12 verify/porttop_20261108/knob_boot.py boot_store $R 200 MASHED_ORIG_TOP=1 MASHED_PROV_43DFD0_SELFTEST=1 MASHED_DET_FRAMES=600
# 2-5) Quick Battle route with the phase word driven by the hand launch (MASHED_PORTSTATE_GAMEFLOW=1): nostore / store x2
for a in 1 2; do
  step qbgf_nostore$a; py -3.12 verify/porttop_20261108/knob_boot.py qbgf_nostore$a $R 300 MASHED_ORIG_TOP=1 MASHED_DBG_MENU=1 "$QB" MASHED_NAV_KEYS_GAP=60 MASHED_PORTSTATE_GAMEFLOW=1 MASHED_TOP08_NO_SLOTSTORE=1
  step qbgf_store$a;   py -3.12 verify/porttop_20261108/knob_boot.py qbgf_store$a   $R 300 MASHED_ORIG_TOP=1 MASHED_DBG_MENU=1 "$QB" MASHED_NAV_KEYS_GAP=60 MASHED_PORTSTATE_GAMEFLOW=1
done
# 6-7) the same route without the phase mirror (PORT-TOP-05 qb_mat: phase 1 throughout, sha8 479d0282): nostore / store
step qb_nostore; py -3.12 verify/porttop_20261108/knob_boot.py qb_nostore $R 300 MASHED_ORIG_TOP=1 MASHED_DBG_MENU=1 "$QB" MASHED_NAV_KEYS_GAP=60 MASHED_TOP08_NO_SLOTSTORE=1
step qb_store;   py -3.12 verify/porttop_20261108/knob_boot.py qb_store   $R 300 MASHED_ORIG_TOP=1 MASHED_DBG_MENU=1 "$QB" MASHED_NAV_KEYS_GAP=60
# 8-9) the row's self-test: hardware write-watch on DAT_0067eab0 on the phase-driven route, nostore (expect 0 -> 1 -> 2 at
#      the first case-3 frame) and store (expect no write)
step watch_nostore; MASHED_TOP08_NO_SLOTSTORE=1 py -3.12 verify/porttop08_20261112/watch_eab0.py watch_nostore $R 300 MASHED_ORIG_TOP=1 MASHED_DBG_MENU=1 "$QB" MASHED_NAV_KEYS_GAP=60 MASHED_PORTSTATE_GAMEFLOW=1 MASHED_TOP08_NO_SLOTSTORE=1
step watch_store;   py -3.12 verify/porttop08_20261112/watch_eab0.py watch_store $R 300 MASHED_ORIG_TOP=1 MASHED_DBG_MENU=1 "$QB" MASHED_NAV_KEYS_GAP=60 MASHED_PORTSTATE_GAMEFLOW=1
# 10-13) menu walk (12 nav-demo captures, MASHED_DET_MPG_PIN=0 via knob_boot.py): nostore / store / nostore / store
export CAPFLAKE_DATA=$D CAPFLAKE_ROOT=$R
for a in 1 2; do
  step walk_nostore$a; MASHED_TOP08_NO_SLOTSTORE=1 py -3.12 verify/porttop_20261108/capflake_runs.py walk_nostore$a ON@dflt
  step walk_store$a;   env -u MASHED_TOP08_NO_SLOTSTORE py -3.12 verify/porttop_20261108/capflake_runs.py walk_store$a ON@dflt
done
step summary; py -3.12 verify/porttop08_20261112/summarize.py $D/boot_store.log $D/qbgf_nostore1.log $D/qbgf_store1.log $D/qbgf_nostore2.log $D/qbgf_store2.log $D/qb_nostore.log $D/qb_store.log $D/watch_nostore.log $D/watch_store.log
