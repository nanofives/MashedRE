#!/bin/bash
# MATCH-T13-SEED original-side arms (PREREG verify/t13seed_20261102/PREREG_MATCH-T13-SEED.md).
# Usage: bash o_arms.sh NAME:TRACK [NAME:TRACK ...]
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-T13-SEED
D=/d/MashedWT/MATCH-T13-SEED_data
export MASHED_ROOT='D:\MashedWT\MATCH-T13-SEED_root'
cd "$W" || exit 2
for spec in "$@"; do
  name=${spec%%:*}; trk=${spec##*:}
  echo "=== $name track=$trk $(date +%T)"
  rm -f log/rules_oracle_rule10.json
  py -3.12 re/frida/scenario_launch.py --track "$trk" --mode 3 --cars 4 --rule 10 --oracle --hold 40 > "$D/$name.run.txt" 2>&1
  echo "    rc=$? $(date +%T)"
  if [ -f log/rules_oracle_rule10.json ]; then cp log/rules_oracle_rule10.json "$D/$name.oracle.json"; fi
  grep -a "seed FUN_004046a0\|ORACLE VERDICT\|0x1509b4\|0xe66db\|Traceback" "$D/$name.run.txt" | cut -c1-400
done
