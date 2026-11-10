# PREREG PORT-COLOURSEL-LEFTRIGHT (2026-11-09)

Step: PORT-COLOURSEL-LEFTRIGHT, parent cmw7xu1960f37p41557tefpnd. Branch `orch/PORT-COLOURSEL-LEFTRIGHT` from
`orch/PORT-COLOURSEL-BAR-Y-FLIP` 5775e1ad1 (MASHED_COLOURSEL_BAR_Y default ON, L-110 a). Committed alone, before any knob arm.
Arm 1 (the original-side trace) ran first, as the brief makes it the input.

## 1. Arm 1 result (done)

Static (MASHED.exe.unpatched): in 0x004332a0's cup-flow path (`[0x007f1a0c] != 0x1000`, 0x004332bc), 0x00433443..0x004334da
loops the 4 entries of the 0x007f1a14 list (stride 0x10). An entry is skipped if it is -1 or if `0x0040e470(i) == 2`.
For each remaining entry, with player p = `[entry]`:

- left edge `0x007f1044 + p*0x4c` set and processed `0x007f1504 + p*0x4c` clear: if `[0x0067f0c0] > 1`, decrement
  (0x00433486..0x00433487). At 1 it writes `0x0067ed70 = 1` instead.
- right edge `0x007f1045 + p*0x4c` set and processed `0x007f1505 + p*0x4c` clear: if `[0x0067f0c0] < 6`, increment
  (0x004334bb..0x004334bc). At 6 it writes `0x0067ed70 = 1` instead.

Dynamic, `re/frida/menu_draw_burst.py --nav` (HB-MENUBURST-NAV), root PORT-TOP-04D-WALK04's, `coloursel/run_lr_orig.sh`:

| nav after confirm x3 | 0x0067f0c0 | 0x0067f0c4 (x) | 0x0067ea98 |
|---|---|---|---|
| right | 2 | 240.0 | 4 |
| right x6 | 6 | 520.0 | 4 |
| right, right, left | 2 | 240.0 | 4 |
| left | 1 | 170.0 | 4 |

So the original's left/right on this screen writes 0x0067f0c0, clamped to 1..6, and does not move 0x0067ea98.

Bar geometry of the single row on the original (HB-MENUBURST-NAV draw list, burst `cs_f0`):

| draw | rect | colour | caller |
|---|---|---|---|
| fill | x 48, y 207, 534 x 26 | 7ff06e14 | 0x0043387c |
| left border | x 46, y 207, 2 x 26 | ffb45010 | 0x004338b5 |
| top border | x 46, y 207, 538 x 2 | ffb45010 | 0x004338ec |
| bottom border | x 46, y 231, 538 x 2 | ffb45010 | 0x00433926 |
| right border | x 582, y 207, 2 x 26 | ffb45010 | 0x00433971 |

The exe today (BAR_Y on) draws fill x 46.2 538.1 x 26.2, top x 46.2 y 207 h 1.0, bottom y 232.2 h 1.0, and no left or right border.

## 2. Changes (both default OFF)

1. **`MASHED_COLOURSEL_ICON`** (existing knob, still default OFF). On screen 4, left/right also moves 0x0067f0c0 with
   the original's law: decrement if > 1, increment if < 6, edge-triggered. The exe's 0x0067ea98 colour cursor and
   FUN_00431b80 call stay unchanged, because they feed the race livery (`exe_main.cpp:2257`). The original commits the
   colour from 0x0067f0c0 through 0x007f1a1c at the confirm (0x0043e56c..0x0043e57e, U-9672). Porting that commit is
   not part of this step, and with the knob on the livery can therefore differ from the icon. The RESULT reports this.
2. **`MASHED_COLOURSEL_BORDER`** (new, default OFF, exe only). In the single-row path (BAR_Y on and cup flow), draw the
   five rects of the table above in that order, instead of the exe's 3-quad bar.

## 3. Arms

2. Gates with both knobs at their defaults (ICON off, BORDER off, BAR_Y on): G-CTRL `8284be46/67931834/31fe043e`, B
   `1d5bf4f9` x3 (`merge_train.json` refs win). Pass bar: unchanged.
3. Gates with ICON + BORDER on: reported.
4. walk_04 (hand path, the default now has BAR_Y on): default, ICON, BORDER, ICON+BORDER. Prediction: BORDER on gives
   border rows 207..208 and 231..232 at x 560, and left/right borders at x 46..47 / 582..583. Only walk_04 moves.
5. Draw-list diff (HB-MENUBURST-NAV recipe: the original `--nav confirm,confirm,confirm` against the exe
   `MASHED_DBG_DRAWSTREAM` at the colour select) with ICON + BORDER on. Prediction: the five bar rows match (MISMATCH or
   MISSING today). The tile rows are out of scope.
6. Left/right on the exe: nav demo cannot press left/right, so the check is the exe's log line of 0x0067f0c0 after a
   scripted right press, if a driver exists for it. If none exists, the RESULT says so and relies on the code read.

## 4. Rules

As before: mashed_lock inside test-slot, no build during a PRIORITY-HOLD or SOLO, no default flip, raw data in
`D:/MashedWT/PORT-COLOURSEL-LEFTRIGHT_data` with SHA-256 in the RESULT.
