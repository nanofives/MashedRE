#!/usr/bin/env bash
CY=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/wip-cammanyaw
LOCK="py -3.12 $CY/re/orchestrator/mashed_lock.py run"
CYD=/d/MashedWT/D4-WIP-CAMMANYAW_data
ROOT='D:\MashedWT\D4-WIP-CAMMANYAW_root'
CYEXE='C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\wip-cammanyaw\mashedmod\build\mashed_re.exe'
BASEEXE='C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\wip-panelsort\mashedmod\build\mashed_re.exe'
M=3; N=0
arm() { N=$((N+1)); local a=$1 exe=$2; shift 2; echo "=== [$N/$M] $a START $(date +%T)"
  ( cd $CY && $LOCK --label D4-WIP-CAMMANYAW -- py -3.12 verify/rsky_20261016/run13.py run --exe "$exe" --root "$ROOT" \
      --out "D:\MashedWT\D4-WIP-CAMMANYAW_data\r13_$a" --tracks 3,13 "$@" ) 2>&1 | tee $CYD/r13_$a.txt
  local rc=${PIPESTATUS[0]}; local n=$(ls $CYD/r13_$a/*/stand/d1r2/*.draws.json 2>/dev/null | wc -l)
  echo "[$N/$M] $a $([ $rc = 0 ] && [ $n = 6 ] && echo PASS || echo "FAIL rc=$rc draws=$n") $(date +%T)"; }
arm yaw  $CYEXE MASHED_CAMMANYAW=1
arm off  $CYEXE
arm base $BASEEXE
echo "=== DRIVER DONE $(date +%T)"
