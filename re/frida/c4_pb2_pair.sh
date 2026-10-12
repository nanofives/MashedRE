#!/bin/bash
# Run two c4_pb2_scenario.py arms in parallel, the second spawned 40 s after the first (concurrent
# frida.spawn calls collide, multi_state_driver.py). Each arm kills only its own PID.
# usage: CTL_ASI=<control .asi> bash re/frida/c4_pb2_pair.sh "<arm> <tag> [ctl|late]" ["<arm> <tag> [ctl|late]"]
#   ctl  -> --ctl-asi $CTL_ASI ; late -> --ctl-asi original/mashed_re_dev.asi (load-path twin of 'on')
cd "$(dirname "$0")/../.."
DEP="$PWD/original/mashed_re_dev.asi"
run() { set -- $1; a=$1; t=$2; x=""; [ "$3" = ctl ] && x="--ctl-asi $CTL_ASI"; [ "$3" = late ] && x="--ctl-asi $DEP";
  timeout 1300 py -3.12 re/frida/c4_pb2_scenario.py --arm $a --tag $t --round 600 --presses 18 $x > log/c4_pb2_$t.txt 2>&1; }
run "$1" & sleep 40; [ -n "$2" ] && run "$2" & wait
