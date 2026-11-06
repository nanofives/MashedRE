#!/bin/bash
# MATCH-T13-SEED: default gates on the copied-out knob-OFF exe, then the port seed arms. Run inside mashed_lock.
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-T13-SEED
D=D:/MashedWT/MATCH-T13-SEED_data
mkdir -p $D/arms_g1
cd $W || exit 2
export A4_ROOT=D:/MashedWT/MATCH-T13-SEED_root A4_OUT=$D/arms_g1 A4_EXE=$D/exe_g1/mashed_re.exe
py -3.12 -W ignore verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
( cd $D/arms_g1 && sha256sum H_DEF.step.csv H_DEF.gates.csv H_DEF.ptrace.log B_DEF_K0.step.csv B_DEF_K1.step.csv B_DEF_K26.step.csv )
py -3.12 $D/p_arms.py P_OFF_SANDS P_ON_SANDS P_OFF_CITY P_ON_CITY P_OFF_NEU P_ON_NEU P_OFF_TRAIN P_ON_TRAIN
