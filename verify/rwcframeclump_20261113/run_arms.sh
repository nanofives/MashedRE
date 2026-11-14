#!/bin/bash
# RWC-FRAME-CLUMP arms (knob MASHED_RWC_FRAME, as RWC-FRAME-492E90; MASHED_RWC_FRAME_N = present count before the boot ends).
# Call inside test-slot -> mashed_lock.py run. Kills only the PIDs knob_boot.py spawns (on its own timeout).
# Usage: run_arms.sh [arm ...]   (default: vs_off vs_frame vs_frameN)
cd "$(dirname "$0")/../.."
export PORTTOP_OUT=${RWC_DATA:-D:/MashedWT/RWC-FRAME-CLUMP_data}
ROOT=${RWC_ROOT:-D:/MashedWT/RWC-FRAME-CLUMP_root}
COMMON="MASHED_ORIG_TOP=1 MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600"
DDK="$(pwd)/mashedmod/build/ddkobj/mashed_re_ddk.exe"
N=${RWC_N:-120}
ARMS="${@:-vs_off vs_frame vs_frameN}"
for a in $ARMS; do
  case $a in
    vs_off)     py -3.12 verify/porttop_20261108/knob_boot.py vs_off $ROOT 180 $COMMON ;;
    ddk_off)    KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_off $ROOT 180 $COMMON ;;
    vs_frame)   py -3.12 verify/porttop_20261108/knob_boot.py vs_frame $ROOT 120 $COMMON MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 ;;
    ddk_frame)  KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_frame $ROOT 120 $COMMON MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 ;;
    vs_frameN)  py -3.12 verify/porttop_20261108/knob_boot.py vs_frameN $ROOT 180 $COMMON MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=$N ;;
    ddk_frameN) KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_frameN $ROOT 180 $COMMON MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=$N ;;
    # run 46: teardown (the 600-frame loop ends, AppDestroy + SoftwareTidyUpBeforeExiting run on the core) and the core draw dump
    vs_frameT)  py -3.12 verify/porttop_20261108/knob_boot.py vs_frameT $ROOT 240 $COMMON MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=100000 MASHED_OT_TIMER=${FT_TIMER:-1} ;;
    ddk_frameT) KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_frameT $ROOT 240 $COMMON MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=100000 MASHED_OT_TIMER=${FT_TIMER:-1} ;;
    vs_dump)    py -3.12 verify/porttop_20261108/knob_boot.py vs_dump $ROOT 240 $COMMON MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=590 MASHED_OT_TIMER=1 MASHED_RWC_DRAWDUMP_SKIP=${DUMPSKIP:-200} MASHED_RWC_DRAWDUMP_FRAMES=${DUMPFRAMES:-8} MASHED_RWC_DRAWDUMP=$PORTTOP_OUT/core_burst_${DUMPTAG:-r46}.json ;;
    ddk_dump)   KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_dump $ROOT 240 $COMMON MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=590 MASHED_OT_TIMER=1 MASHED_RWC_DRAWDUMP_SKIP=${DUMPSKIP:-200} MASHED_RWC_DRAWDUMP_FRAMES=${DUMPFRAMES:-8} MASHED_RWC_DRAWDUMP=$PORTTOP_OUT/core_burst_ddk_${DUMPTAG:-r46}.json ;;
  esac
done
