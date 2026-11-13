#!/bin/bash
# usage: run_fix.sh <label> <rva> [detorig.py args...]   (PROV-CRASH-12; call inside the game lock, under test-slot)
# The PROV-INSITU replay (batch-3 normalised tick clock, verify/provinsitu_20261111/run_arm.sh) with one FIXED --prov body (re/tools/prov/prov_fixlink.py)
# and the --crash-catch first-fault / stall catcher.
L=$1; R=$2; shift 2
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-CRASH-12
export MASHED_ROOT='D:\MashedWT\PROV-CRASH-12_root'
D=D:/MashedWT/PROV-CRASH-12_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
mkdir -p $D; rm -rf $D/$L
py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --q0 0xC0000000 --tick-raw $RAW --timeout ${TMO:-400} \
   --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 --warp-clock 2000 \
   --prov verify/provcrash_20261112/bodies_fix.json --prov-rvas 0x$R --crash-catch ${STALL:-20} "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "prov|warp at|race anchor|exited|agent error|error|CRASH|=== rc" $D/$L.log | cut -c1-400
