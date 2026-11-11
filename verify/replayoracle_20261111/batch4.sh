#!/bin/bash
# RO-STACKROW-READERS + RO-AUDIO-ISOLATION batch 4: batch 3's configuration (tick clock, timer pin, boot-counter pins) plus
#   sp_a   : poison every event-ring field its producer leaves unwritten (types 3 / 4 / 8), at every ring consumer
#   sp_pos : positive control, the same plus type-4 +0x24 (written by FUN_00469df0, read at 0x00422e6a)
#   ao_a ao_b : the __beginthread (audio) workers suspended at the warp     ao_k : suspended at the race anchor
R=verify/replayoracle_20261111/run_arm_norm.sh
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400
C="--clock tick --q0 0xC0000000 --tick-raw $RAW --call-log --timeout 400 --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0"
i=0; N=5
step() { i=$((i+1)); echo "[$i/$N] $1 start $(date +%T)"; bash $R "$@"; }
step sp_a $C --poison stale
step sp_pos $C --poison pos --poison-value 0x47000000
step ao_a $C --audio-off warp
step ao_b $C --audio-off warp
step ao_k $C --audio-off anchor
