#!/usr/bin/env bash
# DDKOBJ-42AD90 boots. Each launch inside mashed_lock (test-slot outside: run this script under `test-slot bash`).
#   1. guarded solo (only 0x0042ad90, adapter route) knob OFF and ON, 40 s (knob_boot.py, the bisect's rule)
#   2. reach runs on the full object build, knob OFF and ON, 40 s (regconv_reach.py run)
set -u
cd "$(dirname "$0")/../.."
LOCK="py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --label DDKOBJ-42AD90 --"
ROOT=D:/MashedWT/DDKOBJ-42AD90_root
DATA=D:/MashedWT/DDKOBJ-42AD90_data
reset() { cp D:/MashedWT/PORT-TOP-04E_data/settings_baseline.bin $ROOT/mashed_re_settings.bin; }
# 3. (argument "nav") reach runs on the nav-demo walk (the 12-capture walk's input), knob OFF and ON, 300 s
if [ "${1:-}" = nav ]; then for k in off on; do
  reset
  kv="MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1 MASHED_DET_MPG_PIN=0 MASHED_VERIFY_OUT=$DATA/nav_$k"; [ $k = on ] && kv="$kv MASHED_ORIG_TOP=1"
  $LOCK py -3.12 re/tools/ddkobj/regconv_reach.py run mashedmod/build/ddkobj $ROOT 300 $DATA/reach_nav_$k.tsv $kv
  cp $ROOT/mashed_re.log $DATA/reach_nav_$k.log 2>/dev/null
done; exit 0; fi
for k in off on; do
  reset
  kv="MASHED_DET_FRAMES=3000"; [ $k = on ] && kv="$kv MASHED_ORIG_TOP=1"
  KNOB_EXE=$PWD/mashedmod/build/ddkobj_adapt_solo/mashed_re_ddk.exe PORTTOP_OUT=$DATA \
    $LOCK py -3.12 verify/porttop_20261108/knob_boot.py adapt_solo_$k $ROOT 40 $kv
done
for k in off on; do
  reset
  kv="MASHED_DET_FRAMES=3000"; [ $k = on ] && kv="$kv MASHED_ORIG_TOP=1"
  $LOCK py -3.12 re/tools/ddkobj/regconv_reach.py run mashedmod/build/ddkobj $ROOT 40 $DATA/reach_run_$k.tsv $kv
  cp $ROOT/mashed_re.log $DATA/reach_run_$k.log 2>/dev/null
done
