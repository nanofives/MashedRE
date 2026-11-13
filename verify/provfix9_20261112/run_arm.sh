#!/bin/bash
# usage: run_arm.sh <label> [rva]   (PROV-FIX-9; call inside the game lock, under test-slot)
# The PROV-INSITU normalised tick-clock replay (verify/provinsitu_20261111/run_arm.sh), with one body linked from the
# COMMITTED fixed source (verify/provfix9_20261112/bodies_committed.json) when rva is given, else a no-body reference.
L=$1; R=$2
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-FIX-9
export MASHED_ROOT='D:\MashedWT\PROV-FIX-9_root'
D=D:/MashedWT/PROV-FIX-9_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
PROV=(); [ -n "$R" ] && PROV=(--prov verify/provfix9_20261112/bodies_committed.json --prov-rvas $R)
mkdir -p $D; rm -rf $D/$L
py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --q0 0xC0000000 --tick-raw $RAW --timeout ${TMO:-400} \
   --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 --warp-clock 2000 "${PROV[@]}" --crash-catch ${STALL:-20} > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "prov|warp at|race anchor|exited|agent error|error|CRASH|=== rc" $D/$L.log | cut -c1-300
