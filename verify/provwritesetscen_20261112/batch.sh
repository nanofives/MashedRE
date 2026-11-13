#!/bin/bash
# PROV-WRITESET-SCENARIO: one game per group (groups.txt), test-slot outside, mashed_lock inside.
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-WRITESET-SCENARIO
V=verify/provwritesetscen_20261112
while read g rv; do
  [ -n "$ONLY" ] && ! echo " $ONLY " | grep -q " $g " && continue
  test-slot bash -c "py -3.12 re/orchestrator/mashed_lock.py run --label PROV-WRITESET-SCENARIO -- bash $V/run_wss.sh ${PFX:-wsa}_$g $rv"
  sleep 5
done < ${GROUPS_FILE:-$V/groups.txt}
