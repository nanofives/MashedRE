#!/bin/bash
# RWC-PLUGIN-ATTACH run 40: one MASHED_RWC_FRAME iteration. Regenerate + route, then build until it passes (max 6):
# a source that fails VS2022 C++ goes to LIFT_INSTEAD_RUN40.txt (lifted instead), a lint ORIGTOP-ONLY pair suggestion
# is allowlisted (allow_origtop.py); anything else stops the loop. Then boot vs_frame and print where it ended.
# Builds wait out merge-train PRIORITY-HOLD / SOLO. usage: frame_iter.sh <build number>
cd "$(dirname "$0")/../.."
D=${RWC_DATA:-D:/MashedWT/RWC-PLUGIN-ATTACH_data}   # RWC-FIRST-FRAME-IM2D: RWC_DATA / RWC_LABEL override
L=verify/rwccorerun_20261111/LIFT_INSTEAD_RUN40.txt
N=$1
for i in 1 2 3 4 5 6; do
  py -3.12 verify/rwccorerun_20261111/gen_rwcore_tus.py 2>&1 | tail -1
  py -3.12 verify/rwccorerun_20261111/route_bindings.py | head -1
  until ! py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py status 2>&1 | grep -q "PRIORITY-HOLD\|SOLO"; do sleep 30; done
  MASHED_NO_DEPLOY=1 cmd //c "mashedmod\\build.bat" > $D/build_vs$N.log 2>&1
  if grep -aq "Build OK" $D/build_vs$N.log; then echo "build_vs$N OK"; break; fi
  progress=0
  if grep -aq "ORIGTOP-ONLY pair" $D/build_vs$N.log; then
    py -3.12 verify/rwccorerun_20261111/allow_origtop.py $D/build_vs$N.log; progress=1
  fi
  srcs=$(grep -a " error " $D/build_vs$N.log | grep -o "re/match/[a-z0-9]*/[0-9a-z_]*\.c" | sort -u)
  if [ -n "$srcs" ]; then echo "$srcs" >> $L; sort -u -o $L $L; echo "lift instead: $(echo $srcs | wc -w) source(s)"; progress=1; fi
  if [ $progress = 0 ]; then
    grep -aE " error |LNK2|fatal|rva-lint\] (ERROR|FAILED)" $D/build_vs$N.log | cut -c1-220 | head -8
    echo "build_vs$N FAILED (not a lift-instead or ORIGTOP-ONLY case)"; exit 1
  fi
  N=$((N+1))
done
grep -aq "Build OK" $D/build_vs$N.log || { echo "no passing build after 6 tries"; exit 1; }
test-slot bash -c "py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --label ${RWC_LABEL:-RWC-PLUGIN-ATTACH} -- bash verify/rwccorerun_20261111/run_arms_attach.sh vs_frame" 2>&1 | grep -a "exited"
cp $D/vs_frame.log $D/vs_frame_$N.log
grep -a "RWC_CORE\|RWC-DEVICE" $D/vs_frame.log | grep -v "registry node\|probe:" | sed -n '/RwEngineStart returns/,$p' | cut -c1-300 | head -8
echo "next build number: $((N+1))"
