# PREREG u9776: car 3 past call 157 and A car 2 with MASHED_WCS_TRINORMAL (default ON since trinflip) (2026-10-20)

Branch `work/u9776` from `3880860f1`. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`.

Step 1 (measurement, no knob): original `--tri-probe` (new: entry + exit of FUN_004c52f0 from 0x00470004, callee-entry only;
count gate `o_tqc_1` 53066 calls, 4 hits, no error) on the B recipe car 3, checked against `o_c3` with `origcmp.py`
(`o_tq_1` rolled at 61, discarded; `o_tq_2` equal through 399). Port: `MASHED_WCS_TRILOG` now also writes `<path>.tq.csv`, the
same fields at the same point. Arms `B_TR3_K0/K1/K26` (car 3 records + log) on the default. Compare every alignment call of car 3
(record calls 149 and 163 on `o_tq_2`) field by field: angle, local_e0, axis, local_ac, m, ring before, ring after.

Already measured on `o_tq_2`: the angle is the 2.5 clamp (`0x40200000`, `0x0046ffbd`) at every alignment call of every car, so
the acos result does not reach the matrix on these calls.

Prediction: the first differing field at call 149 names the step (local_e0 = FUN_0046c5f0 / the wheel points; axis = the cross;
m = FUN_004c4d20; ring_post with equal m and ring_pre = FUN_004c52f0). A knob for a fix comes with its own amendment.
