#!/bin/bash
# ADMITTED-555360-REFRESH knob boots, inside one mashed_lock hold (test slot outside). Run from the worktree root.
R=D:/MashedWT/ADMITTED-555360_root
export PORTTOP_OUT=D:/MashedWT/ADMITTED-555360_data PYTHONIOENCODING=utf-8
for e in default obj; do
  for rw in 0 1; do
    KNOB_EXE=D:/MashedWT/ADMITTED-555360_data/mashed_re_$e.exe py -3.12 verify/porttop_20261108/knob_boot.py boot_${e}_rwmod$rw $R 300 MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=$rw MASHED_DET_FRAMES=3000
  done
done
