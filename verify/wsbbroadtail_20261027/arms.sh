#!/bin/sh
# D4-WSB-C4-BROAD-TAIL arms exactly as the PREREG lists them (run from the worktree root, under test-slot).
export MASHED_ROOT=D:/MashedWT/D4-WSB-C4-BROAD-TAIL_root PYTHONIOENCODING=utf-8
D=verify/wsbbroadtail_20261027
echo "=== R1 racediff Quick Battle ==="
C4_TAILJMP=1 C4_DEREF=1 C4_ARGW='565550:1:ffff' py -3.12 $D/run_tail.py 0055ae50
if py -3.12 -c "import sys;r=[l.split(chr(9)) for l in open('$D/racediff_summary.tsv',encoding='utf-8')][1:];r=[x for x in r if x[0]=='0x0055ae50'];sys.exit(0 if r and r[0][6]=='0' and r[0][7]=='0' else 1)"; then
  echo "=== R2 racediff census (R1 saw 0 calls) ==="
  C4_TAILJMP=1 C4_DEREF=1 C4_ARGW='565550:1:ffff' C4_SCENARIO=census C4_HOLD=400 C4_WINDOW='300 30000' py -3.12 $D/run_tail.py 0055ae50
fi
echo "=== H heap-tracked write set ==="
SHADOW_TAG=heap MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1 py -3.12 $D/run_shadow_tail.py 0055ae50
SHADOW_TAG=heap py -3.12 $D/heap_pages.py 0055ae50
echo "=== G gates ==="
A4_ROOT=$MASHED_ROOT A4_OUT=D:/MashedWT/D4-WSB-C4-BROAD-TAIL_data/gates py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label D4-WSB-C4-BROAD-TAIL -- py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
echo "=== done ==="
