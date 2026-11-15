#!/bin/bash
# run 48 slot 4 (inside test-slot): both branches restacked on 830f22e19. Step 1 bar (e, f, g), then step 2 knob boots,
# self-tests, the replay pair (object build) and the walk. Every launch takes mashed_lock.
LOCK=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
W43=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PORT-TOP-BODY-43DFD0
D43=D:/MashedWT/PORT-TOP-BODY-43DFD0_data
A=D:/MashedWT/RWC-FRAME-BOOT-AV_data/arms.sh
FR="MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=120"
echo "== step 1 (orch/RWC-FRAME-BOOT-AV on 830f22e19)"
bash $A vs vs_frameN_r48 $FR
bash $A ddk ddk_frameN_r48 $FR
bash $A vs vs_frameN_det600 $FR MASHED_DET_FRAMES=600
bash $A vs vs_off_r48 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600
bash $A ddk ddk_off_r48 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600
echo "== step 2 knob boots (orch/PORT-TOP-BODY-43DFD0 on 830f22e19)"
cd $W43
B=verify/porttopbody43dfd0_20261114/b43_arms.sh
bash $B knob vs vs_off3 120 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600
bash $B knob ddk ddk_off3 120 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600
bash $B knob vs vs_on3 120 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600 MASHED_PROV_43DFD0=1
bash $B knob ddk ddk_on3 120 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600 MASHED_PROV_43DFD0=1
bash $B knob vs selftest_on3 120 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600 MASHED_PROV_43DFD0_SELFTEST=1
bash $B knob ddk ddk_selftest_on3 120 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600 MASHED_PROV_43DFD0_SELFTEST=1
for L in vs_off3 ddk_off3 vs_on3 ddk_on3; do grep -q "returned 0 after 600 frames" $D43/$L.log 2>/dev/null && echo "[$L] PASS" || echo "[$L] FAIL"; done
echo "== step 2 replay pair (object build)"
A4_RO_CKPT=0,1,17,18,299,301,600 bash $B ro ddk RO_ROIMG-b43off RO_B43IMG-b43on
echo "== step 2 walk (VS): OFF OFF, then ON ON with a 150 s cap"
py -3.12 $LOCK run --label PORT-TOP-BODY-43DFD0 -- py -3.12 verify/porttopbody43dfd0_20261114/walk.py $W43/mashedmod/build/mashed_re.exe \
  $D43/walk3 'D:\MashedWT\PORT-TOP-BODY-43DFD0_root' OFF OFF
WALK_TMO=150 py -3.12 $LOCK run --label PORT-TOP-BODY-43DFD0 -- py -3.12 verify/porttopbody43dfd0_20261114/walk.py $W43/mashedmod/build/mashed_re.exe \
  $D43/walk3 'D:\MashedWT\PORT-TOP-BODY-43DFD0_root' ON ON
echo "== slot 4 done $(date +%T)"
