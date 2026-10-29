set -x
W=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D5-ICONCUBE-SPIN/verify/iconcube_20261028
D=D:/MashedWT/D5-ICONCUBE-SPIN_data
export A4_EXE=$D/mashed_re_r20.exe A4_ROOT=D:/MashedWT/D5-ICONCUBE-SPIN_root A4_OUT=$D/arms20
mkdir -p $D/arms20
py -3.12 $W/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PG B_PG_K0 B_PG_K1 B_PG_K26 H_PGNR B_PGNR_K0 B_PGNR_K1 B_PGNR_K26 H_PGRD H_RDT
echo GATESDONE20
