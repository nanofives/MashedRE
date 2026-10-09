#!/bin/bash
# PREREG_BREGIME.md §2: original runs one at a time, stop at the first regime-B run, cap 8.
cd /c/Users/maria/Desktop/Proyectos/Mashed
V=verify/d3_bscore_20261009
for i in 1 2 3 4 5 6 7 8; do
  py -3.12 re/tools/orig_rampwatch.py $V/o_breg_r$i.poll.csv --hz 60 -- --track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots \
     --statediff-out $V/o_breg_r$i.msd --statediff-car 1 --statediff-aistep --hold 60 > $V/o_breg_r$i.log 2>&1
  lab=$(py -3.12 $V/label_regime.py $V/o_breg_r$i.poll.csv | tail -1)
  ev=$(py -3.12 $V/label_regime.py $V/o_breg_r$i.poll.csv | head -1)
  echo "run $i label=$lab  ($ev)"
  if [ "$lab" = "B" ]; then break; fi
done
