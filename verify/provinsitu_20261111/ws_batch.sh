#!/bin/bash
# PROV-WRITESET-114: the 114 bodies in 6 groups of 19, first 10 calls of each single-stepped (call inside the game lock, under test-slot)
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-WRITESET-114
for g in ${GROUPS_RUN:-1a 1b 2a 2b 3a 3b}; do
  bash verify/provinsitu_20261111/run_ws.sh ws_g$g $(cat verify/provinsitu_20261111/ws_group$g.txt) 10
  sleep 8
done
