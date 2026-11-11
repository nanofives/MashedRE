#!/bin/bash
# RWC-BIND-4C5890 arms: the four RWC-CORE-RUN arms on this row's exes. Call inside test-slot -> mashed_lock.py run.
# Kills only the PIDs knob_boot.py spawns (on its own timeout).
cd "$(dirname "$0")/../.."
export PORTTOP_OUT=D:/MashedWT/RWC-BIND-4C5890_data
ROOT=D:/MashedWT/RWC-BIND-4C5890_root
COMMON="MASHED_ORIG_TOP=1 MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600"
DDK="$(pwd)/mashedmod/build/ddkobj/mashed_re_ddk.exe"
py -3.12 verify/porttop_20261108/knob_boot.py vs_off $ROOT 180 $COMMON
py -3.12 verify/porttop_20261108/knob_boot.py vs_core $ROOT 120 $COMMON MASHED_RWC_CORE=1
KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_off $ROOT 180 $COMMON
KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_core $ROOT 120 $COMMON MASHED_RWC_CORE=1
