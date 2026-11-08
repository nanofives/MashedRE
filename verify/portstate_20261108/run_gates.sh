#!/usr/bin/env bash
# PORT-STATE-02 P1/P2 (G-CTRL + B, OFF and ON), P4 (D2 x3 OFF and ON, solo), P5 (13 tracks OFF and ON).
# Caller wraps this in test-slot. Frozen exe only (A4_EXE). Spawned PIDs are stopped only by the runners themselves.
set -u
cd "$(dirname "$0")/../.."
ORCH=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration
D=D:/MashedWT/PORT-STATE-02_data
export A4_EXE=$D/exe/mashed_re.exe A4_ROOT=D:/MashedWT/PORT-STATE-02_root A4_OUT=$D/arms
mkdir -p $A4_OUT
L="py -3.12 $ORCH/re/orchestrator/mashed_lock.py run --label PORT-STATE-02"
$L -- py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSS B_PSS_K0 B_PSS_K1 B_PSS_K26
echo GATES-DONE
$L --solo --ttl 3600 -- py -3.12 verify/savelink_20261023/run_arms.py D2_DEFH_1 D2_DEFH_2 D2_DEFH_3 D2_DEFHPSS_1 D2_DEFHPSS_2 D2_DEFHPSS_3
echo D2-DONE
$L -- py -3.12 verify/rsky_20261016/run13.py run --exe $A4_EXE --root D:/MashedWT/PORT-STATE-02_root13 --out $D/t13_off
$L -- py -3.12 verify/rsky_20261016/run13.py run --exe $A4_EXE --root D:/MashedWT/PORT-STATE-02_root13 --out $D/t13_on MASHED_PORTSTATE_SEED=1
echo T13-DONE
