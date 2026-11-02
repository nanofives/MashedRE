#!/usr/bin/env bash
# D4-DC-77b0-ST3-PRETICK: every arm of the PREREG in one lock window. Run from the worktree root, under
#   py -3.12 re/orchestrator/mashed_lock.py run --label D4-DC-77b0-ST3-PRETICK -- bash verify/d4dc77b0st3pretick_20261101/run_all.sh
set -u
V=verify/d4dc77b0st3pretick_20261101
ROOT='D:\MashedWT\D4-DC-77b0-ST3-PRETICK_root'
DATA='D:/MashedWT/D4-DC-77b0-ST3-PRETICK_data'
EXE="$(pwd -W)/mashedmod/build/mashed_re.exe"
export A4_OUT="$DATA/arms" A4_ROOT="$ROOT" A4_EXE="$EXE"
mkdir -p "$DATA/arms"
probe() {   # probe <name> [--env K=V ...]
  local name=$1; shift
  rm -f "/d/MashedWT/D4-DC-77b0-ST3-PRETICK_root/mashed_re.log"
  py -3.12 $V/exe_vsprobe.py --exe "$EXE" --root "$ROOT" --out "$DATA/$name" "$@" 2>&1 | tee "$DATA/$name.log"
  cp "/d/MashedWT/D4-DC-77b0-ST3-PRETICK_root/mashed_re.log" "$DATA/$name.mashed_re.log" 2>/dev/null
  grep -a -E "\[leg2\] phase 3 released|\[st3pretick\]" "$DATA/$name.mashed_re.log"
}
probe x_off
probe x_st3 --env MASHED_ST3_PRETICK=1
py -3.12 $V/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_ST3 B_ST3_K0 B_ST3_K1 B_ST3_K26 2>&1 | tee "$DATA/arms.log"
