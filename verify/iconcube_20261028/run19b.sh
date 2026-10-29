set -x
W=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D5-ICONCUBE-SPIN/verify/iconcube_20261028
D=D:/MashedWT/D5-ICONCUBE-SPIN_data
EXE=$D/mashed_re_r19b.exe
TR=C:/Users/maria/Desktop/Proyectos/Mashed/log/d1r1/orig_training/trace.csv
py -3.12 $W/run_replay3d.py $EXE $D/r19/R5 --trace $TR --shots 884,924,959 --track Training --frames 1450 MASHED_ICONCUBE MASHED_PICKUPS_PREGO MASHED_CAM_REPLAY_GO MASHED_PKCB_TRACE=$D/r19/R5_pkcb.tsv
echo REPLAYDONE_B
export A4_EXE=$EXE A4_ROOT=D:/MashedWT/D5-ICONCUBE-SPIN_root A4_OUT=$D/arms19b
mkdir -p $D/arms19b
py -3.12 $W/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PG B_PG_K0 B_PG_K1 B_PG_K26
echo GATESDONE_B
