#!/bin/bash
# RWC-FRAME-CLUMP: one bring-up iteration. Regenerate the RwCore TUs, build (waits out a merge-train PRIORITY-HOLD /
# SOLO), boot vs_frameN (MASHED_RWC_FRAME_N, default 120) inside test-slot -> mashed_lock.py run, and print where it
# ended with the AV frames resolved against the map. usage: iter.sh <tag>
cd "$(dirname "$0")/../.."
D=${RWC_DATA:-D:/MashedWT/RWC-FRAME-CLUMP_data}
T=$1
py -3.12 verify/rwccorerun_20261111/gen_rwcore_tus.py 2>&1 | tail -1
py -3.12 verify/rwccorerun_20261111/route_bindings.py | head -1
until ! py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py status 2>&1 | grep -q "PRIORITY-HOLD\|SOLO"; do sleep 30; done
MASHED_NO_DEPLOY=1 cmd //c "mashedmod\\build.bat" > $D/build_$T.log 2>&1
if ! grep -aq "Build OK" $D/build_$T.log; then
  grep -aE " error |LNK2|fatal|rva-lint\] (ERROR|FAILED)" $D/build_$T.log | cut -c1-250 | head -8; echo "build_$T FAILED"; exit 1
fi
echo "build_$T OK"
test-slot bash -c "py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --label RWC-FRAME-CLUMP -- bash verify/rwcframeclump_20261113/run_arms.sh vs_frameN" 2>&1 | grep -a "exited"
cp $D/vs_frameN.log $D/vs_frameN_$T.log
grep -a "7420:\|AV at\|STOP\|boot ends\|WinMain 0x00492370 returned" $D/vs_frameN.log | tail -12 | cut -c1-220
offs=$(grep -a "AV stack exe offsets" $D/vs_frameN.log | head -1 | sed 's/.*offsets: //')
eip=$(grep -a "AV at" $D/vs_frameN.log | head -1 | sed -n 's/.*(exe+\(0x[0-9A-F]*\)).*/\1/p')
[ -n "$offs" ] && py -3.12 verify/rwccorerun_20261111/mapsym.py mashedmod/build/mashed_re.map $eip $offs 2>&1 | grep -v "DffModel\|g_b17\|acrt\|ImageBase\|PortTop_00401000.obj)\|PortTop_0040bbb0.obj)" | head -14
