#!/bin/bash
# usage: run.sh <name> <lo:hi> [period_ms] [track] [hold]
# One original-side boot + warp track load with a sampled guard-page access watch on one range.
set -u
WT=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-PIZFS-READERS-RUN
OW=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration
DATA=D:/MashedWT/RO-PIZFS-READERS-RUN_data
NAME=$1; RANGE=$2; PER=${3:-25}; TRACK=${4:-0}; HOLD=${5:-10}
cd "$WT" || exit 9
export MASHED_ROOT=D:/MashedWT/RO-PIZFS-READERS-RUN_root
py -3.12 "$OW/re/orchestrator/mashed_lock.py" run --label RO-PIZFS-READERS-RUN -- \
  py -3.12 "$WT/re/frida/scenario_launch.py" --track "$TRACK" --hold "$HOLD" \
  --access-watch "$RANGE" --access-watch-period "$PER" --access-watch-out "$DATA/$NAME.tsv" \
  2>&1 | tee "$DATA/$NAME.log"
