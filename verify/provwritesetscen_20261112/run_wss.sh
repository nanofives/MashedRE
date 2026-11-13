#!/bin/bash
# PROV-WRITESET-SCENARIO: run_ws.sh of PROV-WRITESET-114 with stride sampling over the whole race (--prov-wstride).
# usage: run_wss.sh <label> <rvas,comma> [N max stepped calls per body]   (call inside the game lock, under test-slot)
L=$1; RV=$2; N=${3:-100000}
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-WRITESET-SCENARIO
export MASHED_ROOT='D:\MashedWT\PROV-WRITESET-SCENARIO_root'
D=D:/MashedWT/PROV-WRITESET-SCENARIO_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
mkdir -p $D; rm -rf $D/$L
py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --q0 0xC0000000 --tick-raw $RAW --timeout ${TMO:-1500} \
   --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 --warp-clock 2000 \
   --prov verify/provinsitu_20261111/bodies_all.json --prov-rvas $RV --prov-wlog $N \
   --prov-wstride verify/provwritesetscen_20261112/wstride.json --prov-warm anchor ${WCAP:+--prov-wcap $WCAP} > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "prov|warp at|race anchor|ticks |wlog|=== rc|exited|agent error|error|wcrash" $D/$L.log | cut -c1-300
