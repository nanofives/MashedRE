#!/bin/bash
# REPLAY-ORACLE-DESIGN batch 2: tick clock with the timer pin at the warp, PTR_LO = image end, q0 0xC0000000
# (3 MHz clock values above the heap range), raw per-tick copies of the blocks batch 1 flagged.
#   t2_a t2_b t2_c : no load hold   t2_h : load hold 50 ms   t2_neg : no hold, negctl 1 (RW RNG word +1 at the anchor)
R=verify/replayoracle_20261111/run_arm.sh
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400
C="--clock tick --q0 0xC0000000 --tick-raw $RAW --call-log --timeout 400"
i=0; N=5
step() { i=$((i+1)); echo "[$i/$N] $1 start $(date +%T)"; bash $R "$@"; }
step t2_a $C --load-sleep-ms 0
step t2_b $C --load-sleep-ms 0
step t2_c $C --load-sleep-ms 0
step t2_h $C --load-sleep-ms 50
step t2_neg $C --load-sleep-ms 0 --negctl 1
