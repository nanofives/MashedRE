#!/usr/bin/env bash
# SOAK-P0 run chain (post-reboot). One game at a time: test-slot OUTSIDE, mashed_lock INSIDE, private root.
# bash verify/soakp0_20261114/run_chain.sh [step ...]   (default: u c e d d1)
WT=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/SOAK-P0
OI=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration
DATA=D:/MashedWT/SOAK-P0_data
export MASHED_ROOT='D:\MashedWT\SOAK-P0_root'
export PROYECTOS_TEST_HUB_VERBOSE=1
LOCK="py -3.12 $OI/re/orchestrator/mashed_lock.py run --label SOAK-P0 --"
# car 0..3 position (record 0x008815a0 + k*0xd04, +0x958..+0x963) and the in-race UI word 0x0063ba8c, raw per tick
RAW=0x0063ba8c-0x0063ba90,0x00881ef8-0x00881f04,0x00882bfc-0x00882c08,0x00883900-0x0088390c,0x00884604-0x00884610

probe() {   # label scen timeout
  mkdir -p "$DATA/$1"
  echo "[run] $1 start $(date +%H:%M:%S)"
  test-slot $LOCK py -3.12 "$WT/re/frida/detorig.py" --label "$1" --out "$DATA/$1" --boot --clock tick --warp-tick 600 \
    --tick-cap 16 --scen "$WT/verify/soakp0_20261114/scen/$2.json" --probe --probe-stop-results --probe-after-end 90 \
    --timeout "$3" --crash-catch 30 2>&1 | tee "$DATA/$1.log" | grep --line-buffered -E "test-hub|test-slot|mashed-lock|match end|=== rc|exited|CRASH|rror"
}

steps=("$@"); [ ${#steps[@]} -eq 0 ] && steps=(u c e d d1)
for s in "${steps[@]}"; do
  case $s in
    u)   # UNEDITED harness (orch-integration's detorig.py, default SCENARIO and INPUT: slot 0 state 1, control 4 held)
      mkdir -p "$DATA/u_unedited"; echo "[run] u_unedited start $(date +%H:%M:%S)"
      test-slot $LOCK py -3.12 "$OI/re/frida/detorig.py" --label u_unedited --out "$DATA/u_unedited" --boot --clock tick \
        --warp-tick 600 --tick-cap 3000 --tick-raw "$RAW" --timeout 400 --crash-catch 30 2>&1 \
        | tee "$DATA/u_unedited.log" | grep --line-buffered -E "test-hub|test-slot|mashed-lock|race anchor|ticks |=== rc|exited|CRASH|rror" ;;
    c)  probe c_ai4_pulse4 c_ai4_pulse4 420 ;;
    e)  probe e_human_pulse4 e_human_pulse4 420 ;;
    d)  probe d_timetrial d_timetrial 300 ;;
    d1) probe d1_tt_human d1_tt_human 300 ;;
    a2) probe a2_base_cin a_base 150 ;;     # probe with the cooked-input columns (cin)
    b2) probe b2_ai4_cin b_ai4 150 ;;
  esac
done
echo "[chain] done $(date +%H:%M:%S)"
