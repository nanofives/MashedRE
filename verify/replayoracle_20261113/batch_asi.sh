#!/bin/bash
# RO-ORIG-ASI arms (PREREG): Frida references f1 f2, ASI replays a1 a2 a3, ASI negative control aneg. One test slot each.
R=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-ORIG-ASI/verify/replayoracle_20261113/run_arm_asi.sh
for spec in "f1" "a1 --asi" "f2" "a2 --asi" "a3 --asi" "aneg --asi --negctl 0x0067f17c --neg-tick 300"; do
  set -- $spec; L=$1; shift
  test-slot bash $R $L "$@"
done
