# HB-RUN13-PHASE3-START arms (PREREG_HB-RUN13-PHASE3-START.md). Run under test-slot: test-slot bash -c 'bash run_arms.sh <arm>...'
set -x
W=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/HB-RUN13-PHASE3-START
D=D:/MashedWT/HB-RUN13-PHASE3-START_data
R=D:/MashedWT/HB-RUN13-PHASE3-START_root
EXE=$D/mashed_re_p3.exe
LOCK=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
CITE=$W/verify/rres_20261016/cites_rres.json
R13=$W/verify/rsky_20261016/run13.py
t13() {  # <name> [KNOB ...]
  n=$1; shift
  py -3.12 $LOCK run --label HB-RUN13-PHASE3-START-$n --wait 7000 -- py -3.12 $R13 run --exe $EXE --root $R --out $D/tracks_$n "$@"
  py -3.12 $R13 score --out $D/tracks_$n --cite $CITE > $D/tracks_score_$n.txt 2>&1
  sha256sum $D/tracks_score_$n.txt
}
for a in "$@"; do
  case $a in
    A)  t13 A ;;
    A0) t13 A0 MASHED_CAM_REPLAY_P3=0 ;;
    B)  t13 B MASHED_SUBSTATE MASHED_P3_RESET_PAIR ;;
    B1) t13 B1 MASHED_SUBSTATE ;;
    gates)
      export A4_EXE=$EXE A4_ROOT=$R A4_OUT=$D/arms; mkdir -p $D/arms
      py -3.12 $LOCK run --label HB-RUN13-PHASE3-START-gates --wait 7000 -- py -3.12 $W/verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
      sha256sum $D/arms/H_DEF.step.csv $D/arms/H_DEF.gates.csv $D/arms/H_DEF.ptrace.log $D/arms/B_DEF_K0.step.csv $D/arms/B_DEF_K1.step.csv $D/arms/B_DEF_K26.step.csv ;;
  esac
  echo "ARMDONE $a"
done
echo ALLDONE
