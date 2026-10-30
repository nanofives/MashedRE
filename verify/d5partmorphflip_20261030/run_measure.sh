# D5-PARTMORPH-MEASURE (L-62 b): the merge train's Gate-Default + Gate-Full on the MASHED_FLIP_D11077=1 exe.
# Same commands as re/orchestrator/merge_train.ps1 (Run-Arms / Gate-Default / Run-D2Arms / Gate-Full), private roots.
# Run under test-slot: test-slot bash -c 'bash run_measure.sh gates d2 run13'
set -x
W=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/D5-PARTMORPH-MEASURE
D=D:/MashedWT/D5-PARTMORPH-MEASURE_data
R=D:/MashedWT/D5-PARTMORPH-MEASURE_root
R13=D:/MashedWT/D5-PARTMORPH-MEASURE_root13
EXE=$D/mashed_re_on.exe
LOCK=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
RUNNER=$W/verify/savelink_20261023/run_arms.py
export A4_EXE=$EXE A4_ROOT=$R A4_OUT=$D/arms
mkdir -p $D/arms
arms() {  # [--solo] <arm>...
  s=; if [ "$1" = --solo ]; then s=--solo; shift; fi
  py -3.12 $LOCK run --label D5-PARTMORPH-MEASURE --wait 7200 $s -- py -3.12 $RUNNER "$@"
}
short_d2() {  # arms whose motion_diag is under 1600 lines
  for a in D2_DEFH_1 D2_DEFH_2 D2_DEFH_3; do
    f=$D/arms/$a.motion_diag.log
    if [ ! -f $f ] || [ $(wc -l < $f) -lt 1600 ]; then echo $a; fi
  done
}
for st in "$@"; do
  case $st in
    gates)
      arms H_DEF
      arms B_DEF_K0 B_DEF_K1 B_DEF_K26
      sha256sum $D/arms/H_DEF.step.csv $D/arms/H_DEF.gates.csv $D/arms/H_DEF.ptrace.log $D/arms/B_DEF_K0.step.csv $D/arms/B_DEF_K1.step.csv $D/arms/B_DEF_K26.step.csv | tee $D/gates_sha.txt
      py -3.12 $W/verify/savelink_20261023/vsum.py $D/arms/B_DEF_K0.step.csv $D/arms/B_DEF_K1.step.csv $D/arms/B_DEF_K26.step.csv > $D/vsum_b.txt 2>&1
      py -3.12 $W/re/tools/ai_speed_env.py --check $D/arms/B_DEF_K0.step.csv > $D/e_check.txt 2>&1 ;;
    d2)
      arms --solo D2_DEFH_1 D2_DEFH_2 D2_DEFH_3
      for i in 1 2; do
        s=$(short_d2); [ -z "$s" ] && break
        echo "D2 rerun $i: $s" | tee -a $D/d2_reruns.txt
        arms --solo $s
      done
      echo "short after reruns: $(short_d2)" | tee -a $D/d2_reruns.txt
      wc -l $D/arms/D2_DEFH_*.motion_diag.log | tee -a $D/d2_reruns.txt
      py -3.12 $W/verify/d2band42_20261029/score_d2_v2.py port $D/arms/D2_DEFH_1.motion_diag.log $D/arms/D2_DEFH_2.motion_diag.log $D/arms/D2_DEFH_3.motion_diag.log > $D/score_d2.txt 2>&1 ;;
    run13)
      tok=$(py -3.12 $LOCK solo-begin --label D5-PARTMORPH-MEASURE-13 --ttl 14400 --owner-pid $$ | sed -n 's/^TOKEN //p')
      export MASHED_LOCK_SOLO_TOKEN=$tok
      py -3.12 $W/verify/rsky_20261016/run13.py run --exe $EXE --root $R13 --out $D/tracks
      py -3.12 $LOCK solo-end --token $tok
      unset MASHED_LOCK_SOLO_TOKEN
      py -3.12 $W/verify/rsky_20261016/run13.py score --out $D/tracks --cite $W/verify/rres_20261016/cites_rres.json > $D/tracks_score.txt 2>&1
      sha256sum $D/tracks_score.txt $W/verify/run13p3_20261029/tracks_score_sub_p3.txt
      diff <(grep -E '^[0-9][0-9]_' $W/verify/run13p3_20261029/tracks_score_sub_p3.txt) <(grep -E '^[0-9][0-9]_' $D/tracks_score.txt) > $D/tracks_rowdiff.txt; echo "rowdiff rc=$?"
      grep 'INSTRUMENT GATES' $D/tracks_score.txt ;;
  esac
  echo "STEPDONE $st"
done
echo ALLDONE
