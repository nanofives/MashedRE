#!/bin/sh
# D4-WSB-C4-BROAD-WS round 2, leg 3 arms exactly as PREREG_R2 lists them (run from the worktree root, under test-slot).
export MASHED_ROOT=D:/MashedWT/D4-WSB-C4-BROAD-WS_root PYTHONIOENCODING=utf-8 C4_DEREF=1
R=verify/wsbbroadws_20261027/run_bvh3.py
C4_ARGW='564040:1:ffff' py -3.12 $R 0055a1f0
C4_ARGW='565260:1:ffff;564c80:1:ffff' py -3.12 $R 0055ab30
C4_SCENARIO=census C4_HOLD=400 C4_WINDOW='300 30000' py -3.12 $R 0055bae0 0055c000 0055c0f0
