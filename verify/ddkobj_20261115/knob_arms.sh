#!/bin/bash
# DDKOBJ-PROV-MIXED-UNIT knob boots (VS2022 exe and object-build exe, MASHED_ORIG_TOP=1).
# Call from the worktree root inside test-slot -> mashed_lock.py run. ARMS="vs ddk" (default both).
# Logs: D:/MashedWT/DDKOBJ-PROV-MIXED-UNIT_data/<label>.log
ROOT=D:/MashedWT/DDKOBJ-PROV-MIXED-UNIT_root
COMMON="MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600"
DDK="$(pwd -W)/mashedmod/build/ddkobj/mashed_re_ddk.exe"
export PORTTOP_OUT=D:/MashedWT/DDKOBJ-PROV-MIXED-UNIT_data
ARMS="${ARMS:-vs ddk}"
for arm in $ARMS; do
  case $arm in
    vs)  py -3.12 verify/porttop_20261108/knob_boot.py vs_top $ROOT 180 $COMMON MASHED_ORIG_TOP=1 ;;
    ddk) KNOB_EXE="$DDK" py -3.12 verify/porttop_20261108/knob_boot.py ddk_top $ROOT 180 $COMMON MASHED_ORIG_TOP=1 ;;
  esac
done
