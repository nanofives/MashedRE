#!/usr/bin/env bash
# HB-SHADOW-ALLOC-COMPARE arm G: default gates on this build's exe, solo window (BROAD-2 run_pass3.sh shape).
cd "$(dirname "$0")/../.."
W=$(pwd)
export A4_EXE=$W/mashedmod/build/mashed_re.exe A4_ROOT=D:/MashedWT/HB-SHADOW-ALLOC-COMPARE_root A4_OUT=D:/MashedWT/HB-SHADOW-ALLOC-COMPARE_data/gates
py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --solo --label HB-SHADOW-ALLOC-COMPARE --wait 43200 -- py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
