#!/bin/bash
# RWC-FRAME-BOMB-DESTROY arms (PREREG_RWC-FRAME-BOMB-DESTROY.md). Call inside test-slot -> mashed_lock.py run.
# knob_boot.py kills only the PID it spawned (on its own timeout). Usage: run_arms.sh arm [arm ...]
cd "$(dirname "$0")/../.."
export PORTTOP_OUT=D:/MashedWT/RWC-FRAME-BOMB-DESTROY_data
ROOT=D:/MashedWT/RWC-FRAME-BOMB-DESTROY_root
COMMON="MASHED_ORIG_TOP=1 MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600"
FRAME="MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1"
T="$FRAME MASHED_RWC_FRAME_N=100000 MASHED_OT_TIMER=1"
DDK="$(pwd)/mashedmod/build/ddkobj/mashed_re_ddk.exe"
KB=verify/porttop_20261108/knob_boot.py
for a in "$@"; do
  case $a in
    vs_frame)              py -3.12 $KB $a $ROOT 120 $COMMON $FRAME ;;
    vs_frame_bomb)         py -3.12 $KB $a $ROOT 120 $COMMON $FRAME MASHED_RWC_BOMB_DESTROY=1 ;;
    ddk_frame_bomb)        KNOB_EXE=$DDK py -3.12 $KB $a $ROOT 120 $COMMON $FRAME MASHED_RWC_BOMB_DESTROY=1 ;;
    vs_frameT)             py -3.12 $KB $a $ROOT 240 $COMMON $T ;;
    ddk_frameT)            KNOB_EXE=$DDK py -3.12 $KB $a $ROOT 240 $COMMON $T ;;
    vs_frameT_bomb)        py -3.12 $KB $a $ROOT 240 $COMMON $T MASHED_RWC_BOMB_DESTROY=1 ;;
    ddk_frameT_bomb)       KNOB_EXE=$DDK py -3.12 $KB $a $ROOT 240 $COMMON $T MASHED_RWC_BOMB_DESTROY=1 ;;
    vs_frameT_s18off)      py -3.12 $KB $a $ROOT 240 $COMMON $T MASHED_RWC_SLOTS18_OFF=1 ;;
    vs_frameT_s18off_bomb) py -3.12 $KB $a $ROOT 240 $COMMON $T MASHED_RWC_SLOTS18_OFF=1 MASHED_RWC_BOMB_DESTROY=1 ;;
    ddk_frameT_s18off)     KNOB_EXE=$DDK py -3.12 $KB $a $ROOT 240 $COMMON $T MASHED_RWC_SLOTS18_OFF=1 ;;
  esac
done
