#!/bin/bash
# PORT-TOP-05 checks, run inside ONE mashed_lock hold (test slot outside):
#   test-slot bash -c 'py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --wait 14400 --label PORT-TOP-05 -- bash verify/porttop05_20261112/run_checks.sh'
# Arms: "mat" = MASHED_ORIG_TOP=1 with the matched 0x004929d0 / 0x00492d30 (this row); "hand" = the same with
# MASHED_TOP05_HANDFRAME=1 (the stand-ins before this row). Raw data stays in D:/MashedWT/PORT-TOP-05_data.
R=D:/MashedWT/PORT-TOP-05_root
D=D:/MashedWT/PORT-TOP-05_data
export PORTTOP_OUT=$D PYTHONIOENCODING=utf-8
QB='MASHED_NAV_KEYS=e,d,e,e,e,e,x*1200'
n=0; N=11
step() { n=$((n+1)); echo "[$n/$N] $1"; }
# 1-2) knob boot, 600 frames, with the existing knob self-test (0x0043dfd0 provisional body, MASHED_PROV_43DFD0_SELFTEST=1)
step boot_mat;  py -3.12 verify/porttop_20261108/knob_boot.py boot_mat  $R 200 MASHED_ORIG_TOP=1 MASHED_PROV_43DFD0_SELFTEST=1 MASHED_DET_FRAMES=600
step boot_hand; py -3.12 verify/porttop_20261108/knob_boot.py boot_hand $R 200 MASHED_ORIG_TOP=1 MASHED_PROV_43DFD0_SELFTEST=1 MASHED_DET_FRAMES=600 MASHED_TOP05_HANDFRAME=1
# 3-6) knob path menu -> Quick Battle -> race start (PORT-STATE-02 P7 route), hand / mat / hand / mat
for a in 1 2; do
  step qb_hand$a; py -3.12 verify/porttop_20261108/knob_boot.py qb_hand$a $R 300 MASHED_ORIG_TOP=1 MASHED_DBG_MENU=1 "$QB" MASHED_NAV_KEYS_GAP=60 MASHED_TOP05_HANDFRAME=1
  step qb_mat$a;  py -3.12 verify/porttop_20261108/knob_boot.py qb_mat$a  $R 300 MASHED_ORIG_TOP=1 MASHED_DBG_MENU=1 "$QB" MASHED_NAV_KEYS_GAP=60
done
# 7) the same route with the phase word mirrored by the hand session (MASHED_PORTSTATE_GAMEFLOW=1): the matched machine
#    leaves phase 1 at the launch, case 2 runs 0x0040d440
step qb_mat_gf; py -3.12 verify/porttop_20261108/knob_boot.py qb_mat_gf $R 300 MASHED_ORIG_TOP=1 MASHED_DBG_MENU=1 "$QB" MASHED_NAV_KEYS_GAP=60 MASHED_PORTSTATE_GAMEFLOW=1
# 8-11) menu walk (12 nav-demo captures, MASHED_DET_MPG_PIN=0 via knob_boot.py): hand / mat / hand / mat
export CAPFLAKE_DATA=$D CAPFLAKE_ROOT=$R
for a in 1 2; do
  step walk_hand$a; MASHED_TOP05_HANDFRAME=1 py -3.12 verify/porttop_20261108/capflake_runs.py walk_hand$a ON@dflt
  step walk_mat$a;  env -u MASHED_TOP05_HANDFRAME py -3.12 verify/porttop_20261108/capflake_runs.py walk_mat$a ON@dflt
done
