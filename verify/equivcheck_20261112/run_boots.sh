#!/usr/bin/env bash
# EQUIV-FU-48E820 knob boots: default exe and DDK object exe, MASHED_ORIG_TOP=1, 3000 frames, private root.
# Run inside test-slot (outside) with mashed_lock (inside), per WORKER_RULES.
set -u
cd "$(dirname "$0")/../.."
L=$(cygpath -w ../orch-integration/re/orchestrator/mashed_lock.py)
export PORTTOP_OUT=D:/MashedWT/EQUIV-FU-48E820_data
R=D:/MashedWT/EQUIV-FU-48E820_root
py -3.12 "$L" run --label EQUIV-FU-48E820 -- py -3.12 verify/porttop_20261108/knob_boot.py knob_on "$R" 600 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000
KNOB_EXE="$(cygpath -w "$PWD/mashedmod/build/ddkobj/mashed_re_ddk.exe")" py -3.12 "$L" run --label EQUIV-FU-48E820 -- py -3.12 verify/porttop_20261108/knob_boot.py knob_on_ddk "$R" 600 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000
echo BOOTS_DONE
