# D5-PREGO-CUBE-FLIP controls: 13 tracks with MASHED_NO_ICONCUBE (PREGO stays default ON), D2 arm re-run (arm 2 had 1530 lines).
set -x
W=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D5-PREGO-CUBE-FLIP
V=$W/verify/pregocubeflip_20261029
D=D:/MashedWT/D5-PREGO-CUBE-FLIP_data
R=D:/MashedWT/D5-PREGO-CUBE-FLIP_root
LOCK=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
export A4_EXE=$D/mashed_re_flip.exe A4_ROOT=$R A4_OUT=$D/arms
py -3.12 $LOCK run --solo --label D5-PREGO-CUBE-FLIP-D2b --wait 7000 -- py -3.12 $V/run_arms.py D2_DEF_4
py -3.12 $W/verify/d2rebase_20261019/score_d2_v2.py port $D/arms/D2_DEF_4.motion_diag.log | tee $D/score_d2_rerun.txt
wc -l $D/arms/D2_DEF_4.motion_diag.log
echo D2BDONE
py -3.12 $LOCK run --solo --label D5-PREGO-CUBE-FLIP-13noic --wait 7000 -- py -3.12 $W/verify/rsky_20261016/run13.py run --exe $D/mashed_re_flip.exe --root $R --out $D/tracks_noic MASHED_NO_ICONCUBE
py -3.12 $W/verify/rsky_20261016/run13.py score --out $D/tracks_noic --cite $W/verify/rres_20261016/cites_rres.json > $D/tracks_score_noic.txt 2>&1
echo CTLDONE
