#!/bin/bash
# MATCH-S06-FU runtime arms: M (measure log, baseline vs branch exe) then G0 (default gates on the branch exe).
# Run inside test-slot; each boot set holds the machine lock (mashed_lock.py run --label MATCH-S06-FU).
set -u
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-S06-FU
LOCK=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
D='D:/MashedWT/MATCH-S06-FU_data'
R='D:/MashedWT/MATCH-S06-FU_root'
cd "$W"
mkdir -p "$D/arms_g1" "$D/measure"
ARMS="${1:-M G0}"
if [[ " $ARMS " == *" M "* ]]; then
echo "=== arm M ==="
py -3.12 "$LOCK" run --label MATCH-S06-FU -- py -3.12 verify/matchs06fu_20261102/tools/run_measure.py "$R" "$D/measure" \
  base="$D/exe_base/mashed_re.exe" g1="$D/exe_g1/mashed_re.exe"
echo "arm M exit=$?"
fi
echo "=== arm G0 ==="
A4_OUT="$D/arms_g1" A4_EXE="$D/exe_g1/mashed_re.exe" A4_ROOT="$R" \
  py -3.12 "$LOCK" run --label MATCH-S06-FU -- py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
echo "arm G0 exit=$?"
for f in "$D"/arms_g1/H_DEF.step.csv "$D"/arms_g1/H_DEF.gates.csv "$D"/arms_g1/H_DEF.ptrace.log "$D"/arms_g1/B_DEF_K0.step.csv "$D"/arms_g1/B_DEF_K1.step.csv "$D"/arms_g1/B_DEF_K26.step.csv; do
  [ -f "$f" ] && sha256sum "$f" || echo "MISSING $f"
done
