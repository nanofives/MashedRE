# D5-PREGO-CUBE-FLIP diagnostic: the four tracks with RED cube rows, flipped default + MASHED_CAM_REPLAY_GO (run 20 R6's alignment).
set -x
W=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D5-PREGO-CUBE-FLIP
D=D:/MashedWT/D5-PREGO-CUBE-FLIP_data
R=D:/MashedWT/D5-PREGO-CUBE-FLIP_root
LOCK=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
until grep -q CTLDONE $D/RUNLOG_ctl.txt; do sleep 20; done
py -3.12 $LOCK run --solo --label D5-PREGO-CUBE-FLIP-13go --wait 7000 -- py -3.12 $W/verify/rsky_20261016/run13.py run --exe $D/mashed_re_flip.exe --root $R --out $D/tracks_go --tracks 5,6,11,13 MASHED_CAM_REPLAY_GO
py -3.12 $W/verify/rsky_20261016/run13.py score --out $D/tracks_go --tracks 5,6,11,13 --cite $W/verify/rres_20261016/cites_rres.json > $D/tracks_score_go.txt 2>&1
echo GODONE
