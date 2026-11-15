#!/bin/bash
# RWC-FRAME-BOOT-AV control arms on the orch/integration head build (no PORT-TOP-BODY-43DFD0). Call under test-slot.
#   arms.sh <vs|ddk> <label> [KEY=VALUE ...]   knob boot (rc + log), then catch_av.py on the same env
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RWC-FRAME-BOOT-AV
CATCH=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PORT-TOP-BODY-43DFD0/verify/porttopbody43dfd0_20261114/catch_av.py
LOCK=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
ROOT='D:\MashedWT\RWC-FRAME-BOOT-AV_root'
D=D:/MashedWT/RWC-FRAME-BOOT-AV_data
cd $W
B=$1; L=$2; shift 2
if [ "$B" = ddk ]; then EXE="$W/mashedmod/build/ddkobj/mashed_re_ddk.exe"; MAP="$W/mashedmod/build/ddkobj/mashed_re_ddk.map"
else EXE="$W/mashedmod/build/mashed_re.exe"; MAP="$W/mashedmod/build/mashed_re.map"; fi
echo "[$L] exe sha256 $(sha256sum "$EXE" | cut -c1-64)"
KNOB_EXE="$EXE" PORTTOP_OUT=$D py -3.12 $LOCK run --label RWC-FRAME-BOOT-AV -- \
  py -3.12 verify/porttop_20261108/knob_boot.py $L "$ROOT" 120 MASHED_WIN_POS=primary-bl "$@" > $D/$L.out 2>&1
echo "[$L] knob exit $? $(date +%T)"
grep -E "spawned|exited|TIMEOUT" $D/$L.out | head -3
grep -E "first frame|WinMain 0x00492370|presents|FATAL|RWC_FRAME" $D/$L.log 2>/dev/null | cut -c1-220 | tail -8
echo "[$L] lines $(wc -l < $D/$L.log 2>/dev/null)"
grep -q "rc=3221225477" $D/$L.out || { echo "[$L] no AV, catch skipped"; exit 0; }
CATCH_TMO=120 py -3.12 $LOCK run --label RWC-FRAME-BOOT-AV -- \
  py -3.12 $CATCH "$EXE" "$MAP" "$ROOT" $D/av_$L.json MASHED_WIN_POS=primary-bl "$@" > $D/av_$L.out 2>&1
echo "[$L] catch exit $? $(date +%T)"
grep -v "^\s*$" $D/av_$L.out | head -30
