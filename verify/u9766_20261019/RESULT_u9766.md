# RESULT u9766: car 1's torque `+0x9c0` at call 5 (2026-10-19)

Branch `work/u9766` from `work/u9765` `27ee18b78`. PREREG `PREREG_u9766.md` (`5b3909695`), amendment 1 with the knob
(`71d973bd2`). Exe `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\u9756_root`. References: o_wobj2 (unprobed), o_w9c0 (write
watch on `+0x9c0`), o_aq0 (A6a probe on wheel 0); both match o_wobj2 outside the torque-ring history. Model claude-opus-5-5,
account2.

## 0. Verdict

1. **Two causes, both in the standalone's arithmetic, both behind default-OFF knobs.** Together with u9764's
   `MASHED_ROTATE_LUT` and u9765's `MASHED_ORTHO_LUT` they hold car 1 equal to o_wobj2 in every physics field (position,
   velocity, torque, angular speed, body matrix) **through call 66** (`firstdiff.py`). Before this lane the first gap was the
   torque at call 5, velocity at 21.
   - **`lc0` sum order (call 5).** o_w9c0: A6a's tail writes `+0x9c0` at `0x004685a2`, A6b at `0x0046892b` writes half of it.
     The A6a probe on wheel 0 puts the first gap in block #4's `lc0` (`lc8` / `lc0'` 1 ulp off, `s0`, `s1` and the axis
     equal). The original sums `(+0x9d8 * s1 + +0x9d4 * s0) + +0x9dc * (inv * ld8)` (`0x0046814b`, `0x0046819f..0x004681c3`),
     which u9764's `MASHED_A6A_LC0=1` implements. On top of `RLO` it makes wheel 0's block #4 equal o_aq0 through call 10 and
     moves the torque gap to call 28.
   - **x87 precision in `RwMatrixRotate` (call 27).** The next gap was the steered wheel axis (`+0x220`, `+0x2e4`) at call 27.
     `Math/RwMatrixRotateInner.cpp` is an inline x87 transcription of `0x004c4a60..0x004c4b95` whose bit-identity was
     measured in the .asi, inside MASHED.exe, where the race thread runs at PC_24. The standalone runs it at the CRT's default
     precision. **`MASHED_ROTATE_PC24=1` (new, default OFF)** scopes `_PC_24` over `RwMatrixRotate`'s FSIN / FCOS / `1 - cos` and
     the inner build. With it, all fields hold through call 66.
2. **Not a default candidate.** On `RLOLCP` (all four knobs): (b) A 2/3, **B 2/3** (car 2 fails `c1_distinct 48 [17,46]`; car
   3, which failed on `RLO`, passes), C 0/3. D2 0/3: slip1500 `0.2008` in, slip2000 `0.2616` OUT, drvmed `1788.31` OUT, the
   same as `MASHED_ORTHO_LUT` alone (u9765). The D2 shift comes from the ortho knob.
3. **Next gap (new U-9767): call 67, the frame-start forward.** Through call 66 both sides' Phase 0 forward (`+0x9d4`, and the
   unsteered wheels' axis) equals the previous call's `+0x948`. At call 67 neither does: the original uses `-0.010067312`,
   the port `-0.00835652556`, while both dumps hold `+0x948 = -0.00666628964` at the end of call 66. Then 108 fields split
   at once (wheel 0 force x `220.9` against `376.9`). The integer AI controls stay equal until aistep row 99 (= record call
   98), so the AI does not cause it. [UNCERTAIN] which writer sets the call-67 frame-start pose; a write watch on `+0x9d4`
   (or the chunk / substep count at call 67) is the missing evidence.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` on both builds | PASS |
| G-LC0 | `B_XW0RLOLC_K1`: wheel 0 block #4 = o_aq0 through call 10 | PASS |
| G-REC | `RLOLC`: torque first differs at call 28 (was 5); `RLOLCP`: every physics field equal through call 66 | PASS |
| G-POS | `RLOLC`: velocity from 27, position from 34; `RLOLCP`: both from 67 | reported |
| G-B | `RLOLC` `24d6ebb3 / 09823a9a / d50fbd12`: A 2/3, B 2/3 (car 3 FAIL), C 0/3. `RLOLCP` `1b790a49 / f09e4998 / 246060fa`: A 2/3, B 2/3 (car 2 FAIL), C 0/3 | FAIL |
| G-D2 | `D2_RLOLCP_1..3`: slip1500 `0.2008` in, slip2000 `0.2616` OUT, drvmed `1788.31` OUT, H1 398/400, 1627-1631 lines | 0/3 FAIL |

13 tracks and G-CTRL were not run: G-B and D2 rule the stack out as a default.

## 2. Watch note

`--watch-write 9c0,9e8` froze car 1 (664 frames, 4 distinct payloads, every logged value 0). The same recipe with `9c0`
alone ran normally. Two other frozen runs (u9764, `2e4,2e8`) had the same signature. Check distinct payloads before reading
any watch CSV. The frozen run is in `D:/MashedWT/u9766_data/bad`.

## 3. Code / tools

`Math/RwMatrixRotate.cpp`: `MASHED_ROTATE_PC24`. Arm stacks `XW0RLO`, `RLOLC`, `XW0RLOLC`, `P24`, `RLOLCP`, `XW0RLOLCP` in
`run_arms.py`. Evidence in `D:/MashedWT/u9766_data` (`MOVED_OFF_GIT.txt`).
