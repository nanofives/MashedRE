#!/bin/bash
# crash1509b4 natural two-process test: two stock MASHED on the private copy, Team Play route, Vehicle left 0,
# race-start confirms synchronized by a file barrier. Usage: run_pair.sh <pairtag> [side-b delay s, default 0]
cd "C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/crash1509b4"
OUT=verify/crash1509b4_20261012
GR="C:/Users/maria/AppData/Local/Temp/claude/C--Users-maria-Desktop-Proyectos-Mashed/9eafd301-97ce-428b-80c5-c57899a60131/scratchpad/gameroot/original"
B="C:/Users/maria/AppData/Local/Temp/claude/C--Users-maria-Desktop-Proyectos-Mashed/9eafd301-97ce-428b-80c5-c57899a60131/scratchpad/barrier_$1"
rm -rf "$B"
for side in a b; do
  D=0; [ "$side" = b ] && D="${2:-0}"
  C4PB2_BARRIER_DELAY="$D" C4PB2_GAMEROOT="$GR" C4PB2_BARRIER="$B" C4PB2_S18_ROW=4 C4PB2_EXTRA_OPP=0 py -3.12 re/frida/c4_pb2_scenario.py --arm stock --catch-crash --round 60 --presses 0 --out $OUT --tag "$1$side" > $OUT/$1$side.txt 2>&1 &
  sleep 3
done
wait
for side in a b; do echo "== $1$side"; grep -E "spawned|BARRIER|CRASH-FULL|verdict|Race Results" $OUT/$1$side.txt | cut -c1-260; done
