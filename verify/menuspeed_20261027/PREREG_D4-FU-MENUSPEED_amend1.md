# PREREG amendment 1, D4-FU-MENUSPEED: the frame clock's input (default-OFF knob)

Committed alone, after arms N0-N2, A0-A4, the gates and the parity run, and before any arm below.

## 1. What the arms so far showed

`orig_anim_timing.py` (observe-only attach on `0x004325c0`, push screen 1, private root) logged 230 calls over
4041 ms: median gap 17 ms, and `DAT_007f1004` = `0.016666666` (one tick) on **all 230**. Item rows (`0xff040000`)
settle at call 16 = 270 ms, the back row (`0xff000000`) at call 40 = 781 ms, all frozen at call 90 = 1632 ms.
The call counts are the per-tick model's (16 / 40 / 90, navsm N1). The wall clock is not:
D4-FU-MENUANIM's 800 ms / 2000 ms assumed one tick per 50 ms of wall time.

Cause, from the disassembly: `FUN_00493390` stores the constant `0x32` (50) into `DAT_007f1000` at `0x004933d5`
on every call. `FUN_00493480` calls it (`0x00493480`) and then reads `DAT_007f1000` (`0x00493485`), so its input
is always 50: one tick per frame, `DAT_007f1004 = 50 * 1/3000` (`0x0049351f..0x0049352b`). The wall clock
(`FUN_004950b0` at `0x00493392`) only feeds `DAT_007f0ffc` / `DAT_007f1010` / `DAT_007f1030`.

The exe instead feeds real elapsed ms (`exe_main.cpp` frame-clock block, clamped to 50), which at ~17 ms frames
gives one tick every third frame. So at the new default speed 1.0 the exe menus run about 3x slower than
the original at 60 fps (item rows ~800 ms against 270 ms). Before this step (x6) they ran about 1.8x faster.

## 2. Change (default OFF)

`MASHED_MENU_CLOCK_FAITHFUL=1`: the exe passes 50 to `Nav_FrameClockUpdate` every frame, the value `0x004933d5`
stores. Unset: today's elapsed-ms input. `g_wall_dt` is untouched. Debug `MASHED_MENU_ANIM_LOG=<path>` (default
off): one line per `Nav_AnimTick` call with QPC ms since the first call after the last screen change, the dt,
and tag/slide/type of records 0..6. Same fields as `orig_anim_timing.py`.

## 3. Arms and predictions (exe, private root, `MASHED_GOTO=8`, 60 fps cap)

| Arm | Env | Prediction |
|---|---|---|
| E0 | log only | dt is 0 on about 2 of 3 calls. Item rows settle at tick 16 (~frame 48, ~800 ms), back row tick 40 (~2000 ms) |
| E1 | `MASHED_MENU_CLOCK_FAITHFUL=1` | dt `0.016666666` on every call. Item rows at call 16, back row at call 40, frozen at call 90. Wall ms within 20% of the original's 270 / 781 / 1632 |
| E2 | `MASHED_MENU_CLOCK_FAITHFUL=1 MASHED_MENU_ANIM_SPEED=6` | item rows at call 3, back row at call 7 (navsm x6 per-tick model) |

Gates re-run on the rebuilt exe (knob OFF): unchanged, `5943c5f1/364350ae/52052890`, B `3a03670c/e85ea6fb/53778664`.
The knob's default flip is filed as a decision row, not done here.
