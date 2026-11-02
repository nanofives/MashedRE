#!/usr/bin/env bash
# HB-SHADOW-ALLOC-COMPARE: arms S, C0, K0, M1, M2 in PREREG order (each row in its own solo window, drive_sa.py).
cd "$(dirname "$0")/../.."
D=verify/hb_shadowalloc_20261101/drive_sa.py
export MASHED_ROOT=D:/MashedWT/HB-SHADOW-ALLOC-COMPARE_root MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1 TRACK=0 HOLD=60 SAMPLES_DONE=24
echo "[arm S]";  MASHED_SHADOW_ALLOC=1 py -3.12 $D shadow 0055ded0 0055dca0 0055dc70 0057c300 0057c500 || exit $?
echo "[arm C0]"; MASHED_SHADOW_ALLOC=1 SHADOW_TAG=c0 py -3.12 $D shadow 0055ddd0 || exit $?
echo "[arm K0]"; SHADOW_TAG=off py -3.12 $D shadow 0055ded0 || exit $?
echo "[arm M1]"; MASHED_SHADOW_ALLOC=1 MASHED_SHADOW_ALLOC_MUT=1 SHADOW_TAG=m1 SAMPLES_DONE=6 py -3.12 $D shadow 0055ded0 || exit $?
echo "[arm M2]"; MASHED_SHADOW_ALLOC=1 MASHED_SHADOW_ALLOC_MUT=2 SHADOW_TAG=m2 SAMPLES_DONE=6 py -3.12 $D shadow 0055ded0 || exit $?
echo "[arms done]"
