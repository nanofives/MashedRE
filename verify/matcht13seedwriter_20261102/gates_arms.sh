#!/bin/bash
# MATCH-T13-SEED-WRITER: default gates (knob unset) on the copied-out exe, then the L / C arms. Run inside mashed_lock.
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-T13-SEED-WRITER
D=D:/MashedWT/MATCH-T13-SEED-WRITER_data
mkdir -p $D/arms_g1
cd $W || exit 2
export A4_ROOT=D:/MashedWT/MATCH-T13-SEED-WRITER_root A4_OUT=$D/arms_g1 A4_EXE=$D/exe/mashed_re.exe
py -3.12 -W ignore verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
( cd $D/arms_g1 && sha256sum H_DEF.step.csv H_DEF.gates.csv H_DEF.ptrace.log B_DEF_K0.step.csv B_DEF_K1.step.csv B_DEF_K26.step.csv )
py -3.12 verify/matcht13seedwriter_20261102/l_arms.py L_SANDS L_NEU L_CITY C_SANDS C_NEU C_CITY
