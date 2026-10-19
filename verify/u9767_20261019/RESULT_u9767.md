# RESULT u9767: car 1's frame-start forward at call 67 (2026-10-19)

Branch `work/u9767` from `work/u9766` `b34dbbcfa`. PREREG `PREREG_u9767.md` with the knob (commit after `b34dbbcfa`). Exe
`MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\u9756_root`. Reference o_wobj2 (unprobed). Model claude-opus-5-5, account2.

## 0. Verdict

1. **Cause found: the steer-0 heading assist read a stale forward.** Call 67 is car 1's first steer-0 frame, and
   `FUN_0046c7d0` (`Race/HeadingAssist.cpp`, default ON) rotated the pose on both sides, by different angles: the original's
   A5 started from `-0.010067312`, the port's from `-0.00835652556`, both from `+0x948 = -0.00666628964`. The assist reads its
   heading from `+0x9d4 / +0x9dc` (`[edi + 0x881f74]`). In the original that field is refreshed during the AI tick by the
   getter `FUN_0046d510` (call `0x0046d53a`: `(0,0,1)` at `0x00614708` through the vectors slot with the slot-0 matrix
   `rec + 0x928`, i.e. its `at` row; 12 callers in the AI step `0x004143d5..0x0041849f`, plus `0x0042536f / 0x00425467 /
   0x00425624 / 0x0042562f / 0x0044833f / 0x0047ed35`). The port writes `+0x9d4` only at A5's frame start, so its assist read
   the previous frame's forward. The `+0x9d4` dump difference that earlier lanes excluded as read time is what the assist
   consumes.
2. **`MASHED_HEADING_FWD_LIVE=1` (default OFF)** refreshes `+0x9d4..+0x9dc` from `rec + 0x948..+0x950` at `AssistCar`'s entry.
   On the four-knob stack (`RLOLCPF`) car 1 equals o_wobj2 in every physics field **through call 104** (was 66).
   [UNCERTAIN] the knob assumes `FUN_0046d510` ran for the car earlier in the frame; its per-frame call set is not measured.
3. **Not a default candidate.** (b): `RLOLCPF` A 2/3, **B 0/3** (car 1 `c1_distinct 16 [17,46]`, car 2 `48`, car 3 fails), C 0/3.
   The knob alone on the default (`F`): A 2/3, B 2/3 (car 3 `c1_distinct 50 [17,46]`), C 0/3. Car 1's regime-A
   `abs_steer_median` goes from `1.5` to `5.5` on the stack (band `[7,23]`).
4. **Next gap (new U-9768): the AI lookahead target at call 104.** The original's look point jumps from
   `(-1.271637, -14.80128)` to `(-1.009332, -20.2197)` at aistep row 105 (record call 104), and its steering error
   (`0x008032d8 + v*0x14`) goes from `359.3607` to `357.803`, so it steers (`c1 = 19`). The port keeps the near point
   (`-1.267988, -14.92525`) and does not steer. Own position, `curv`, `march_n / march_idx0` and the spline index are equal.
   Physics then splits at call 105.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` | PASS |
| G-REC | `XW0RLOLCPF`: every physics field = o_wobj2 through call 104 (was 66) | PASS |
| G-B | `RLOLCPF` `dca1ecf9 / e96df243 / 15c558be`: A 2/3, B 0/3, C 0/3. `F` `b1c5840b / 901059b0 / 11dce6e7`: A 2/3, B 2/3, C 0/3 | FAIL |

D2, 13 tracks and G-CTRL were not run: G-B rules the knob out as a default.

## 2. Watch note

`--watch-write 9d4` froze car 1 in both runs (325 and 333 frames, about 13 fps, 4 distinct payloads). The traps land at
`0x004dc481`, inside the SSE vectors slot `0x004dc410`. The two-slot watches that froze earlier hit other sites. Only the
writer sites of these runs are used here; their values are not. Frozen runs are in `D:/MashedWT/u9767_data/bad`.

## 3. Code / tools

`Race/HeadingAssist.cpp`: `MASHED_HEADING_FWD_LIVE`. `verify/u9767_20261019/aictl.py`: first car-v AI control / input
difference, original aistep CSV against the port step CSV. Evidence in `D:/MashedWT/u9767_data` (`MOVED_OFF_GIT.txt`).
