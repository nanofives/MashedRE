#!/bin/sh
# dcmath4d20 arm driver: G1/G2 (H_, B_), G-CAM (replay), G3 (D2). Run under mashed_lock.py run --solo.
set -u
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D4-DC-MATH-4d20
export A4_OUT='D:\MashedWT\D4-DC-MATH-4d20_data\arms'
export A4_ROOT='D:\MashedWT\D4-DC-MATH-4d20_root'
export A4_EXE="$(cygpath -w "$W/mashedmod/build/mashed_re.exe")"
cd "$W"
py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_CR B_CR_K0 B_CR_K1 B_CR_K26
T=verify/d1r2_20261016/orig2_trace_v3.csv
C='D:\MashedWT\D4-DC-MATH-4d20_data\cam'
py -3.12 verify/dcmath4d20_20261025/run_replay.py "$A4_EXE" "$C\CAM_DEF" --trace $T
py -3.12 verify/dcmath4d20_20261025/run_replay.py "$A4_EXE" "$C\CAM_CR" --trace $T MASHED_CAM_ROTATE_RW
py -3.12 verify/dcmath4d20_20261025/run_replay.py "$A4_EXE" "$C\CAM_DEF2" --trace $T
py -3.12 verify/savelink_20261023/run_arms.py D2_DEF_1 D2_DEF_2 D2_DEF_3 D2_CR_1 D2_CR_2 D2_CR_3
echo ALL_DONE
