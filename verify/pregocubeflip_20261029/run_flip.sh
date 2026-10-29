# D5-PREGO-CUBE-FLIP gates on the flipped default, NO knob env var set (arms DEF), plus the opt-out control (NOPGIC).
set -x
W=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D5-PREGO-CUBE-FLIP
V=$W/verify/pregocubeflip_20261029
D=D:/MashedWT/D5-PREGO-CUBE-FLIP_data
R=D:/MashedWT/D5-PREGO-CUBE-FLIP_root
LOCK=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
export A4_EXE=$D/mashed_re_flip.exe A4_ROOT=$R A4_OUT=$D/arms
mkdir -p $D/arms
py -3.12 $LOCK run --label D5-PREGO-CUBE-FLIP --wait 7000 -- py -3.12 $V/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_NOPGIC B_NOPGIC_K0 B_NOPGIC_K1 B_NOPGIC_K26
echo GATESDONE
py -3.12 $LOCK run --solo --label D5-PREGO-CUBE-FLIP-D2 --wait 7000 -- py -3.12 $V/run_arms.py D2_DEF_1 D2_DEF_2 D2_DEF_3
py -3.12 $W/verify/d2rebase_20261019/score_d2_v2.py port $D/arms/D2_DEF_1.motion_diag.log $D/arms/D2_DEF_2.motion_diag.log $D/arms/D2_DEF_3.motion_diag.log | tee $D/score_d2.txt
wc -l $D/arms/D2_DEF_*.motion_diag.log
echo D2DONE
py -3.12 $LOCK run --solo --label D5-PREGO-CUBE-FLIP-13 --wait 7000 -- py -3.12 $W/verify/rsky_20261016/run13.py run --exe $D/mashed_re_flip.exe --root $R --out $D/tracks
py -3.12 $W/verify/rsky_20261016/run13.py score --out $D/tracks --cite $W/verify/rres_20261016/cites_rres.json > $D/tracks_score.txt 2>&1
echo TRACKSDONE
