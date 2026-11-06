#!/bin/bash
# MATCH-PORTSTATE-GAMEFLOW-FU: default gates (DEF) and knob-ON gates (PSGF = COURSE+RECORDS+GAMEFLOW+FU) on the
# copied-out exe rebuilt at 6f383c743 (merge of orch/integration, ledger-only).
set -e
W=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-PORTSTATE-GAMEFLOW-FU
D=D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW-FU_data
mkdir -p $D/exe_g2 $D/arms_g2
cp $D/mashed_re_6f383c743.exe $D/exe_g2/mashed_re.exe
cd $W
export A4_ROOT=D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW-FU_root A4_OUT=$D/arms_g2 A4_EXE=$D/exe_g2/mashed_re.exe
py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --label MATCH-PORTSTATE-GAMEFLOW-FU -- py -3.12 -W ignore verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSGF B_PSGF_K0 B_PSGF_K1 B_PSGF_K26
cd $D/arms_g2; sha256sum *.step.csv *.gates.csv *.ptrace.log 2>/dev/null
grep -H "portstate-gameflow" *.log || echo "no portstate-gameflow lines"
