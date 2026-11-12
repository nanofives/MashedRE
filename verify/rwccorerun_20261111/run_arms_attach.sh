#!/bin/bash
# RWC-PLUGIN-ATTACH arms (same knob and arms as PREREG_RWC-CORE-RUN.md). Call inside test-slot -> mashed_lock.py run.
# Kills only the PIDs knob_boot.py spawns (on its own timeout). Usage: run_arms_attach.sh [arm ...] (default: all four)
cd "$(dirname "$0")/../.."
export PORTTOP_OUT=D:/MashedWT/RWC-PLUGIN-ATTACH_data
ROOT=D:/MashedWT/RWC-PLUGIN-ATTACH_root
COMMON="MASHED_ORIG_TOP=1 MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600"
DDK="$(pwd)/mashedmod/build/ddkobj/mashed_re_ddk.exe"
ARMS="${@:-vs_off vs_core ddk_off ddk_core}"
for a in $ARMS; do
  case $a in
    vs_off)   py -3.12 verify/porttop_20261108/knob_boot.py vs_off $ROOT 180 $COMMON ;;
    vs_core)  py -3.12 verify/porttop_20261108/knob_boot.py vs_core $ROOT 120 $COMMON MASHED_RWC_CORE=1 ;;
    ddk_off)  KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_off $ROOT 180 $COMMON ;;
    ddk_core) KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_core $ROOT 120 $COMMON MASHED_RWC_CORE=1 ;;
  esac
done
