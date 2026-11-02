#!/bin/bash
# usage: run_arm.sh <label> [detorig.py args...]   (call inside the game lock)
L=$1; shift
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROMOTE-DETORIG-LOADHOLD
export MASHED_ROOT='D:\MashedWT\PROMOTE-DETORIG-LOADHOLD_root'
D=D:/MashedWT/PROMOTE-DETORIG-LOADHOLD_data
rm -rf $D/$L
test-slot py -3.12 re/frida/detorig.py --label $L --out $D/$L "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "race anchor|call log|=== rc|exited|agent error" $D/$L.log
