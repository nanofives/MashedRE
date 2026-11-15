#!/bin/bash
# usage: run_slotlist.sh <name> <hold> [scenario_launch args...]
# One original-side boot with --peek fslist (read-only RtFS list walk). Run under: test-slot bash run_slotlist.sh ...
# mashed_lock is INSIDE the slot.
set -u
WT=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-PIZFS-SLOT-TRACE
OW=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration
DATA=D:/MashedWT/RO-PIZFS-SLOT-TRACE_data
NAME=$1; HOLD=$2; shift 2
cd "$WT" || exit 9
export MASHED_ROOT=D:/MashedWT/RO-PIZFS-SLOT-TRACE_root
export PYTHONIOENCODING=utf-8
py -3.12 "$OW/re/orchestrator/mashed_lock.py" run --label RO-PIZFS-SLOT-LIST-RUN -- \
  py -3.12 "$WT/re/frida/scenario_launch.py" --hold "$HOLD" \
  --peek "fslist,007d3e54:u,007d3e58:u,0068f4cc:u" "$@" \
  2>&1 | tee "$DATA/$NAME.log"
