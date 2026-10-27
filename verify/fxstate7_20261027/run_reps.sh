#!/bin/bash
# D4-FX-STATE7: original reps on the D4-FX-REBASE recipe (--poke-ctrl-slots --statediff-out, no pulses).
# usage: bash verify/fxstate7_20261027/run_reps.sh <rep> [<rep> ...]   (run from the worktree root)
# a rep name starting with "p" adds --poke-cfc (orig_fx_state7.py: rec+0xcfc = 1 on all cars at the first state-7 frame)
WT=$(pwd)
DATA=D:/MashedWT/D4-FX-STATE7_data
export MASHED_ROOT=D:/MashedWT/D4-FX-STATE7_root
for R in "$@"; do
  POKE=""; case "$R" in p*) POKE="--poke-cfc";; esac
  py -3.12 C:/Users/maria/Desktop/Proyectos/.claude/scripts/testslot/proyectos_test_slot.py \
    py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label D4-FX-STATE7 -- \
    py -3.12 "$WT/verify/fxstate7_20261027/orig_fx_state7.py" 240 "$DATA/$R" $POKE --poke-ctrl-slots --statediff-out "$DATA/$R.msd" \
    2>&1 | tee "$DATA/${R}_run.txt"
done
