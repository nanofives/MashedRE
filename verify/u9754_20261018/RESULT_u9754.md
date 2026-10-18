# RESULT u9754: the spawn per-wheel ground normal (2026-10-18)

Branch `work/u9754` from `race/first-frame-parity` `e7bc39c3b` (default G-CTRL `b1f49e7a / b1be0054 / d3a20f4c`). Model
claude-opus-5-5, account2. PREREG `PREREG_u9754.md` with the knob (`b57af75dc`), amendments 1 (`62665711d`), 2
(`6cb179b82`), 3, each committed before the arms it governs. Exe `eb8c3e44` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9754_root`. Original references: `o_wobj2`, new capture `o_seed1` (same recipe, car-1 record, write
watchpoints on `+0x204` and `+0x95c`).

## 0. Verdict

1. **U-9754 is resolved.** The original's spawn wheel normal is written by the classifier `FUN_0046cc40` at `0x0046cff8`
   (watched on o_seed1: value 0.9994620680809021, return chain ending `0x0046f827`, after the walk call `0x0046f817` in
   `FUN_0046f6c0`) on the first physics frame after spawn: it is the walk's face normal of the triangle under the car,
   LUT-normalized (length 0.99995). It is held through the countdown until each wheel meets a new triangle (front pair at
   window call 15, rear pair at 22).
2. **The port's seed came from a port-only overwrite.** `SolveWheelContacts` (`Vehicle/VehiclePhysicsRun.cpp`) ends in
   `ReassertContacts`, which writes `ProbeGround`'s triangle-soup normal (exact unit) over all four wheel normals whenever
   the car is grounded. At spawn that replaced the classifier's walk normal: (0, 0.999512017, -0.0312363114) against the
   original's (0, 0.999462068, -0.0312347524).
3. New knob **`MASHED_REASSERT_KEEP_NORMAL`** (default OFF) skips that normal write. With it the seed is the original's bit
   for bit and the force words move toward the original's (`+0x210` exact at call 1, `+0x21c` 0.0078 off where it was
   0.126); with lk21's `MASHED_SPAWN_BASIS_RW` too, the half-valued words (`+0x39c`, `+0x460`, `+0xad4`) also match.
4. **Car 1's z drift is unchanged** (call 4 2.4e-7, call 21 1.2e-4, the lookahead still flips at 21; G-DRIFT FAIL). The
   remaining call-0 differences are the spawn y `+0x95c` (7 ulp, `FUN_0046d400`'s RW line query) with the wheel depths it
   feeds (`+0x194 / +0x258 / +0x31c / +0x3e0`), and the port's single pose in the `+0x968` half (the stated U-9152
   deviation). New U-9755.
5. **No default change.** `SBKN` (both knobs) keeps the default's (b) pass set but misses D2's driving-median bound on 3/3
   (1997.89 / 2002.84 / 1996.54 > 1982.44); each knob alone misses D2 too (`D2_SB_1` slip 1500-2000 0.1863, `D2_KN_1` 0.1861
   and driving-median 1995.34), and `KN` alone fails (b) B car 1.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `e332af0c / 01ad5975 / 772f9fb7`, `B_SPL_K1` `a8f11851` | PASS |
| G-SEED | `SPLKNR1` car 1 call 0 (0, 0.999462068, -0.0312347524) on all four wheels; refresh at 15 / 22 as the original | PASS |
| G-FORCE | call 1 `+0x210` exact, `+0x214` / `+0x21c` 1.0e-4 / 0.0078 off | not exact |
| G-DRIFT | `SPLKN` / `SPLSBKN` car 1 z gap at call 21 1.2e-4 | FAIL |
| G-B | `KN` A 2/3 B 1/3 (car 1 `c1#` 16) C 0/3; `SBKN` A 2/3 B 2/3 (cars 1, 2) C 0/3; `SPLKN`, `SPLSBKN` 0/3 x3 | `KN` FAIL, `SBKN` PASS |
| G-D2 | `D2_SBKN_1..3` driving-median OUT 3/3; `D2_SB_1`, `D2_KN_1` slip 1500-2000 OUT | FAIL |

The 13 tracks and `H_SBKN` were not run: the D2 gate ruled out a default proposal first.

## 2. Code (`b57af75dc`)

`Vehicle/VehiclePhysicsRun.cpp`: `ReassertKeepNormal()` (`MASHED_REASSERT_KEEP_NORMAL`); `ReassertContacts` keeps the state
and grounded-count writes and skips the three normal writes under it.

## 3. Evidence

`o_seed1.msd.watch.csv` (the writer), `nseed.py` / `pseed.py` (normal timelines), lk21's `recdiff1.py` / `newdiff.py` /
`posdrift.py`. Arms on `D:\MashedWT\u9754_data` (`MOVED_OFF_GIT.txt`); the 3.6 MB `o_seed1.msd` and its aistep stay there too.

## 4. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9754 RESOLVED; new U-9755 (the remaining call-0 drift sources); hooks.csv notes on `0046cc40`, `0046f6c0`, `0046d400`;
one CHANGELOG line.

## 5. Follow-ups

- U-9755: port `FUN_0046d400`'s spawn y (an RW line query, `FUN_00538c80` with callback `FUN_0046d3a0`) and re-run the drift
  gate; then the `+0x968` previous-pose half.
- `ReassertContacts` stays a port-only stand-in on the default; dropping its normal write is faithful but moves D2, so it
  belongs with the D2 owners' next pass.
