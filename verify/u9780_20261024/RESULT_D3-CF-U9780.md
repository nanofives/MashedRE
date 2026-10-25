# RESULT D3-CF-U9780: car 2's 1-ulp vx at k=218 (B K0) does not reproduce on the current default - GREEN (2026-10-25)

Branch `orch/D3-CF-U9780` from `orch/integration` `b5c84688e`. No source change. Build `MASHED_NO_DEPLOY=1`, exe `7c78d294`.
Private root `D:\MashedWT\D3-CF-U9780_root`, every boot inside `mashed_lock.py run`. Runner `run_arms.py` (copy of
`verify/savelink_20261023/run_arms.py`, recipe F, adds the `C2*` stacks). Comparison `reccmp.py`: car 2's record (slot 2,
`MASHED_RECDUMP_SLOT=2`) against the original captures `o_c2_2` / `o_c2_3` (`D:\MashedWT\u9777_data\arms`), aligned on k the way
`verify/u9778_20261021/velcmp.py` does. It prints position `+0x958/95c/960`, velocity `+0x9b0/9b4/9b8` and angular velocity
`+0x9bc/9c0/9c4` as raw hex, and the ulp distance where a word differs. Slices k 170..222: `reccmp_slices.txt`. Provenance: `RUNLOG.txt`.

## 1. Verdict

U-9780 is resolved on the current default. On the default exe car 2 matches both original captures in every position, velocity
and angular-velocity word on all 220 common k (0..219). The first difference is k=220, the round end (U-9779 / U-9781).

The fiber deletion did not resolve it. At soflip (exe `50558a73`, `SUBSTEP_OUTER` seeded, fiber code gone) the row reproduces:
vx `0x44f06b06` in the original against `0x44f06b07` in the port at k=218 (1 ulp, 1923.34). The default flip that removed it is
**dotflip** (`2d9cd2b2f`, `MASHED_A6A_DOT_ORDER` default ON). That flip put A6a `0x00467650`'s three 3-term dots in the original's
association: the drive arm `0x00467be9..0x00467c0c`, block #5 `0x0046837d..0x00468394` and clamp #6 `0x00468771..0x00468793`
(`verify/dcphys_20261021/PREREG_dcphys.md:24`). On the current exe `MASHED_NO_A6A_DOT_ORDER=1` alone brings back the soflip-era
car-2 record, line for line over k 0..219.

## 2. Arms (B recipe K0 = recipe F, `MASHED_RING_SKIP=0`, 1200 frames)

| arm | exe | stack | step sha8 | car 2 vs `o_c2_2` (and `o_c2_3`) |
|---|---|---|---|---|
| `B_C2_K0` | `7c78d294` | default + slot-2 record + car-car log | `f9deb9b9` (= `B_DEF_K0`) | 220/220 rows equal, first difference k=220 (both captures) |
| `B_C2NX_K0` | `7c78d294` | + `MASHED_NO_SUBSTEP_OUTER=1` (instrument control) | `1cce0ea9` | first difference pos / vel k=204, angvel k=214 (the u9778 split) |
| `B_C2_K0-sf` | `50558a73` (soflip) | slot-2 record + car-car log | `9bd86ffe` (= soflip B K0) | angvel from 174 (1..8 ulp), vel `+0x9b4` 179..184 (1 ulp), pos `+0x95c` 182 / 183; vel equal 185..217, pos equal 184..219; **k=218 vx `44f06b06 / 44f06b07`, 1 ulp**; k=219 vx 1 ulp; k=220 the round-end split |
| `B_C2NODOT_K0` | `7c78d294` | + `MASHED_NO_A6A_DOT_ORDER=1` | `2af53fa6` | lines for k 0..219 identical to `B_C2_K0-sf` |
| `B_C2NOP2_K0` | `7c78d294` | + the eight dcphys2 opt-outs (`NOP2`) | `7990b823` | 220/220 rows equal, first difference k=220 |
| `B_C2NOB_K0` | `7c78d294` | + `NODOT` + `NOP2` | `34cdb312` | lines for k 0..219 identical to `B_C2_K0-sf` |

The step dumps of `NODOT` / `NOB` differ from soflip's (`9bd86ffe`), so later flips also move other cars or other step columns.
Car 2's record over k 0..219 does not depend on them: the dcphys2 stack (`p2flip` `fadf5eb63`) is inert on it in both directions.

The row as filed (u9778, fiber form) said bit-exact through 217 with the k=218 vx as the first difference. The soflip arm shows
angular-velocity and `+0x9b4` differences of 1..8 ulp from k=174 that u9778's velocity table did not count. Position and
`+0x9b0/9b8` are equal through 217 in that arm, so the filed k=218 vx difference is the first position / velocity split along
`+0x9b0`. Both kinds of difference are gone on the default.

## 3. Gates (exe `7c78d294`, no knob)

| gate | measured | expected (WORKER_RULES) |
|---|---|---|
| G-CTRL `H_DEF` | `bacfc747 / 8dd1c004 / 6f9932b6` | `bacfc747 / 8dd1c004 / 6f9932b6` |
| B `B_DEF_K0 / K1 / K26` | `f9deb9b9 / d3601fcc / 9da9a26a` | `f9deb9b9 / d3601fcc / 9da9a26a` |

Recording slot 2 and the car-car log do not move the step dump (`B_C2_K0` = `B_DEF_K0`). Every arm ran once, self-exited with
exit 0, no crash, no re-run. Only PIDs this lane spawned were touched.

## 4. Trackers

`apply_trackers.py` (the re-classify transaction, no C-level change): U-9780 marked RESOLVED with this evidence, one CHANGELOG
line. ROADMAP's D3 carried-forward line for U-9780 is left to the merge train.

## 5. Next

The next car-2 split on B K0 is k=220, the round end, which is U-9779 / U-9781's lane. Nothing in this lane remains open.
