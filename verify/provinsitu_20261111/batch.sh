#!/bin/bash
# usage: batch.sh <label> [args] [-- <label> [args]]...  runs arms in sequence inside one mashed_lock hold
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-INSITU
R=verify/provinsitu_20261111/run_arm.sh
args=(); n=0; total=$(printf '%s\n' "$@" | grep -c '^--$'); total=$((total+1))
run1() { n=$((n+1)); echo "[$n/$total] $1 start $(date +%T)"; bash $R "$@"; }
for a in "$@"; do
  if [ "$a" = "--" ]; then run1 "${args[@]}"; args=(); else args+=("$a"); fi
done
[ ${#args[@]} -gt 0 ] && run1 "${args[@]}"
