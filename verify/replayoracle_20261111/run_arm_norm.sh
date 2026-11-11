#!/bin/bash
# usage: run_arm_norm.sh <label> [detorig.py args...]   (RO-WARP-NORMALIZE; call inside the game lock, under test-slot)
L=$1; shift
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-WARP-NORMALIZE
export MASHED_ROOT='D:\MashedWT\RO-WARP-NORMALIZE_root'
D=D:/MashedWT/RO-WARP-NORMALIZE_data
mkdir -p $D; rm -rf $D/$L
py -3.12 re/frida/detorig.py --label $L --out $D/$L "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "normalise|warp at|race anchor|call log|ticks |=== rc|exited|agent error|error" $D/$L.log | cut -c1-400
