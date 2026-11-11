#!/bin/bash
# usage: run_arm.sh <label> [detorig.py args...]   (call inside the game lock, under test-slot)
L=$1; shift
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/REPLAY-ORACLE-DESIGN
export MASHED_ROOT='D:\MashedWT\REPLAY-ORACLE-DESIGN_root'
D=D:/MashedWT/REPLAY-ORACLE-DESIGN_data
rm -rf $D/$L
py -3.12 re/frida/detorig.py --label $L --out $D/$L "$@" > $D/$L.log 2>&1
echo "[$L] exit $? $(date +%T)"
grep -E "race anchor|call log|ticks |=== rc|exited|agent error|error" $D/$L.log
