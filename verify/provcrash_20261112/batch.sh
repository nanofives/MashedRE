#!/bin/bash
# usage: batch.sh <label>:<rva> ...   runs single-body arms in sequence inside one mashed_lock hold
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-CRASH-12
n=0; total=$#
for a in "$@"; do n=$((n+1)); echo "[$n/$total] ${a%%:*} start $(date +%T)"; bash verify/provcrash_20261112/run_arm.sh "${a%%:*}" "${a##*:}"; done
