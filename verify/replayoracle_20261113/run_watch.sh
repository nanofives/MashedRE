#!/bin/bash
# usage: run_watch.sh <label> [detorig.py args...]   (RO-CTL-68BC0C; call inside mashed_lock, under test-slot)
# PROV-INSITU's normalised tick-clock replay configuration (verify/provinsitu_20261111/run_arm.sh) on a private root,
# with the hardware write watch on 0x0068bc0c armed from the install (--watch-from install).
L=$1; shift
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-CTL-68BC0C
export MASHED_ROOT='D:\MashedWT\RO-CTL-68BC0C_root'
D=D:/MashedWT/RO-CTL-68BC0C_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
mkdir -p $D; rm -rf $D/$L
py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --q0 0xC0000000 --tick-raw ${TICKRAW:-$RAW} --timeout 400 \
   --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 --warp-clock 2000 \
   --watch 0x0068bc0c,0x0068bc1c --watch-from install "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "watch|warp at|race anchor|=== rc|exited|agent error|error" $D/$L.log | cut -c1-400
