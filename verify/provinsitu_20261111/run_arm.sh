#!/bin/bash
# usage: run_arm.sh <label> [detorig.py args...]   (PROV-INSITU; call inside the game lock, under test-slot)
# Normalised tick-clock replay of the original (batch-3 configuration of RO-WARP-NORMALIZE) plus the PROV options.
L=$1; shift
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-INSITU
export MASHED_ROOT='D:\MashedWT\PROV-INSITU_root'
D=D:/MashedWT/PROV-INSITU_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
mkdir -p $D; rm -rf $D/$L
py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --q0 0xC0000000 --tick-raw ${TICKRAW:-$RAW} --timeout 400 \
   --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "prov|warp at|race anchor|ticks |=== rc|exited|agent error|error" $D/$L.log | cut -c1-300
