#!/bin/bash
# RWC-CORE-RUN arms (PREREG_RWC-CORE-RUN.md). Call inside test-slot -> mashed_lock.py run. Kills only the PIDs
# knob_boot.py spawns (on its own timeout).
cd "$(dirname "$0")/../.."
export PORTTOP_OUT=D:/MashedWT/RWC-CORE-RUN_data
ROOT=D:/MashedWT/RWC-CORE-RUN_root
COMMON="MASHED_ORIG_TOP=1 MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600"
DDK="$(pwd)/mashedmod/build/ddkobj/mashed_re_ddk.exe"
py -3.12 verify/porttop_20261108/knob_boot.py vs_off $ROOT 180 $COMMON
py -3.12 verify/porttop_20261108/knob_boot.py vs_core $ROOT 120 $COMMON MASHED_RWC_CORE=1
KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_off $ROOT 180 $COMMON
KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_core $ROOT 120 $COMMON MASHED_RWC_CORE=1
