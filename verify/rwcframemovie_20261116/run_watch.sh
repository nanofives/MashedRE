#!/bin/bash
# RWC-FRAME-MOVIE-BACKDROP: hardware write watch on 0x00771a04 (the 0x00493f70 movie flag) on the original, armed at the
# PE entry (--watch-from install), on the og1 boot recipe (as RO-67F0B8-CALLER run.sh, without --watch-stack, which is
# not on orch/integration). Each hit gives the writing pc and value (watch.jsonl).
# Call under test-slot (outside); the launch takes mashed_lock itself (inside). Kills only PIDs detorig.py spawns.
#   run_watch.sh [label]   (default w771a04)
cd "$(dirname "$0")/../.."
D=D:/MashedWT/RWC-FRAME-MOVIE-BACKDROP_data
mkdir -p $D
L=${1:-w771a04}
LOCK=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
rm -rf $D/$L
MASHED_ROOT='D:\MashedWT\RWC-FRAME-MOVIE-BACKDROP_root' py -3.12 $LOCK run --label RWC-FRAME-MOVIE-BACKDROP -- py -3.12 re/frida/detorig.py \
  --label $L --out $D/$L --clock tick --boot --q0 0x40000000 --timeout ${TMO:-400} \
  --tick-cap ${CAP:-600} --warp-tick 300 --warp-clock 300 --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 \
  --ckpt-ticks 0,1,16,17,299,301 --watch 0x00771a04 --watch-from install > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "warp at|race anchor|ticks |=== rc|exited|agent error|error|watch" $D/$L.log | cut -c1-300 | head -20
cat $D/$L/watch.jsonl 2>/dev/null | cut -c1-400 | head -20
