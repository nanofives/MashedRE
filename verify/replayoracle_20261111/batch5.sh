#!/bin/bash
# RO-STACKROW-READERS + RO-AUDIO-ISOLATION batch 5: write watches on the dwords batch 4 showed moving
#   ao_w : audio workers suspended at the warp, watch 0x006904c8 and 0x006036cc
#   sp_w : stale-field poison, watch 0x006fe7ec and 0x006fe7dc
#   n3_w2: plain batch-3 configuration, watch 0x006fe7ec and 0x006904c8 (the writers when nothing is perturbed)
R=verify/replayoracle_20261111/run_arm_norm.sh
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400
C="--clock tick --q0 0xC0000000 --tick-raw $RAW --call-log --timeout 400 --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0"
i=0; N=3
step() { i=$((i+1)); echo "[$i/$N] $1 start $(date +%T)"; bash $R "$@"; }
step ao_w $C --audio-off warp --watch 0x006904c8,0x006036cc
step sp_w $C --poison stale --watch 0x006fe7ec,0x006fe7dc
step n3_w2 $C --watch 0x006fe7ec,0x006904c8
