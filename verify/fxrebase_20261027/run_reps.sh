#!/bin/bash
# D4-FX-REBASE: original reps on the port's recipe (--poke-ctrl-slots --statediff-out, no pulses).
# usage: bash verify/fxrebase_20261027/run_reps.sh <rep> [<rep> ...]   (run from the worktree root)
WT=$(pwd)
DATA=D:/MashedWT/D4-FX-REBASE_data
export MASHED_ROOT=D:/MashedWT/D4-FX-REBASE_root
for R in "$@"; do
  py -3.12 C:/Users/maria/Desktop/Proyectos/.claude/scripts/testslot/proyectos_test_slot.py \
    py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label D4-FX-REBASE -- \
    py -3.12 "$WT/verify/fxrebase_20261027/orig_fx_rebase.py" 240 "$DATA/$R" --poke-ctrl-slots --statediff-out "$DATA/$R.msd" \
    2>&1 | tee "$DATA/${R}_run.txt"
done
