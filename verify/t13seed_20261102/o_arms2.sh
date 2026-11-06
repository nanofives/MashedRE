#!/bin/bash
# MATCH-T13-SEED amendment-1 original arms. spec NAME:TRACK:MODE:INIT
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-T13-SEED
D=/d/MashedWT/MATCH-T13-SEED_data
export MASHED_ROOT='D:\MashedWT\MATCH-T13-SEED_root'
cd "$W" || exit 2
for spec in "$@"; do
  IFS=: read name trk mode init <<< "$spec"
  echo "=== $name track=$trk mode=$mode init=$init $(date +%T)"
  rm -f log/rules_oracle_rule10.json
  if [ "$init" = 1 ]; then export T13SEED_INIT_DIFF_TABLE=1; else unset T13SEED_INIT_DIFF_TABLE; fi
  py -3.12 re/frida/scenario_launch.py --track "$trk" --mode "$mode" --cars 4 --rule 10 --oracle --hold 40 > "$D/$name.run.txt" 2>&1
  echo "    rc=$? $(date +%T)"
  if [ -f log/rules_oracle_rule10.json ]; then cp log/rules_oracle_rule10.json "$D/$name.oracle.json"; fi
  grep -a "init-diff-table\|ORACLE VERDICT\|0x1509b4\|0xe66db\|Traceback\|RACE RUNNING" "$D/$name.run.txt" | cut -c1-300
done
