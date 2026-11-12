#!/bin/bash
# RO-MENU-DIV-43DFD0 batch 4: more fixed-0x0043d2a0 repeats for a second class-matched run.
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-MENU-DIV-43DFD0
export PROYECTOS_TEST_MEM_GB=4
F=D:/MashedWT/RO-MENU-DIV-43DFD0_data/bodies_fix1.json
A=verify/romenudiv_20261112/run_arm.sh
for k in 3 4 5; do test-slot bash $A f${k}_43d2a0 --prov $F --prov-rvas 0x0043d2a0; done
echo BATCH_DONE
