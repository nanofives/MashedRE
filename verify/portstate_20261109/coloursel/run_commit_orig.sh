#!/bin/bash
# PORT-COLOURSEL-COMMIT arm 1 (original): menu_draw_burst.py --nav to the cup-flow colour select, move the choice
# 0x0067f0c0 to N, confirm (0xff1d0000 arm, commit at 0x0043e56c..0x0043e57e), confirm on the challenge select, wait,
# then peek the slot colour 0x007f1a1c (+0x10 per slot), the choice, the cursor 0x0067ea98, the depth 0x0067e9f8,
# the game mode 0x0067e9fc and the session phase 0x00771968 (U8, 3 = race). Run inside test-slot; each boot takes
# mashed_lock itself.
O=D:/MashedWT/PORT-COLOURSEL-COMMIT_data
P=0x007f1a1c,0x007f1a2c,0x007f1a3c,0x007f1a4c,0x0067f0c0,0x0067ea98,0x0067e9f8,0x0067e9fc,0x00771968
L="py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label PORT-COLOURSEL-COMMIT --"
export MASHED_ROOT=D:/MashedWT/PORT-COLOURSEL-COMMIT_root
for spec in "n1:" "n3:right,right," "n6:right,right,right,right,right,right,right," "n5l:right,right,right,right,right,right,left,"; do
  lab=${spec%%:*}; mv=${spec#*:}
  $L py -3.12 re/frida/menu_draw_burst.py --nav confirm,confirm,confirm,${mv}confirm,confirm,wait:3,confirm,wait:20 \
     --frames 2 --label $lab --out $O/race_$lab.json --peek $P > $O/race_$lab.log 2>&1
  echo "$lab rc=$?"; grep -a "peek after" $O/race_$lab.log
done
