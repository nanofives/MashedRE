#!/bin/sh
# D4-DC-4d20-GRID arm driver (PREREG_D4-DC-4d20-GRID.md section 2). Run under mashed_lock.py run --solo.
set -u
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D4-DC-4d20-GRID
export A4_OUT='D:\MashedWT\D4-DC-4d20-GRID_data\arms'
export A4_ROOT='D:\MashedWT\D4-DC-4d20-GRID_root'
export A4_EXE="$(cygpath -w "$W/mashedmod/build/mashed_re.exe")"
cd "$W"
py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_GR B_GR_K0 B_GR_K1 B_GR_K26 H_GT
py -3.12 verify/savelink_20261023/run_arms.py D2_DEF_1 D2_DEF_2 D2_DEF_3 D2_GR_1 D2_GR_2 D2_GR_3 D2_GT_1 D2_GRT_1
py -3.12 verify/rsky_20261016/run13.py run --exe "$A4_EXE" --root 'D:\MashedWT\D4-DC-4d20-GRID_root13' --out 'D:\MashedWT\D4-DC-4d20-GRID_data\tracks'
echo ALL_DONE
