#!/bin/bash
# PROV-WRITESET-114: re-run of single bodies whose run failed to attach (call inside the game lock, under test-slot)
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PROV-WRITESET-114
for r in "$@"; do bash verify/provinsitu_20261111/run_ws.sh ws_s_$r $r 10; sleep 6; done
