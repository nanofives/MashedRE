#!/bin/sh
# C4 group M run matrix: 4 concurrent MASHED per batch, each script kills only its own PID.
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/c4a1-m
S=re/frida/c4_m_scenario.py
O=log/c4_m/runs; mkdir -p $O
C1=log/c4_m/ctl1_mashed_re_dev.asi; C2=log/c4_m/ctl2_mashed_re_dev.asi; D=original/mashed_re_dev.asi
run(){ tag=$1; shift; timeout 600 py -3.12 $S --out $O --tag $tag "$@" > $O/$tag.txt 2>&1; }
batch(){ for j in "$@"; do eval "$j" & sleep 6; done; wait; }
batch "run off1 --arm off" "run off2 --arm off" "run on1 --arm on" "run on2 --arm on"
batch "run stock1 --arm stock" "run late1 --arm late --ctl-asi $D" "run ctlA1 --arm late --rows A --ctl-asi $C1" "run ctlB1 --arm late --rows B --ctl-asi $C1"
batch "run ctlC1 --arm late --rows C --ctl-asi $C1" "run ctlT1 --arm late --rows T --ctl-asi $C1" "run ctlA2 --arm late --rows A --ctl-asi $C1" "run ctlB2 --arm late --rows B --ctl-asi $C1"
batch "run ctlC2 --arm late --rows C --ctl-asi $C1" "run ctlT2 --arm late --rows T --ctl-asi $C1" "run ctlTd1 --arm late --rows T --ctl-asi $C2" "run ctlTd2 --arm late --rows T --ctl-asi $C2"
batch "run offF1 --arm off --force-f" "run onF1 --arm on --force-f" "run ctlF1 --arm late --rows F --ctl-asi $C1 --force-f" "run ctlF2 --arm late --rows F --ctl-asi $C1 --force-f"
echo MATRIX-DONE
