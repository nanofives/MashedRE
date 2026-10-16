# U-9672 residuals, 2026-10-16 (branch work/fe_residuals_b)

Original captures use a private copy of the install (MASHED_ROOT scratch gameroot, MASHED.exe hash B9977DAB... = the patched exe), own PIDs only.
Standalone runs: MASHED_GOTO / MASHED_NAV_KEYS (new `key*N` = hold N frames) in the same gameroot.

## A. Screens 20 and 23 (cursor -1) captured on the original, one real defect fixed
orig_scr_keys.py pushes the screen with FUN_0043d2a0(S,0) and presses d,u,d,e (statenav input path).

| screen | original | standalone before | standalone now |
|---|---|---|---|
| 20 | cursor 0 -> -1 -> -1 -> -1; confirm at -1: MenuEntryGet returns 0xff4c0000, depth 2 -> 1 (pop) | same | same |
| 23 | same keys; confirm at -1: 0xff4c0000, depth 2 -> 3, PUSH of screen 1 (table 0x5f6980) | pop | push 1 (depth 1 -> 2) |

Cause: ActionToScreen's 0xff4c0000 arm tests the slot kind == 0x17 (harvest L1156-L1165) but `NavSlot::slot_kind` was never written, so the
branch was unreachable. DAT_0067ed3c[depth*0x40] is the screen id the slot was pushed with (re/analysis/u9667_menu_tables/dump_push.json: rec[1]
== id on 33/33 pushed screens), so Nav_Select now passes `s.screen_id`. Files: orig_s20_a.json, orig_s23_a.json, sa_s20_fix.log, sa_s23_fix.log.
The post-pop screen differs by design: the original's depth-1 root is the title table 0x5f6900, the standalone has no title; orig_s20_root.json
shows UP/DOWN do nothing on that title slot.

## B. Refusal popups (FUN_0042bf30(msg,0,1,0x2d,0,0), single OK) now open
- 0xff420000, mode 10, DAT_0067ea98 / ea9c / eaa0 all 0 (harvest L954-L964): popup 0x24f, no launch. Standalone: modal step 52, logged
  `MODAL step=52 ok_body=0x24f`, the second confirm dismisses (`step=0`), the third reopens (sa_noopp.log; opponents set to 0,0,0 with the
  screen-24 rows, cursors 4/5/6 = ea98/ea9c/eaa0). Previously the launch was refused silently with an SFX.
- 0xff3c0000 (harvest L877-L902): FUN_0042bb60 verdict 0..3 opens 0xd6..0xd9 (verdict -1 opens nothing). Wired in Nav_Select; unreachable
  without a populated team table, not exercised in a run.

## C. Key repeat on hold (screens 6 and 7)
FUN_0042aff0 / FUN_0042b180 (0x0042b0ab.., 0x0042b25b..): while the direction is held and the slot kind is 6 or 7, timer += frame time, and above
[0x005ccac0] = 0.4 (0x3ecccccd) the timer is set to 0.2 and the call returns 1.
ORIGINAL (orig_hold_2500.json, save with column 1 unlocked on all 13 rows, DOWN held 2500 ms): row steps at +30, +421, +646, +852, +1081, +1288,
+1512, +1719, +1946, +2152, +2379 ms (first repeat 0.4 s, then about 0.21 s at 60 Hz).
STANDALONE (sa_hold.log, `d*160`): first repeat after 47 frames of about 120 fps (about 0.39 s), then a REPEAT every 203 ms (13 ticks of 15.6 ms; the
original's 13 frames of 16.7 ms give 217 ms). Same law, different tick quantisation. Implemented in exe_main.cpp (`holdRepeat`).

## D. Test gate
mashedmod\run_tests.bat: navsm, ruleengine, racemodes GREEN; menustr and badges FAIL only because this worktree has no original\TOASTART.
G-CTRL (run_h4_wt.ps1 F_NEW) after the changes: 991a5fff / be0d47ea / bc75b405.

## E. Still open, with bytes
- Arm 0xff1d0000 push-6 branch side writes (harvest L359-L367): DAT_007f1a1c from DAT_0067f0c0, FUN_00431d00 (0x00431d00: DAT_0067ea74=1, ea90=1,
  ea94=0, ea98=1, ea9c=2, eaa0=3, eaac=0, then FUN_00431b80 x3), DAT_0067ea78=0. Readers of 0x0067ea78: 0x0043b4b8, 0x0043bcfb (screen 6 overlay,
  FUN_0043af10) and 0x0042fd27, 0x0042fe55. The standalone keeps its own state for these; not ported.
- The team branch of that arm (FUN_0042b9e0, DAT_0067ea64 -> 0xf/0x10) is ported in exe_main (CarSlotAssign + the MASHED_PLAYERS shim), not in the pure
  routing function MenuNavSM::ActionToScreen.
- Refusal path of 0xff260000 (harvest L405-L409 -> LAB_0043f6d4): sets `_DAT_0067e840 = 0.0` and, for a confirm press (bVar2 false), jumps to
  LAB_0043fc9a. No sound call on that path. [0x0067e840] is read by FUN_0043c000 (TimerSlotTickDispatcher, 40 reads 0x0043c13d..0x0043c589) and decays by
  _DAT_005cd18c each frame (harvest L41); the overlay ramp it drives is not modelled. Where the original plays the menu confirm sound is not located.
- Rows 8..12 vs the standalone's 8 cup tracks, the Team Play heading address, the dev mode-0 gate: unchanged.
- The UP/DOWN arms are gated on `DAT_00898ab0 == 0` (harvest L1349, L1353). Writers 0x0042a924, 0x0042aaba, 0x00438e56, 0x00438e7b, 0x00439071,
  0x0043907c, 0x004390a0; FUN_0042ae10 returns 0 while it is non-zero and DAT_0067eab0 != 2 and DAT_0067e7c8 == 0. The standalone never locks input.
  Not decoded.
