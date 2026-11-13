#!/usr/bin/env bash
# DDKOBJ-REGCONV arms (PREREG_DDKOBJ-REGCONV.md arms 3-5). Run from the worktree root.
# Every launch: test-slot outside, mashed_lock inside; waits while the merge train holds PRIORITY-HOLD or SOLO.
set -u
W=$(pwd)
ORCH=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
ROOT=D:/MashedWT/DDKOBJ-REGCONV_root
DATA=D:/MashedWT/DDKOBJ-REGCONV_data
DDK=$W/mashedmod/build/ddkobj/mashed_re_ddk.exe
VS=$W/mashedmod/build/mashed_re.exe
STEPS=${STEPS:-"boots walk bisect"}

wait_quiet() {
  while py -3.12 "$ORCH" status 2>&1 | grep -q "PRIORITY-HOLD\|SOLO"; do
    echo "[wait] merge-train PRIORITY-HOLD / SOLO open $(date +%T)"; sleep 60; done
}

boot() {   # boot <label> <exe> [KEY=VALUE ...]
  local lab=$1 exe=$2; shift 2
  wait_quiet
  KNOB_EXE=$exe PORTTOP_OUT=$DATA test-slot bash -c "py -3.12 '$ORCH' run --label DDKOBJ-REGCONV -- py -3.12 verify/porttop_20261108/knob_boot.py $lab $ROOT 180 $*"
}

if [[ $STEPS == *boots* ]]; then
  echo "[1/3] ddk_off";  boot ddk_off "$DDK" MASHED_DET_FRAMES=3000
  echo "[2/3] ddk_on";   boot ddk_on  "$DDK" MASHED_DET_FRAMES=3000 MASHED_ORIG_TOP=1
  echo "[3/3] vs_on";    boot vs_on   "$VS"  MASHED_DET_FRAMES=3000 MASHED_ORIG_TOP=1
fi

if [[ $STEPS == *walk* ]]; then
  for spec in OFF@dflt ON@dflt OFF@dflt ON@dflt; do
    wait_quiet
    echo "[walk] $spec"
    KNOB_EXE=$DDK CAPFLAKE_ROOT=$ROOT CAPFLAKE_DATA=$DATA/caps test-slot bash -c \
      "py -3.12 '$ORCH' run --label DDKOBJ-REGCONV -- py -3.12 verify/porttop_20261108/capflake_runs.py ddk_${spec%@*}_$RANDOM $spec"
  done
fi

if [[ $STEPS == *bisect* ]]; then
  wait_quiet
  # arm 5: the first, unguarded spike build of the 38 units (DDKOBJ_REGCONV=promote), knob OFF, bisected to the culprit(s)
  DDKOBJ_REGCONV=promote BISECT_ROOT=$ROOT BISECT_DATA=$DATA/bisect BISECT_LABEL=DDKOBJ-REGCONV \
    test-slot bash -c "py -3.12 re/tools/ddkobj/bisect_units.py $DATA/regconv38.json $DATA/BISECT_promote38.tsv --knob off"
fi
echo "[arms] done"
