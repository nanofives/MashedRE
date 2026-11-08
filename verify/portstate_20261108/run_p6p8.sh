#!/usr/bin/env bash
# PORT-STATE-02 arms P6 (boot-to-menu), P7 (one Quick Battle), P8 (lane-A retry, experiment exe E).
# Each boot: test-slot outside (the caller), mashed_lock inside. Route recorded in the RESULT:
#   P6: MASHED_DBG_MENU=1 MASHED_NAV_KEYS=x            (menu root, one no-op step, quit)
#   P7: MASHED_DBG_MENU=1 MASHED_NAV_KEYS=e,d,e,e,e,e,x*1200 GAP=60   (main -> 2 -> row 1 -> 4 -> 6 -> 24 -> race, ~20 s)
set -u
cd "$(dirname "$0")/../.."
ORCH=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration
EXE=D:/MashedWT/PORT-STATE-02_data/exe/mashed_re.exe
EXEE=D:/MashedWT/PORT-STATE-02_data/exeE/mashed_re.exe
R=D:/MashedWT/PORT-STATE-02_root
L="py -3.12 $ORCH/re/orchestrator/mashed_lock.py run --label PORT-STATE-02 --"
S="py -3.12 verify/portstate_20261108/run_seed.py"
QB="MASHED_DBG_MENU=1 MASHED_NAV_KEYS=e,d,e,e,e,e,x*1200 MASHED_NAV_KEYS_GAP=60"
BM="MASHED_DBG_MENU=1 MASHED_NAV_KEYS=x"
for rep in 1 2; do
  $L $S $EXE P6_OFF_$rep $R $BM
  $L $S $EXE P6_ON_$rep  $R $BM MASHED_PORTSTATE_SEED=1
  $L $S $EXE P7_OFF_$rep $R $QB --timeout 300
  $L $S $EXE P7_ON_$rep  $R $QB MASHED_PORTSTATE_SEED=1 --timeout 300
done
# P8: default exe first-run file (reference), then E OFF / ON with the file present, then E ON first-run
$L $S $EXE  P8_D_absent  $R $BM --cfg absent
cp $R/videocfg.bin D:/MashedWT/PORT-STATE-02_data/videocfg_D_firstrun.bin
$L $S $EXEE P8_E_OFF_keep $R $BM --timeout 120
$L $S $EXEE P8_E_ON_keep  $R $BM MASHED_PORTSTATE_SEED=1
$L $S $EXEE P8_E_ON_absent $R $BM MASHED_PORTSTATE_SEED=1 --cfg absent
$L $S $EXEE P8_E_OFF_absent $R $BM --cfg absent --timeout 120
# leave the root with the default first-run file again
cp D:/MashedWT/PORT-STATE-02_data/videocfg_D_firstrun.bin $R/videocfg.bin
echo P6P8-DONE
