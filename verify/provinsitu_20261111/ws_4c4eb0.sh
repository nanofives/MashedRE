#!/bin/bash
# PROV-WRITESET-114: 0x004c4eb0 died with 10 stepped calls; try 1 and 3 (call inside the game lock, under test-slot)
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-WRITESET-114
for n in 1 3; do bash verify/provinsitu_20261111/run_ws.sh ws_4c4eb0_n$n 0x004c4eb0 $n; sleep 6; done
