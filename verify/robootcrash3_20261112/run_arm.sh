#!/bin/bash
# usage: run_arm.sh <label> [detorig.py args...]   (RO-BOOT-CRASH-3, copied from verify/romenu_20261112/run_arm.sh; call under test-slot, it takes the game lock itself)
# Boot-mode replay of the original: tick clock and per-tick hashes from the PE entry point, through the main menu,
# warp at a fixed tick, then the oracle's race (RO-WARP-NORMALIZE batch-3 / PROV-INSITU configuration).
L=$1; shift
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-BOOT-CRASH-3
export MASHED_ROOT='D:\MashedWT\RO-BOOT-CRASH-3_root'
D=D:/MashedWT/RO-BOOT-CRASH-3_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
mkdir -p $D; rm -rf $D/$L
py -3.12 /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label RO-BOOT-CRASH-3 -- \
 py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --boot --q0 0xC0000000 --tick-raw ${TICKRAW:-$RAW} --timeout ${TMO:-400} \
   --tick-cap ${CAP:-6000} --warp-tick ${WT:-900} --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "boot|prov|warp at|race anchor|ticks |=== rc|exited|agent error|error|err" $D/$L.log | cut -c1-300
