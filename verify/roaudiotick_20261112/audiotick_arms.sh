#!/bin/bash
# RO-AUDIOTICK-BODIES arms: knob boots of this row's two exes. Call inside test-slot -> mashed_lock.py run.
# Kills only the PIDs knob_boot.py spawns (on its own timeout).
cd "$(dirname "$0")/../.."
export PORTTOP_OUT=D:/MashedWT/RO-AUDIOTICK-BODIES_data
ROOT=D:/MashedWT/RO-AUDIOTICK-BODIES_root
COMMON="MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600"
DDK="$(pwd)/mashedmod/build/ddkobj/mashed_re_ddk.exe"
py -3.12 verify/porttop_20261108/knob_boot.py vs_timer $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1
py -3.12 verify/porttop_20261108/knob_boot.py vs_audio $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1 MASHED_OT_AUDIOTICK=1
KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_timer $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1
KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_audio $ROOT 180 $COMMON MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1 MASHED_OT_AUDIOTICK=1
