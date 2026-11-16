# RESULT RWC-FRAME-LADDER-CHECK (run 51, 2026-11-16): GREEN for the ladder, one measured mismatch filed (the 0x0043d2a0 screen build)

Branch `orch/RWC-FRAME-LADDER-CHECK` from `orch/integration` 6e84c6dea. Top-down row (the only exe change is a log line in
the default-OFF frame-route stand-in). Self-test PASS on every core arm. G-CTRL / B / 12 captures: not run (top-down row).

## Verdict per transition

Ladder call index = the count of 0x0043dfd0 calls. Original: 0x0043dfd0 is called once per tick from 0x00492dce
(return 0x00492dd3, in 0x00492d30, itself called from 0x004922b3, return 0x004922b8); its first call is in tick 0, so
tick t = ladder call t+1. Core: the per-call log of `PortTop_RwcPhaseLadder_0043dfd0`.

| transition | original (tick / ladder call, writer) | core (ladder call, present count) | verdict |
|---|---|---|---|
| DAT_0067eca4 0 -> 1 | tick 0, `mov [0x67eca4], 1` at 0x00433249 (0x00433240, called from 0x004927a0, return 0x004927a5), before the first ladder call | 1 already at ladder call 1 (present 5); writer not watched on the exe | value matches at call 1 |
| DAT_0067eca8 fade-in | ticks 0..15: 0x00, 0x10 .. 0xf0, one write per tick at 0x0043e0c6 | calls 1..16: 0, 16 .. 240 | match, 16 of 16 |
| DAT_0067eca4 1 -> 2 | tick 16 = call 17: 0x0043e0c6 writes 0x100, 0x0043e0d9 writes 0xff, 0x0043e0e3 writes 2 | call 17 (present 21): fade 255, phase 2 | **match** (0 frames) |
| DAT_0067eca8 fade-out | ticks 17..47: 0xf7 .. 0x07, step -8 at 0x0043e112 | calls 18..48: 247 .. 7 | match, 31 of 31 |
| DAT_0067eca4 2 -> 3 | tick 48 = call 49: 0x0043e112 writes 0xffffffff (-1), 0x0043e120 writes 0, 0x0043e126 writes 3 | call 49 (present 53): fade 0, phase 3 | **match** (0 frames) |
| after phase 3 | no write of either word from tick 49 to the warp at tick 300 | ladder idle (phase 3) | match |

`LADDER_COMPARE.txt`: per-call (phase, fade) at the end of each call, **0 mismatches of 49 calls**. DAT_007f1004 is
0x3C888888 on both sides (original checkpoints tick 16 and 17, core every call from call 2; call 1 reads 0.0 on the core).
The stand-in's constants k1 = 1000.0 / k2 = -500.0 give the original's steps +16 / -8.

Within-call difference, no end-of-call effect: the original stores the unclamped sum first (0x100 at tick 16, -1 at
tick 48) and then the clamp (0xff, 0); the stand-in stores only the clamped value. The two original hits are 23 bytes
(pc 0x0043e0cc -> 0x0043e0e3) and 14 bytes (0x0043e118 -> 0x0043e126) apart. [UNCERTAIN] that no instruction between
those two stores reads DAT_0067eca8 (not disassembled in this row; test in the register).

## The mismatch: 0x0043d2a0(DAT_0067ecac, 0) screen build

At the 1 -> 2 write the original pushes 0x16 (the stack top at pc 0x0043e0e3 / 0x0043e0e9 is 0x16; DAT_0067ecac = 0x16 =
22 on both sides) and calls 0x0043d2a0. Original checkpoints tick 16 -> tick 17 (both runs, byte-identical in these
ranges):

| address | tick 16 | tick 17 | core, calls 1..49 |
|---|---|---|---|
| 0x0067e9f8 | 0 | 1 | 0 |
| 0x0067ed78 (+0..+0xc) | 0 0 0 0 | 005f6900 00000016 00000000 00000001 | 0 0 0 0 |
| 0x00898ac0 (+0..+0xc) | 0 0 0 0 | ff000000 00000001 00000000 00ffffff | 0 0 0 0 |

The .data diff tick 16 -> 17 also shows 0x0067ed78..0x0067edb4, 0x00898ac0..0x00898b24, 0x007d709c and 0x007d7100 changing
only on that tick. **The core never writes the screen the original's 0x0043d2a0 builds** (the declared deviation of the
stand-in: the hand menu builds its own screen stack). So the second half of the row's [UNCERTAIN] is settled as NO for
the original's globals: the original's level stack and item array stay zero on the core. Consequence: any matched reader of
0x0067e9f8 / 0x0067ed78 / 0x00898ac0 on the core frame route reads zeros where the original reads the screen-22 level.
[UNCERTAIN] which readers run on the frame route at phase 3 and whether the 80 of 82 matched draws (RWC-FRAME-CLUMP run 46
/ 49) depend on them (register line). The fix is the real 0x0043dfd0 body (PORT-TOP-BODY-43DFD0, handoff) or a 0x0043d2a0
body (54.3%), not a new row here: the L-168 exception covers the ladder timing, which this row measures as exact.

## Runs

Original: `re/frida/detorig.py --clock tick --boot --q0 0x40000000 --warp-tick 300 --warp-clock 300 --normalize
re/frida/warp_normalize.tsv --load-sleep-ms 0 --tick-cap 600 --watch 0x0067eca4,0x0067eca8 --watch-from install
--watch-stack 48 --ckpt-ticks 0,1,2,15,16,17,18,47,48,49,50,51,52,60,120` (the RO-67F0B8-CALLER recipe with a second
watch address), private root `D:\MashedWT\RWC-FRAME-LADDER-CHECK_root`, test-slot outside, mashed_lock inside. orig1 and
orig2: rc 0, 206 hits each (54 before the warp), **identical (tick, pc, values) across all 206 hits**; the watch files
differ only in tid / n / stack words. Hits from tick 301 on are 0x00432b0a / 0x00432b1f (0x00432ad0, race path after the
warp), out of scope.

Core: `verify/porttop_20261108/knob_boot.py` with `MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1
MASHED_RWC_FRAME_N=120 MASHED_OT_TIMER=1 MASHED_DET_FRAMES=600`: vs_ladder1, ddk_ladder1 (object build), vs_ladder2. All
three end `boot ends after present 120 on the RW core (exit 0x5257F001)`, identical ladder lines (49 each). Self-tests on
every arm: `[portstate-image] self-test PASS (0 of 8 checks failed)`, `[OT_CODEPTR] self-test PASS (0 of 367 checks
failed)`, binding `self-test PASS (0 of 11 checks failed)`, `[RWC-DEVICE] selftest PASS (0 fail)`.

Builds: VS2022 `=== Build OK ===`, object build (`MASHED_DDKOBJ=1`) `=== Build OK ===`. `ddkobj.py check`: IDENTICAL
1561/1599, DIFFERENT 22, not-in-map 16, refusals 11. This row changes no matched unit (the edit is in
`Match/PortTop_StandIns05.cpp`, a VS2022 stand-in TU, not in the DIFFERENT list).

## Change

`mashedmod/src/mashed_re/Match/PortTop_StandIns05.cpp` `PortTop_RwcPhaseLadder_0043dfd0`: one log line per call while
the phase is 1 or 2 (call index, D3D9 present count, phase before / after, fade, DAT_007f1004, DAT_0067ecac, DAT_0067e9f8,
four dwords of 0x0067ed78 and of 0x00898ac0), at most 96 lines. Log only, frame route only (default OFF). No behaviour
change, no tracker edit.

## Files

Committed: `ladder_compare.py`, `LADDER_COMPARE.txt`, `ORIG_WATCH_prewarp_orig1.jsonl` (54 pre-warp hits, fuzzy stack
only), `CORE_LADDER_vs_ladder1.txt`, `RAW_SHA256.txt`. Raw data `D:/MashedWT/RWC-FRAME-LADDER-CHECK_data` (orig1/,
orig2/ with watch.jsonl, ckpt*.data.bin, ticks.bin; orig*.log, *_ladder*.log, build logs), sha256 in `RAW_SHA256.txt`.
