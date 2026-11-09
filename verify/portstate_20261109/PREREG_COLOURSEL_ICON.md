# PREREG PORT-COLOURSEL-ICON (2026-11-09)

Step: PORT-COLOURSEL-ICON, parent cmw7xu1960f37p41557tefpnd. Branch `orch/PORT-COLOURSEL-ICON` from `orch/integration`
bba25cb03. Committed alone, before any knob arm. Arm 1 (original-side trace) ran first, as the brief makes it the input.

## 1. Arm 1 result (done, `coloursel/icon_trace.py`, `coloursel/watch_addr.py`)

On the original at walk_04 (colour select, no left/right pressed):

- 0x004335f0 draws player 1's icon by calling 0x0042bcb0 at 0x004339d3 with (profile 0, x 170.0, y 219.0, tint
  0xff3d3a98, flag 0) (40 calls, all identical). The globals it reads: choice **0x0067f0c0 = 1** (cmp 6 at 0x00433979, else
  the tint table index), x 0x0067f0c4 = 170.0, y 0x0067f0c8 = 220.0, profile 0x007f1a14 = 0.
- 0x004332a0's tail recomputes x and y every frame from the choice: `x = choice*0x46 - 0x46 + [0x005cd90c]`
  (0x004333fd..0x00433439), `y = 220.0` (0x00433428). Watch hits at 0x00433432 / 0x0043343f only.
- The one write of 0x0067f0c0 from boot to the colour select: `mov [0x0067f0c0], 1` at **0x0043f4d1**, right after
  `push 4; call 0x0043d2a0` (the push of screen 4) at 0x0043f4c3. This is the tail of LAB_0043f468 (0x0043f468..0x0043f4db),
  shared by the nav actions 0xff3d0000 and 0xff400000 (the Challenge Cup confirm). Backtrace 0x0043f4db <- 0x00492dd3 <-
  0x004922b8.

The exe draws the icon from 0x0067ea98 instead (`exe_main.cpp:6213-6222`, with the same law, x = 170 + (sel-1)*70 and
`kSwatch[sel-1]`). That address is 4 after the matched 0x004924f0 (FUN_00431d00, equal to the original) and 0 on the
hand path. So the exe draws column 4 (yellow) or grey, where the original draws choice 1 (red, column 1).

## 2. Change (default OFF)

Knob **`MASHED_COLOURSEL_ICON`** (`1` = on, read once), exe only:

1. `Frontend/MenuNavSM.cpp` `ApplyActionGameMode`: for 0xff3d0000 / 0xff400000, `*(int*)0x0067f0c0 = 1` (0x0043f4d1).
2. `exe_main.cpp` icon draw: `sel` = `*(int*)0x0067f0c0` instead of `*(int*)0x0067ea98`.

Also, with no behaviour change, the stale comment at `exe_main.cpp:189-190` (it says the span is not mirrored to
0x007f0a40, but `CopySpanAndRederive` does mirror it, PORT-SAVE-SPAN-MIRROR) is replaced by a correct comment of the
same two lines.

Not in scope, and recorded: the left/right handler. The exe moves 0x0067ea98 on left/right (`exe_main.cpp:2779-2802`,
modelled on 0x00440337). Which address the original's left/right moves on this screen is not traced. With the knob ON
the icon therefore stays at the entry choice when left/right is pressed. That blocks a flip, not this arm.

## 3. Arms

2. **Default gates, knob OFF**: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3. Pass bar: unchanged.
3. **Knob-ON gates** (stack `CSI`): reported.
4. **walk_04** (nav demo, 04D settings baseline): hand OFF / hand ON / matched (`MASHED_ORIG_TOP=1 MASHED_TOP04D_DATAZERO=1`)
   OFF / matched ON. Checked against the original's walk_04 `d40b38fdd6a30c50` on player 1's icon: column (x 148..195 red
   pixels in the bar row) and tint. Prediction: ON shows the icon in column 1 tinted red, on both paths. The bar's y
   (row PORT-COLOURSEL-BAR-Y) is not part of this check.

## 4. Rules

Same as the earlier steps: mashed_lock inside test-slot for every launch, no build during a PRIORITY-HOLD or SOLO, no
default flip, raw data in `D:/MashedWT/PORT-COLOURSEL-ICON_data` with SHA-256 in the RESULT.
