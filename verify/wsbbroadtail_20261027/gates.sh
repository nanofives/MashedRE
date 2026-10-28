#!/bin/sh
# D4-WSB-C4-BROAD-TAIL arm G re-run (first try: A4_OUT folder missing, run_arms.py copy failed after H_DEF). Under test-slot.
export MASHED_ROOT=D:/MashedWT/D4-WSB-C4-BROAD-TAIL_root PYTHONIOENCODING=utf-8
mkdir -p D:/MashedWT/D4-WSB-C4-BROAD-TAIL_data/gates
A4_ROOT=$MASHED_ROOT A4_OUT=D:/MashedWT/D4-WSB-C4-BROAD-TAIL_data/gates py -3.12 -W ignore C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label D4-WSB-C4-BROAD-TAIL -- py -3.12 -W ignore verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
echo "=== gates done ==="
