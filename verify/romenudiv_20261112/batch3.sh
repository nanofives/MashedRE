#!/bin/bash
# RO-MENU-DIV-43DFD0 batch 3: more fixed-0x0043dfd0 repeats until two land in the reference class 39a80501,
# plus no-body references so other classes have their own reference.
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-MENU-DIV-43DFD0
export PROYECTOS_TEST_MEM_GB=4
F=D:/MashedWT/RO-MENU-DIV-43DFD0_data/bodies_fix1.json
A=verify/romenudiv_20261112/run_arm.sh
for k in 6 7 8 9; do test-slot bash $A f${k}_43dfd0 --prov $F --prov-rvas 0x0043dfd0; done
for k in 4 5; do test-slot bash $A ref$k; done
echo BATCH_DONE
