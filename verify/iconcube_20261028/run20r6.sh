set -x
W=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D5-ICONCUBE-SPIN/verify/iconcube_20261028
D=D:/MashedWT/D5-ICONCUBE-SPIN_data
TR=C:/Users/maria/Desktop/Proyectos/Mashed/log/d1r1/orig_training/trace.csv
py -3.12 $W/run_replay3d.py $D/mashed_re_r20.exe $D/r20/R6 --trace $TR --shots 884,924,959 --track Training --frames 1450 MASHED_ICONCUBE MASHED_PICKUPS_PREGO MASHED_CAM_REPLAY_GO MASHED_PKCB_TRACE=$D/r20/R6_pkcb.tsv
echo R6DONE
