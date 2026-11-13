#!/bin/bash
# usage: run_watch_boot.sh <label> [detorig.py args...]   (RO-CTL-68BC0C; call under test-slot, it takes the game lock itself)
# Boot-mode replay (verify/robootcrash3_20261112/run_arm.sh configuration) on a private root, with the hardware write
# watch on 0x0068bc0c / 0x0068bc1c armed at the PE entry point (--watch-from install) and re-armed at the warp.
L=$1; shift
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-CTL-68BC0C
export MASHED_ROOT='D:\MashedWT\RO-CTL-68BC0C_root'
D=D:/MashedWT/RO-CTL-68BC0C_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
mkdir -p $D; rm -rf $D/$L
py -3.12 /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label RO-CTL-68BC0C -- \
 py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --boot --q0 0xC0000000 --tick-raw ${TICKRAW:-$RAW} --timeout ${TMO:-400} \
   --tick-cap ${CAP:-6000} --warp-tick ${WT:-900} --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 \
   --watch 0x0068bc0c,0x0068bc1c --watch-from install "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "boot|watch|warp at|race anchor|=== rc|exited|agent error|error|err" $D/$L.log | cut -c1-400
