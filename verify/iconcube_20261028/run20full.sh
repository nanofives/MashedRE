set -x
W=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D5-ICONCUBE-SPIN
V=$W/verify/iconcube_20261028
D=D:/MashedWT/D5-ICONCUBE-SPIN_data
LOCK=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
export A4_EXE=$D/mashed_re_r20.exe A4_ROOT=D:/MashedWT/D5-ICONCUBE-SPIN_root A4_OUT=$D/arms20
py -3.12 $LOCK run --solo --label D5-ICONCUBE-SPIN-D2 --wait 7000 -- py -3.12 $V/run_arms.py D2_PG_1 D2_PG_2 D2_PG_3
py -3.12 $W/verify/d2rebase_20261019/score_d2_v2.py port $D/arms20/D2_PG_1.motion_diag.log $D/arms20/D2_PG_2.motion_diag.log $D/arms20/D2_PG_3.motion_diag.log | tee $D/score_d2_pg.txt
wc -l $D/arms20/D2_PG_*.motion_diag.log
echo D2DONE20
py -3.12 $LOCK run --solo --label D5-ICONCUBE-SPIN-13 --wait 7000 -- py -3.12 $W/verify/rsky_20261016/run13.py run --exe $D/mashed_re_r20.exe --root D:/MashedWT/D5-ICONCUBE-SPIN_root --out $D/tracks_pg MASHED_PICKUPS_PREGO
py -3.12 $W/verify/rsky_20261016/run13.py score --out $D/tracks_pg --cite $W/verify/rres_20261016/cites_rres.json > $D/tracks_score_pg.txt 2>&1
echo TRACKSDONE20
