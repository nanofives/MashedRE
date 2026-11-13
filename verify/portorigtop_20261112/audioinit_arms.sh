#!/bin/bash
# PORT-TOP-4669B0 arms: knob boots of this row's two exes. Call inside test-slot -> mashed_lock.py run.
# Kills only the PIDs knob_boot.py spawns (on its own timeout).
cd "$(dirname "$0")/../.."
export PORTTOP_OUT=D:/MashedWT/PORT-TOP-4669B0_data
ROOT=D:/MashedWT/PORT-TOP-4669B0_root
COMMON="MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600"
DDK="$(pwd)/mashedmod/build/ddkobj/mashed_re_ddk.exe"
OT="MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1"
# control: knob off, both builds (OT_AUDIOTICK on, piece 3 as before this row)
py -3.12 verify/porttop_20261108/knob_boot.py vs_audio $ROOT 180 $COMMON $OT MASHED_OT_AUDIOTICK=1
KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_audio $ROOT 180 $COMMON $OT MASHED_OT_AUDIOTICK=1
# knob on, no audio tick: the matched 0x004669b0 runs, its self-test, then 600 frames
py -3.12 verify/porttop_20261108/knob_boot.py vs_init $ROOT 180 $COMMON $OT MASHED_OT_AUDIOINIT=1
KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_init $ROOT 180 $COMMON $OT MASHED_OT_AUDIOINIT=1
# knob on + audio tick: the audio-tick bodies pass their DAT_006904e8 gate and reach their callees
py -3.12 verify/porttop_20261108/knob_boot.py vs_init_audio $ROOT 180 $COMMON $OT MASHED_OT_AUDIOINIT=1 MASHED_OT_AUDIOTICK=1
KNOB_EXE=$DDK py -3.12 verify/porttop_20261108/knob_boot.py ddk_init_audio $ROOT 180 $COMMON $OT MASHED_OT_AUDIOINIT=1 MASHED_OT_AUDIOTICK=1
