# STATUS u9773 (2026-10-20): in progress, no fix yet

Narrowed (PREREG amendment 1): car 3's call-149 split is made inside the SECOND wheel-contact-solver call (FUN_0046f6c0) of the
frame that produces call 149. Entry states / loads equal on both sides; the classifier (FUN_0046cc40) writes different depths for
all four wheels. Next measurement: the transformed wheel points `DAT_0088e620..0x0088e64c` (12 floats) and the ring matrix used by
`Rw_VtableDispatch` at that call, original side (a new `scenario_launch.py` probe: read 0x0088e620 at the classifier's entry, or
at FUN_0046f6c0's call to 0x004c3d90, filtered to the statediff car) against the port (`g_wheelContactPos` logged at the same
point under `MASHED_WCS_POSLOG`). If the points differ: the ring matrix / position after the second substep's integrate
(FUN_0046e9e0) or the suspension scratch (`g_suspScratch`, written in A5) is the producer; if they are equal: the walked batch
(FUN_00538c80 / FUN_00468b80) differs.

Kickoff for the next session: "Continue U-9773 on branch work/u9773 (worktree .worktrees/u9773, tip <sha>): read
verify/u9773_20261020/PREREG_u9773.md amendment 1 and STATUS_u9773.md; add the DAT_0088e620 probe on the original and the matching
port log, find which input of car 3's second solver call in the frame of record call 149 differs, port the fix behind a
default-OFF knob, then car 2 (call 150), A car 1, B car 2. Same rules."
