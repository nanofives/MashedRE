#!/bin/bash
# RO-MENU-DIV-43DFD0 batch 1: no-body references, the PROV-INSITU blobs (before) and the fixed blobs (fix1).
cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-MENU-DIV-43DFD0
export PROYECTOS_TEST_MEM_GB=4
B=verify/provinsitu_20261111/bodies_all.json
F=D:/MashedWT/RO-MENU-DIV-43DFD0_data/bodies_fix1.json
A=verify/romenudiv_20261112/run_arm.sh
test-slot bash $A ref1
test-slot bash $A b_43dfd0 --prov $B --prov-rvas 0x0043dfd0
test-slot bash $A b_43d2a0 --prov $B --prov-rvas 0x0043d2a0
test-slot bash $A f1_43dfd0 --prov $F --prov-rvas 0x0043dfd0
test-slot bash $A f1_43d2a0 --prov $F --prov-rvas 0x0043d2a0
test-slot bash $A ref2
echo BATCH_DONE
