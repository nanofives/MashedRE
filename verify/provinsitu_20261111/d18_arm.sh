#!/bin/bash
# PROV-DIVERGED-18: one replay of the original with DDK bodies live (same configuration as run_arm.sh of PROV-INSITU),
# in this row's private root and data folder.
#   usage: d18_arm.sh <label> [<bodies.json> <rva[,rva]>]      (no bodies: a reference run)
# Call it inside test-slot; it takes the game lock itself (mashed_lock.py run).
# The compare uses this root's own references d18_r1 / d18_r2: the original keeps its exe path in 0x00773c50..74
# (block 0x00773c00), so a run in another root differs there at every tick (checked: f448220_a against PROV-INSITU r1).
# The PROV-INSITU pair x_ref / x_ref2 is added as an allowed-block pair, as PROV-INSITU did.
L=$1; B=$2; R=$3
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-DIVERGED-18
cd $W
export MASHED_ROOT='D:\MashedWT\PROV-DIVERGED-18_root'
D=D:/MashedWT/PROV-DIVERGED-18_data
I=D:/MashedWT/PROV-INSITU_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
P=()
[ -n "$B" ] && P=(--prov "$B" --prov-rvas "$R")
mkdir -p $D; rm -rf $D/$L
py -3.12 /c/Users/maria/Desktop/Proyectos/Mashed/re/orchestrator/mashed_lock.py run --label PROV-DIVERGED-18 -- \
  py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --q0 0xC0000000 --tick-raw $RAW --timeout 400 \
   --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 --warp-clock 2000 "${P[@]}" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "prov rows|prov install|race anchor|=== rc|agent error" $D/$L.log | cut -c1-160 | tail -4
if [ -d $D/d18_r1 ] && [ -d $D/d18_r2 ] && [ "$L" != d18_r1 ] && [ "$L" != d18_r2 ]; then
  py -3.12 re/frida/prov_compare.py $D/$L $D/d18_r1 $D/d18_r2 --pair $I/x_ref,$I/x_ref2 --json $D/$L.verdict.json 2>&1 | head -3
  py -3.12 re/tools/prov/prov_ticks_all.py $D/$L $D/d18_r1 $D/d18_r2 --pair $I/x_ref,$I/x_ref2 > $D/$L.ticks.txt 2>&1
  head -6 $D/$L.ticks.txt
fi
