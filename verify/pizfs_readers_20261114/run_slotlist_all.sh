#!/bin/bash
# Three runs in sequence: menu (no warp), race track 0, race track 3. Each wrapped by its own test-slot.
S=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/RO-PIZFS-SLOT-TRACE/verify/pizfs_readers_20261114/run_slotlist.sh
test-slot bash "$S" fs_menu 30 --no-warp
test-slot bash "$S" fs_t0 30 --track 0
test-slot bash "$S" fs_t3 30 --track 3
echo ALL_DONE
