#!/bin/bash
# RO-MENU-DIV-43DFD0 batch 2: class-matched repeats of the fixed blobs, a menu negative control, one more reference.
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-MENU-DIV-43DFD0
export PROYECTOS_TEST_MEM_GB=4
F=D:/MashedWT/RO-MENU-DIV-43DFD0_data/bodies_fix1.json
A=verify/romenudiv_20261112/run_arm.sh
for k in 2 3 4 5; do test-slot bash $A f${k}_43dfd0 --prov $F --prov-rvas 0x0043dfd0; done
test-slot bash $A f2_43d2a0 --prov $F --prov-rvas 0x0043d2a0
test-slot bash $A negm --negctl 0x0067f17c --neg-tick 300
test-slot bash $A ref3
echo BATCH_DONE
