set -u
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PORT-STATE-02
ORCH=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration
EXE=D:/MashedWT/PORT-STATE-02_data/exe/mashed_re.exe
EXEE=D:/MashedWT/PORT-STATE-02_data/exeE/mashed_re.exe
R=D:/MashedWT/PORT-STATE-02_root
L="py -3.12 $ORCH/re/orchestrator/mashed_lock.py run --label PORT-STATE-02 --"
S="py -3.12 verify/portstate_20261108/run_seed.py"
BM="MASHED_DBG_MENU=1 MASHED_NAV_KEYS=x"
$L $S $EXE  P8b_D_keep     $R $BM
$L $S $EXE  P8b_D_absent   $R $BM --cfg absent
$L $S $EXEE P8b_E_ON_keep  $R $BM MASHED_PORTSTATE_SEED=1
$L $S $EXEE P8b_E_ON_absent $R $BM MASHED_PORTSTATE_SEED=1 --cfg absent
$L $S $EXEE P8b_E_OFF_keep $R $BM --timeout 120
cp D:/MashedWT/PORT-STATE-02_data/videocfg_D_firstrun.bin $R/videocfg.bin
echo P8B-DONE
