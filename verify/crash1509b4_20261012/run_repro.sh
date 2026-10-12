#!/bin/bash
# crash1509b4 repro: explore5 route (Team Play, screen 18 row 4 Vehicle RIGHT x1) vs the same route with no edit.
cd "C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/crash1509b4"
OUT=verify/crash1509b4_20261012
# private install copy (robocopy of original/, exe SHA-256 B9977DAB...FE87), used by the root* arms only
GR="C:/Users/maria/AppData/Local/Temp/claude/C--Users-maria-Desktop-Proyectos-Mashed/9eafd301-97ce-428b-80c5-c57899a60131/scratchpad/gameroot/original"
for i in "$@"; do
  case $i in
    rootprobe*) C4PB2_GAMEROOT="$GR" C4PB2_PROBE_PIZ=1 C4PB2_S18_ROW=4 C4PB2_EXTRA_OPP=0 py -3.12 re/frida/c4_pb2_scenario.py --arm stock --catch-crash --round 60 --presses 0 --out $OUT --tag $i > $OUT/$i.txt 2>&1 ;;
    rootctl*) C4PB2_GAMEROOT="$GR" C4PB2_S18_ROW=4 C4PB2_EXTRA_OPP=0 py -3.12 re/frida/c4_pb2_scenario.py --arm stock --catch-crash --round 60 --presses 0 --out $OUT --tag $i > $OUT/$i.txt 2>&1 ;;
    rootholdveh*) C4PB2_GAMEROOT="$GR" C4PB2_HOLD_PIZ=1 C4PB2_S18_ROW=4 C4PB2_EXTRA_OPP=1 py -3.12 re/frida/c4_pb2_scenario.py --arm stock --catch-crash --round 60 --presses 0 --out $OUT --tag $i > $OUT/$i.txt 2>&1 ;;
    roothold*) C4PB2_GAMEROOT="$GR" C4PB2_HOLD_PIZ=1 C4PB2_S18_ROW=4 C4PB2_EXTRA_OPP=0 py -3.12 re/frida/c4_pb2_scenario.py --arm stock --catch-crash --round 60 --presses 0 --out $OUT --tag $i > $OUT/$i.txt 2>&1 ;;
    veh*) C4PB2_S18_ROW=4 C4PB2_EXTRA_OPP=1 py -3.12 re/frida/c4_pb2_scenario.py --arm stock --catch-crash --round 60 --presses 0 --out $OUT --tag $i > $OUT/$i.txt 2>&1 ;;
    dzveh*) C4PB2_DEV_ZERO=3 C4PB2_S18_ROW=4 C4PB2_EXTRA_OPP=1 py -3.12 re/frida/c4_pb2_scenario.py --arm stock --catch-crash --round 60 --presses 0 --out $OUT --tag $i > $OUT/$i.txt 2>&1 ;;
    dzctl*) C4PB2_DEV_ZERO=3 C4PB2_S18_ROW=4 C4PB2_EXTRA_OPP=0 py -3.12 re/frida/c4_pb2_scenario.py --arm stock --catch-crash --round 60 --presses 0 --out $OUT --tag $i > $OUT/$i.txt 2>&1 ;;
    ctl*) C4PB2_S18_ROW=4 C4PB2_EXTRA_OPP=0 py -3.12 re/frida/c4_pb2_scenario.py --arm stock --catch-crash --round 60 --presses 0 --out $OUT --tag $i > $OUT/$i.txt 2>&1 ;;
  esac
  echo "== $i done"; grep -E "spawned|piz locked|HOLD_PIZ|open .piz|CRASH-FULL|devices|DEV_ZERO|colour choices|s18|CRASH|verdict|Race Results|process exited" $OUT/$i.txt | cut -c1-300
done
# (appended) dz* arms: same as veh/ctl with profile 3's device dword zeroed at the title (C4PB2_DEV_ZERO=3).
