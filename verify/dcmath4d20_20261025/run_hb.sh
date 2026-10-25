#!/bin/sh
# dcmath4d20 G1/G2 re-run (the first driver pass lost H_DEF: the arms dir did not exist yet).
set -u
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D4-DC-MATH-4d20
export A4_OUT='D:\MashedWT\D4-DC-MATH-4d20_data\arms'
export A4_ROOT='D:\MashedWT\D4-DC-MATH-4d20_root'
export A4_EXE="$(cygpath -w "$W/mashedmod/build/mashed_re.exe")"
cd "$W"
py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_CR B_CR_K0 B_CR_K1 B_CR_K26
echo ALL_DONE
