#!/bin/bash
# usage: batch.sh <label>[:<rva>] ...   runs the arms in sequence inside one mashed_lock hold (PROV-FIX-9)
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-FIX-9
n=0; total=$#
for a in "$@"; do n=$((n+1)); l=${a%%:*}; r=""; [[ "$a" == *:* ]] && r=${a##*:}; echo "[$n/$total] $l start $(date +%T)"; bash verify/provfix9_20261112/run_arm.sh "$l" "$r"; done
