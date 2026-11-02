#!/usr/bin/env bash
# HB-SHADOW-ALLOC-COMPARE pass 2 follow-ups: 0x0057c500 re-run after a launch failure ("could not attach"),
# then the PREREG HOLD=180 re-runs of every row with fewer than 5 samples (tag h180).
cd "$(dirname "$0")/../.."
D=verify/hb_shadowalloc_20261101/drive_sa.py
export MASHED_ROOT=D:/MashedWT/HB-SHADOW-ALLOC-COMPARE_root MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1 TRACK=0 HOLD=60 SAMPLES_DONE=24
echo "[arm S re-run]"; MASHED_SHADOW_ALLOC=1 py -3.12 $D shadow 0057c500 || exit $?
echo "[arm h180]"; MASHED_SHADOW_ALLOC=1 HOLD=180 SHADOW_TAG=h180 py -3.12 $D shadow 0055dca0 0055dc70 || exit $?
echo "[arm C0 h180]"; MASHED_SHADOW_ALLOC=1 HOLD=180 SHADOW_TAG=c0_h180 py -3.12 $D shadow 0055ddd0 || exit $?
echo "[rerun done]"
