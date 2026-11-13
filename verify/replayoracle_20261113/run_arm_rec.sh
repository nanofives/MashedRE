#!/bin/bash
# usage: run_arm_asi.sh <label> [detorig.py args...]   (RO-INPUT-RECORD; call under test-slot, it takes the game lock itself)
# RO-MENU-REPLAY boot-mode configuration. Add --asi for the in-process replay (mashed_re_dev.asi), omit it for the Frida
# reference. Root D:\MashedWT\RO-INPUT-RECORD_root holds this branch's asi at original\mashed_re_dev.asi.
L=$1; shift
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-INPUT-RECORD
export MASHED_ROOT='D:\MashedWT\RO-INPUT-RECORD_root'
D=D:/MashedWT/RO-INPUT-RECORD_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
mkdir -p $D; rm -rf $D/$L
py -3.12 /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label RO-INPUT-RECORD -- \
 py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --boot --q0 0xC0000000 --tick-raw ${TICKRAW:-$RAW} --timeout ${TMO:-700} \
   --tick-cap ${CAP:-4000} --warp-tick ${WT:-600} --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "boot|warp|race|ticks |=== rc|exited|agent error|error|err" $D/$L.log | cut -c1-300
