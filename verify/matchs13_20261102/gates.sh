#!/bin/bash
# MATCH-S13: default gates (merge_train.json gctrl/b arms) on the copied-out post-merge exe. Run inside mashed_lock.
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-S13
D=D:/MashedWT/MATCH-S13_data
G=${1:-g1}
mkdir -p $D/arms_$G
cd $W || exit 2
export A4_ROOT=D:/MashedWT/MATCH-S13_root A4_OUT=$D/arms_$G A4_EXE=$D/exe_$G/mashed_re.exe
sha256sum $A4_EXE
py -3.12 -W ignore verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
( cd $D/arms_$G && sha256sum H_DEF.step.csv H_DEF.gates.csv H_DEF.ptrace.log B_DEF_K0.step.csv B_DEF_K1.step.csv B_DEF_K26.step.csv )
