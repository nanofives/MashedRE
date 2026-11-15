#!/bin/bash
# run 48 slot 5 (inside test-slot). (1) RWC-FRAME-BOOT-AV [UNCERTAIN] about RWC-FRAME-CLUMP's passing arms: the frame route on
# an exe WITHOUT the 0x005c47e0 fix (orch/PORT-TOP-BODY-43DFD0 on 830f22e19, knob MASHED_PROV_43DFD0 off), with and without
# MASHED_DET_FRAMES=600 (CLUMP's run_arms.sh COMMON). (2) the self-test fault site: catch_av.py sees the first-chance AV that
# the self-test's __except then handles.
LOCK=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
W43=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PORT-TOP-BODY-43DFD0
D43=D:/MashedWT/PORT-TOP-BODY-43DFD0_data
FR="MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=120"
cd $W43
B=verify/porttopbody43dfd0_20261114/b43_arms.sh
bash $B knob vs nofix_frameN 120 $FR
bash $B knob vs nofix_frameN_det600 120 $FR MASHED_DET_FRAMES=600
for L in nofix_frameN nofix_frameN_det600; do echo "[$L] $(grep -E 'rc=' $D43/$L.out | head -1)"; grep -E "AV at|boot ends after present" $D43/$L.log | cut -c1-200; done
CATCH_TMO=60 py -3.12 $LOCK run --label PORT-TOP-BODY-43DFD0 -- py -3.12 verify/porttopbody43dfd0_20261114/catch_av.py \
  $W43/mashedmod/build/mashed_re.exe $W43/mashedmod/build/mashed_re.map 'D:\MashedWT\PORT-TOP-BODY-43DFD0_root' $D43/av_selftest.json \
  MASHED_WIN_POS=primary-bl MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600 MASHED_PROV_43DFD0_SELFTEST=1 > $D43/av_selftest.out 2>&1
echo "[selftest catch] exit $?"; head -30 $D43/av_selftest.out
echo "== slot 5 done $(date +%T)"
