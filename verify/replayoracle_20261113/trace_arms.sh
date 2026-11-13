#!/bin/bash
# RO-EXE-TICK-TRACE arms (PREREG_RO-EXE-TICK-TRACE.md). Call inside test-slot; takes mashed_lock itself per arm.
# usage: trace_arms.sh <vs|ddk> <arm> [<arm> ...]   (arms are run_arms.py RO_ names). Kills only PIDs run_arms.py spawns.
cd "$(dirname "$0")/../.."
B=$1; shift
export A4_OUT=D:/MashedWT/RO-EXE-TICK-TRACE_data A4_ROOT=D:/MashedWT/RO-EXE-TICK-TRACE_root
if [ "$B" = ddk ]; then export A4_EXE="$(pwd -W)/mashedmod/build/ddkobj/mashed_re_ddk.exe"; else export A4_EXE="$(pwd -W)/mashedmod/build/mashed_re.exe"; fi
mkdir -p $A4_OUT
for a in "$@"; do
  py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label RO-EXE-TICK-TRACE -- \
    py -3.12 verify/savelink_20261023/run_arms.py $a
done
