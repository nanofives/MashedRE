#!/bin/bash
# Original (b) reference runs per regime (d3_regimes_20261013). Extends
# verify/d3_bscore_20261009/capture_breg.sh: same argv (the regime-A reference recipe), cap raised
# from 8 to CAP (default 30), and it stops only when it has NEED_B fresh regime-B AND NEED_C fresh
# regime-C runs (default 2 + 2). Each run is labelled from its own aistep flag_a368 column
# (label_run.py); the poll's mode368 is logged as a second witness.
# MASHED hygiene: scenario_launch spawns its own MASHED; this script never kills by name.
# A run with no aistep csv (NO-GO, or a piz collision with another session) counts toward CAP only.
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/d3regimes
V=verify/d3_regimes_20261013
CAP=${CAP:-30}; NEED_B=${NEED_B:-2}; NEED_C=${NEED_C:-2}; START=${START:-1}
nb=0; nc=0
for ((i=START; i<START+CAP; i++)); do
  py -3.12 re/tools/orig_rampwatch.py $V/o_reg_r$i.poll.csv --hz 60 -- --track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots \
     --statediff-out $V/o_reg_r$i.msd --statediff-car 1 --statediff-aistep --hold 60 > $V/o_reg_r$i.log 2>&1
  py -3.12 $V/label_run.py $V/o_reg_r$i > $V/o_reg_r$i.label.txt 2>&1
  lab=$(tail -1 $V/o_reg_r$i.label.txt)
  av=$(grep -c "access violation\|0x1509b4\|0xe66db\|1509b4\|e66db" $V/o_reg_r$i.log)
  echo "run $i label=$lab av_lines=$av $(date -u +%H:%M:%SZ)" | tee -a $V/CAPTURE.txt
  [ "$lab" = "B" ] && nb=$((nb+1))
  [ "$lab" = "C" ] && nc=$((nc+1))
  if [ $nb -ge $NEED_B ] && [ $nc -ge $NEED_C ]; then echo "target met: B=$nb C=$nc" | tee -a $V/CAPTURE.txt; break; fi
done
echo "done: B=$nb C=$nc" | tee -a $V/CAPTURE.txt
