#!/bin/bash
# REPLAY-ORACLE-DESIGN batch 1 (one game-lock hold, test slot outside). Arms in order:
#   t_a t_b t_c : --clock tick, no load hold          (portable clock rule; load length left free)
#   t_h         : --clock tick, load hold 50 ms        (load length forced to the usual 118 calls)
#   t_neg       : --clock tick, no load hold, negctl 1 (RW RNG word +1 at the race anchor)
#   n_a n_b     : --clock n (pilot rule), load hold, --tick-hash (per-tick view of the LOADHOLD config)
R=verify/replayoracle_20261111/run_arm.sh
i=0; N=7
step() { i=$((i+1)); echo "[$i/$N] $1 start $(date +%T)"; bash $R "$@"; }
step t_a  --clock tick --load-sleep-ms 0 --call-log --timeout 400
step t_b  --clock tick --load-sleep-ms 0 --call-log --timeout 400
step t_c  --clock tick --load-sleep-ms 0 --call-log --timeout 400
step t_h  --clock tick --load-sleep-ms 50 --call-log --timeout 400
step t_neg --clock tick --load-sleep-ms 0 --negctl 1 --call-log --timeout 400
step n_a  --clock n --tick-hash --load-sleep-ms 50 --call-log --timeout 400
step n_b  --clock n --tick-hash --load-sleep-ms 50 --call-log --timeout 400
