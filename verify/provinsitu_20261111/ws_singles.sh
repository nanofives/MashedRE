#!/bin/bash
# PROV-WRITESET-114: one body per run for the bodies of the groups that died (call inside the game lock, under test-slot)
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-WRITESET-114
for g in ${GROUPS_RUN:-2b 3b}; do
  for r in $(tr ',' ' ' < verify/provinsitu_20261111/ws_group$g.txt); do
    bash verify/provinsitu_20261111/run_ws.sh ws_s_$r $r 10
    sleep 6
  done
done
