#!/bin/bash
# PORT-SAVE-SPAN-MIRROR diagnostic: 0x0067ea70..0x0067eaff and 0x00803344 at walk_04 on the original
# (orig_walk04.py, orch/PORT-TOP-04D-WALK04 @ 2528ad5d4) and on the exe, hand and matched paths.
O=D:/MashedWT/PORT-SAVE-SPAN-MIRROR_data; X=$O/exe/mashed_re.exe; R=D:/MashedWT/PORT-SAVE-SPAN-MIRROR_root
WALK=/c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/PORT-SAVE-SPAN-MIRROR/verify/porttop_20261108/walk04/orig_walk04.py
py -3.12 $WALK --root D:/MashedWT/PORT-TOP-04D-WALK04_root --out $O --tag orig_snap --snap 0x0067ea70,0x0067ea74,0x0067ea78,0x0067ea7c,0x0067ea80,0x0067ea84,0x0067ea88,0x0067ea8c,0x0067ea90,0x0067ea94,0x0067ea98,0x0067ea9c,0x0067eaa0,0x0067eaa4,0x0067eaa8,0x0067eaac,0x0067eab0,0x0067eab4,0x0067eab8,0x0067eabc,0x0067eac0,0x0067eac4,0x0067eac8,0x0067eacc,0x0067ead0,0x0067ead4,0x0067ead8,0x0067eadc,0x0067eae0,0x0067eae4,0x0067eae8,0x0067eaec,0x0067eaf0,0x0067eaf4,0x0067eaf8,0x0067eafc,0x00803344
NAV="--env MASHED_NAV_DEMO=1 --env MASHED_DETERMINISTIC=1"
: py -3.12 exe_snap.py --exe $X --root $R --out $O/snap_hand.json --addrs 0x0067ea70-0x0067eb00,0x00803344-0x00803348 $NAV
: py -3.12 exe_snap.py --exe $X --root $R --out $O/snap_matched.json --addrs 0x0067ea70-0x0067eb00,0x00803344-0x00803348 $NAV --env MASHED_ORIG_TOP=1 --env MASHED_TOP04D_DATAZERO=1
