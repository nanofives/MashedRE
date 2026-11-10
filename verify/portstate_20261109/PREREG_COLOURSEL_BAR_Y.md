# PREREG PORT-COLOURSEL-BAR-Y (2026-11-09)

Step: PORT-COLOURSEL-BAR-Y, parent cmw7xu1960f37p41557tefpnd. Branch `orch/PORT-COLOURSEL-BAR-Y` from
`orch/PORT-COLOURSEL-ICON` 69e213e1d. Committed alone, before any knob arm.

## 1. Cause (static, MASHED.exe.unpatched, with the PORT-COLOURSEL-ICON trace)

The original's colour-select row layout has two paths:

- `0x004335f0` 0x00433726..0x0043372d: `call 0x00430760`. Non-zero (game mode 2/10/3/4/5, the cup flow) takes the
  **single-row** draw. Zero jumps to 0x00433a76 (the multi-row draw).
- Single row: the bar top is `[0x0067f0c8] - [0x005cd8d8]` = record y - 13.0 (0x00433817..0x0043381d). The icon is at
  `[0x0067f0c8] - 1.0` (0x00433982..0x00433988, 0x005cc320 = 1.0).
- `0x004332a0` sets that record y to the constant **220.0** every frame on its 0x00433443 path (`[0x007f1a0c] != 0x1000`,
  0x004332bc), at 0x00433428 (`mov [0x0067f0c8], 0x435c0000`).
- So in the cup flow: one bar with its top at 207.0 (640x480 virtual), icon y 219.0, record y 220.0. The trace at walk_04
  (PORT-COLOURSEL-ICON arm 1) read 0x0067f0c8 = 220.0 and the icon y argument 219.0. The original's capture has the bar
  border rows at 207..232.

The exe (`exe_main.cpp`, colour-select rows block) draws `g_input_active_slots` rows **bottom-anchored**: last bar top
261.25 virtual (fitted to a 4-row capture of the multi-row path), and the number y from the multi-row law
(`Q_ListBaseY`, 240 for one row). It does not branch on the cup flow. On this machine it draws one row at 261.25, about
54 px below the original's 207.

## 2. Change (default OFF)

Knob **`MASHED_COLOURSEL_BAR_Y`** (`1` = on, read once), exe only. In the colour-select rows block, when the knob is
on and the game mode word (`Nav_GameModeWord()`) is 2, 10, 3, 4 or 5 (0x00430760's set):

- draw one row (the keyboard profile's), with bar top `(220.0 - 13.0) * kVScale`
- number y = 220.0 (the record y, 0x00433ecc)
- the icon keeps its offset from the bar (unchanged formula `iy = by + (bh - ish)/2`)

Otherwise the block is unchanged (multi-row path, bottom-anchored). Nothing else changes.

## 3. Arms

2. Default gates, knob OFF: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3, unchanged.
3. Knob-ON gates (stack `CSB`, and `CSIB` = this knob + `MASHED_COLOURSEL_ICON=1`): reported.
4. walk_04, hand path, four runs: OFF, BAR_Y ON, ICON ON, both ON. Measured against the original (`d40b38fd`): the bar
   border rows at x 560 (original 207..232), and the red pixels of the icon in column 1. Prediction: BAR_Y ON puts the
   border rows at 207..232. Both ON puts the icon red in column 1 in that row. Only walk_04 changes among the 12
   captures.
5. Parity harness: `re/frida/menu_draw_burst.py --screen 4` reaches screen 4 by a direct `FUN_0043d2a0(4, 0)` push. That
   skips LAB_0043f468, so it leaves the game mode and 0x0067f0c0 at their title values, and the original then shows the
   multi-row path. A draw-list diff of this screen in the cup-flow state needs an input-driven capture, which the harness
   does not have. The RESULT records that gap and does not claim a GREEN draw-list diff.

## 4. Rules

As before: mashed_lock inside test-slot, no build during a PRIORITY-HOLD or SOLO, no default flip, raw data in
`D:/MashedWT/PORT-COLOURSEL-BAR-Y_data` with SHA-256 in the RESULT.
