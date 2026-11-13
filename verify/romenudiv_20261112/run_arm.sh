#!/bin/bash
# usage: run_arm.sh <label> [detorig.py args...]   (RO-MENU-DIV-43DFD0; call under test-slot, it takes the game lock itself)
# Same boot-mode replay as verify/romenu_20261112/run_arm.sh, plus raw per-tick copies of the blocks the
# two DIVERGED bodies differ in: 0x0067ec00 / 0x007d6000 (0x0043dfd0) and 0x00898800 (0x0043d2a0).
L=$1; shift
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-MENU-DIV-43DFD0
export MASHED_ROOT='D:\MashedWT\RO-MENU-DIV-43DFD0_root'
D=D:/MashedWT/RO-MENU-DIV-43DFD0_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
RAW=$RAW,0x0067ec00-0x0067f000,0x007d6000-0x007d6400,0x00898800-0x00898c00${XRAW:+,$XRAW}
mkdir -p $D; rm -rf $D/$L
py -3.12 /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label RO-MENU-DIV-43DFD0 -- \
 py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --boot --q0 0xC0000000 --tick-raw ${TICKRAW:-$RAW} --timeout ${TMO:-700} \
   --tick-cap ${CAP:-4000} --warp-tick ${WT:-600} --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "boot|prov|warp at|race anchor|ticks |=== rc|exited|agent error|error|err" $D/$L.log | cut -c1-300
