#!/bin/bash
# RO-67F0B8-CALLER: the original on the og1 recipe (oraclegate/og_arms.sh orig, as PORT-TOP-BODY-43DFD0 run 49 leg origi),
# write watch on 0x0067f0b8 armed at the PE entry (--watch-from install), plus esp/ebp, a FUZZY backtrace and 256 raw
# stack dwords per hit (--watch-stack 256), so the caller chain of 0x0042e5b0 can be resolved offline.
# Call under test-slot (outside); the launch takes mashed_lock itself (inside). Kills only PIDs detorig.py spawns.
#   run.sh [label]   (default og1ws50)
cd "$(dirname "$0")/../.."
D=D:/MashedWT/RO-67F0B8-CALLER_data
mkdir -p $D
L=${1:-og1ws50}
LOCK=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
rm -rf $D/$L
MASHED_ROOT='D:\MashedWT\RO-ORACLE-GATE_root' py -3.12 $LOCK run --label RO-67F0B8-CALLER -- py -3.12 re/frida/detorig.py \
  --label $L --out $D/$L --clock tick --boot --q0 0x40000000 --tick-raw $RAW --timeout ${TMO:-400} \
  --tick-cap ${CAP:-600} --warp-tick 300 --warp-clock 300 --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 \
  --ckpt-ticks 0,1,16,17,18,299,301,600 --watch 0x0067f0b8 --watch-from install --watch-stack 256 > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "warp at|race anchor|ticks |=== rc|exited|agent error|error|watch" $D/$L.log | cut -c1-300 | head -20
