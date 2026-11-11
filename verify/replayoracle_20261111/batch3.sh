#!/bin/bash
# RO-WARP-NORMALIZE batch 3: batch 2's configuration (tick clock, timer pin, q0 0xC0000000, same raw ranges) plus the
# boot-counter pins of re/frida/warp_normalize.tsv at the warp and at the race anchor.
#   n3_a n3_b n3_c : no load hold   n3_h : load hold 50 ms   n3_neg : negctl 1   n3_w : write watch on 0x00616110
R=verify/replayoracle_20261111/run_arm_norm.sh
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400
C="--clock tick --q0 0xC0000000 --tick-raw $RAW --call-log --timeout 400 --normalize re/frida/warp_normalize.tsv"
i=0; N=6
step() { i=$((i+1)); echo "[$i/$N] $1 start $(date +%T)"; bash $R "$@"; }
step n3_a $C --load-sleep-ms 0
step n3_b $C --load-sleep-ms 0
step n3_c $C --load-sleep-ms 0
step n3_h $C --load-sleep-ms 50
step n3_neg $C --load-sleep-ms 0 --negctl 1
step n3_w $C --load-sleep-ms 0 --watch 0x00616110
