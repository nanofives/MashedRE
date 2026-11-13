#!/bin/bash
# RO-FIRST-DIVERGENCE arms. Call under test-slot; every launch takes mashed_lock itself. Kills only PIDs the tools spawn.
#   firstdiv_arms.sh orig <label> [detorig.py args...]   original, boot-mode replay matched to replay_trace.json
#                                                        (q0 0x40000000, warp at tick 300 with clock 300, 600 records)
#   firstdiv_arms.sh exe <vs|ddk> <arm>                  exe RO_ arm (run_arms.py); A4_RO_CKPT / A4_RO_WATCH from the caller
cd "$(dirname "$0")/../.."
W=$(pwd)
LOCK=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py
D=D:/MashedWT/RO-FIRST-DIVERGENCE_data
RAW=0x00603400-0x00603800,0x00614400-0x00614800,0x00618000-0x00618400,0x006dcc00-0x006dd400,0x00773800-0x00773c00,0x007dd000-0x007dd400,0x007f0c00-0x007f1400
mkdir -p $D
K=$1; shift
if [ "$K" = orig ]; then
  L=$1; shift
  rm -rf $D/$L
  MASHED_ROOT='D:\MashedWT\RO-FIRST-DIVERGENCE_root' py -3.12 $LOCK run --label RO-FIRST-DIVERGENCE -- \
    py -3.12 re/frida/detorig.py --label $L --out $D/$L --clock tick --boot --q0 0x40000000 --tick-raw $RAW --timeout ${TMO:-400} \
      --tick-cap ${CAP:-600} --warp-tick 300 --warp-clock 300 --normalize re/frida/warp_normalize.tsv --load-sleep-ms 0 "$@" > $D/$L.log 2>&1
  echo "[$L] exit $? $(date +%T)"
  grep -E "warp at|race anchor|ticks |=== rc|exited|agent error|error|ckpt-tick0|watch" $D/$L.log | cut -c1-300 | head -20
elif [ "$K" = exe ]; then
  B=$1; shift
  export A4_OUT=$D A4_ROOT=D:/MashedWT/RO-FIRST-DIVERGENCE_root
  if [ "$B" = ddk ]; then export A4_EXE="$(pwd -W)/mashedmod/build/ddkobj/mashed_re_ddk.exe"; else export A4_EXE="$(pwd -W)/mashedmod/build/mashed_re.exe"; fi
  for a in "$@"; do
    py -3.12 $LOCK run --label RO-FIRST-DIVERGENCE -- py -3.12 verify/savelink_20261023/run_arms.py $a > $D/$a.out 2>&1
    echo "[$a] exit $? $(date +%T)"
    grep -E "\[TRACE\] (self-test|watch)|\[REPLAY\] self-test" D:/MashedWT/RO-FIRST-DIVERGENCE_root/mashed_re.log | tail -4
  done
fi
