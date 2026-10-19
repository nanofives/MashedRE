# RESULT u9152: the +0x928 / +0x968 double buffer (2026-10-18)

Branch `work/u9152` from `race/first-frame-parity` `a5a77d17c`. Model claude-opus-5-5, account2. PREREG `PREREG_u9152.md`
(`882d3a1d4`, knobs in the same commit), amendment 1 (`74e5e2ecd`), each committed before the arms it governs. Exe
`b5efa344` (`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\u9756_root`. Reference o_wobj2 (regime B, car 1 record).

## 0. Verdict

1. **The original's double buffer is decoded** (PREREG section 0, disassembly): A9 writes dst = `[+0x9ac]` from src =
   `[+0x9a8]` (`0x00470a6f..0x00470ad3`), the solver reads and corrects dst only (`0x0046f6d8`), a settled substep swaps the
   selectors (`0x00470c31..0x00470c41`), exhausted retries skip the swap (`0x00470ab3` -> `0x00470c47`), and an odd frame
   ends with `+0x9a8 = 0` and slot 0 copied over slot 1 (`0x00471195..0x004711af`).
2. **`MASHED_RING_DBUF` (default OFF) reproduces the half.** Car 1's `+0x968..+0x9a0` equals o_wobj2 at window call 0
   (`+0x99c` 0.468632549, `+0x9a0` -0.906101644) and at call 1 (`+0x970` 2.17004508e-5 vs 2.17004526e-5 with UP_TRINORMAL).
   Only pad dwords differ (`+0x954 / +0x964 / +0x984 / +0x9a4`, original 9.4e-29 / int 5 / int 3 / 9.4e-29, port 0).
3. **The U-9152 note's premise is refuted: the half does not drive car 1's drift.** The solver never reads the stale half
   (it reads dst after A9 wrote it), and the drift is bit-identical with and without the knob through call 27 (call 21
   1.202e-4, look flip unchanged). The u9756 note compared the half at frame end, not at the solver's read.
4. **The half's only in-substep readers are car-car**: the proximity test (car i's `+0x9a8` translation, pre-substep under
   the knob) and `FUN_00469df0` (`CarCarContacts.cpp:57-58`, `:216-218`). Under the knob the port pairs car i's pre-substep
   pose with car j's frame-start pose, because it steps each car's whole frame before the next car; the original pairs it
   with car j's post-substep-1 pose on substep 2. So the knob is half of a faithful car-car pairing, and on its own it moves
   contacts: car-car `ret=1` 34 -> 22 over the S run, B car 1 PASS -> FAIL on the default. **Not a default candidate.**
5. **`MASHED_RING_DBUF_DISCARD`** (exhausted retry keeps the src pose) never fires in these arms (`DBX` = `DB` on K0 / K1 /
   K26).
6. **Next driver of car 1's drift: the world forward `+0x9d4`.** At call 0 the remaining float diffs are `+0x9cc` (up y,
   0.999988973 / 1, 0.999989033 with `MASHED_UP_TRINORMAL`) and `+0x9d4` (forward x, -8.74227766e-08 / -4.37113883e-08,
   exactly 2x). The port writes the forward as `(cos yaw, 0, sin yaw)` (`Vehicle/VehiclePhysicsRun.cpp:1209 / 1411 / 1774`);
   A6a's lateral-velocity term reads it (`Integrate2.cpp:671-672`), and at call 1 the velocity `+0x9b0` differs
   (-0.00108456612 / -0.00108465552) with omega `+0x9c0`. The original's writer of `+0x9d4` is not identified here
   (`scenario_launch.py --watch-write` on `+0x9d4`). New U-9758.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `c7b40cf2 / 5b9905d8 / f680a529`; `H_DEF` `b7534619 / fb690fb1 / d3a20f4c` | PASS |
| G-HALF | `SPLSBKNF60DBR1` `+0x968..+0x9a0` = o_wobj2 at call 0 (pads aside) | PASS |
| G-DRIFT | `SPLSBKNF60DB` = `SPLSBKNF60` through call 27 (1.202e-4 at 21) | unchanged, as predicted (H2) |
| G-B | `DB`: A 2/3, B 1/3 (car 1 PASS -> FAIL), C 0/3; `DBX` identical | FAIL |
| contacts | S stack, ai_override onsets: base 1+3@169, 1+2@204; `DB` 1+3@169, 1+2@203; original 1+3@159, 2+3@264 | in-window unchanged |
| G-CTRL | `H_DB` `6d7ec881 / 27519c1a / a6665a88` | reported |
| UT arms | `SPLSBKNF60UTR1`: `+0x9cc` 0.999989033 (orig 0.999988973), drift unchanged | reported |

## 2. Code

`Vehicle/VehiclePhysicsRun.cpp`: `RingDbuf` / `RingDbufDiscard` / `RingSwapSelectors` / `RingFrameEnd`;
`SyncContactRingMatrix` skips the src half under the knob; the substep loop tracks `committed` (both `break` sites) and
swaps or discards after the retry loop; `RingFrameEnd` after the fix-L chunk loop. Knobs off: byte-identical dumps.

## 3. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9152 note (the double buffer decoded and ported behind a knob; the half is not the drift driver; its readers are car-car);
new U-9758 (the forward `+0x9d4`); hooks.csv notes on `004709a0` and `00470c70`; one CHANGELOG line.

## 4. For the next lane

- U-9758 first: find the original's `+0x9d4` writer, port it behind a knob, re-run `posdrift.py` on `SPLSBKNF60`.
- Car-car fidelity needs per-substep interleaving across cars (car j's pose after the same substep count) before
  `MASHED_RING_DBUF` can be judged as a default; the B contact schedule (lane 5) runs through the same path.
