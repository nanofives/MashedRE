#!/bin/bash
# RO-EXE-REPLAY-KNOB arms (PREREG_RO-EXE-REPLAY-KNOB.md). Call inside test-slot -> mashed_lock.py run.
# Kills only the PIDs knob_boot.py spawns (on its own timeout).
cd "$(dirname "$0")/../.."
export PORTTOP_OUT=D:/MashedWT/RO-EXE-REPLAY-KNOB_data
mkdir -p $PORTTOP_OUT
ROOT=D:/MashedWT/RO-EXE-REPLAY-KNOB_root
COMMON="MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600"
DDK="$(pwd)/mashedmod/build/ddkobj/mashed_re_ddk.exe"
RJ="$(pwd -W)/verify/replayoracle_20261113/replay_selftest.json"
for arm in "$@"; do
  case $arm in
    vs_off)   py -3.12 verify/porttop_20261108/knob_boot.py vs_off $ROOT 180 $COMMON ;;
    vs_top)   py -3.12 verify/porttop_20261108/knob_boot.py vs_top $ROOT 180 $COMMON MASHED_ORIG_TOP=1 ;;
    vs_rep_a) py -3.12 verify/porttop_20261108/knob_boot.py vs_rep_a $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_REPLAY=$RJ ;;
    vs_rep_b) py -3.12 verify/porttop_20261108/knob_boot.py vs_rep_b $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_REPLAY=$RJ ;;
    ddk_top)  KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_top $ROOT 180 $COMMON MASHED_ORIG_TOP=1 ;;
    ddk_rep)  KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_rep $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_REPLAY=$RJ ;;
    vs_timer) py -3.12 verify/porttop_20261108/knob_boot.py vs_timer $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1 ;;
    ddk_timer) KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_timer $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1 ;;
    vs_rep_nophase) py -3.12 verify/porttop_20261108/knob_boot.py vs_rep_nophase $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_REPLAY="${RJ%.json}_nophase.json" ;;
  esac
done
