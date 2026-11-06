#!/usr/bin/env bash
# D4-FLIP-MATCHEND-ST3PRETICK: the four default gate arms on the flipped exe (MASHED_MATCHEND_SUBSTATE + MASHED_ST3_PRETICK
# default ON). Run from the worktree root, under
#   py -3.12 re/orchestrator/mashed_lock.py run --label D4-FLIP-MATCHEND-ST3PRETICK -- bash verify/d4flip_matchend_st3pretick_20261102/run_all.sh
set -u
V=verify/d4flip_matchend_st3pretick_20261102
ROOT='D:\MashedWT\D4-FLIP-MATCHEND-ST3PRETICK_root'
DATA='D:/MashedWT/D4-FLIP-MATCHEND-ST3PRETICK_data'
EXE="$(pwd -W)/mashedmod/build/mashed_re.exe"
export A4_OUT="$DATA/arms" A4_ROOT="$ROOT" A4_EXE="$EXE"
mkdir -p "$DATA/arms"
py -3.12 $V/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 2>&1 | tee "$DATA/arms.log"
