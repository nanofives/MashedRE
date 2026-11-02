#!/bin/bash
# batch 1: load hold (50 ms per main-thread clock call in phase 2, the detorig.py default) + call log
D=D:/MashedWT/PROMOTE-DETORIG-LOADHOLD_data
H=0x00468980,0x00470670
echo "batch1 start $(date +%T)"
bash $D/run_arm.sh off_g --call-log --timeout 400
bash $D/run_arm.sh on_c  --call-log --timeout 400 --hooks $H
bash $D/run_arm.sh off_h --call-log --timeout 400
bash $D/run_arm.sh on_d  --call-log --timeout 400 --hooks $H
bash $D/run_arm.sh neg_c --call-log --timeout 400 --negctl 1
bash $D/run_arm.sh off_i --call-log --timeout 400
bash $D/run_arm.sh off_w --call-log --timeout 400 --watch 0x007eca10,0x007eca50
bash $D/run_arm.sh on_w  --call-log --timeout 400 --hooks $H --watch 0x007eca10,0x007eca50
echo "batch1 end $(date +%T)"
