# RESULT PORT-COLOURSEL-ICON (2026-11-09)

PREREG: `PREREG_COLOURSEL_ICON.md` (committed alone, 334af4519). Branch `orch/PORT-COLOURSEL-ICON` from `orch/integration`
bba25cb03. Code commit 637b0a510.

## 1. Answer

**GREEN.** The field is the choice at **0x0067f0c0**:

- 0x004335f0 reads it at 0x00433971 for player 1's keyboard icon. 0x004332a0's tail derives the icon's x from it
  (0x004333fd..0x00433439).
- On the colour-select entry the original sets it to 1 at **0x0043f4d1**, the tail of LAB_0043f468, right after the push of
  screen 4. That path is taken by the actions 0xff3d0000 and 0xff400000.
- Choice 1 is the red tile in column 1: 0x0042bcb0 receives x 170.0 and tint 0xff3d3a98.

The exe drew from 0x0067ea98 instead. Knob `MASHED_COLOURSEL_ICON=1` writes 0x0067f0c0 = 1 on those actions and reads
it for the icon. walk_04 then shows the icon red in column 1 on both the hand and the matched 0x004924f0 path, as the
original does. It moves only walk_04 (one capture of 12), so it ships default OFF. Gates are unchanged with the knob
OFF and ON.

## 2. Arm 1: the original (`coloursel/icon_trace.py`, `coloursel/watch_addr.py`, one boot each)

| measurement | result |
|---|---|
| 0x0042bcb0 args, 40 calls at walk_04 (return 0x004339d8) | (0, x 170.0 `0x432a0000`, y 219.0 `0x435b0000`, tint `0xff3d3a98`, 0), all 40 the same |
| globals at those calls | 0x0067f0c0 = 1, 0x0067f0c4 = 170.0, 0x0067f0c8 = 220.0, 0x007f1a14 = 0 |
| writes to 0x0067f0c0/c4/c8 during 1.5 s at walk_04 | 0x00433432 / 0x0043343f only, the per-frame x/y stores of 0x004332a0 (`y = 220.0` at 0x00433428) |
| writes to 0x0067f0c0 from boot to walk_04 | one: value 1, pc after the store 0x0043f4db (store at 0x0043f4d1), on the single player -> colour select step, backtrace 0x0043f4db <- 0x00492dd3 <- 0x004922b8 |

0x0043f468..0x0043f4db: `[0x007f0fe8] = 0`, `0x0067f184 = 1` (0xff3d0000) or `0` (0xff400000), `call 0x0042f6b0`,
`FUN_0046dc00(i, 1)` for i = 0..3, `0x0067ea64 = 0x0067ecdc = 0x0067f17c = 0x0067f180 = 0`, `FUN_0043d2a0(4, 0)`,
`0x0067ea68 = 0`, `0x0067f0c0 = 1`.

## 3. Change (637b0a510)

- `Frontend/MenuNavSM.cpp`: `ColourSelIcon_On()` (env `MASHED_COLOURSEL_ICON`, read once). `ApplyActionGameMode` writes
  `*(int*)0x0067f0c0 = 1` for 0xff3d0000 / 0xff400000 when it is on (0x0043f4d1).
- `exe_main.cpp` icon draw: `sel` reads 0x0067f0c0 when the knob is on, 0x0067ea98 otherwise. The x / tint law is
  unchanged (it already equals 0x004332a0's: x = 170 + (sel-1)*70).
- `exe_main.cpp:189-190`: the stale comment ("our port does not mirror the span back") is replaced. It is two lines,
  as before.
- run_arms.py: stack `CSI`.

Build `8d627cd2` (`MASHED_NO_DEPLOY=1`): rva-lint `NEW=0`, Build OK.

## 4. Gates (one run, private root `D:/MashedWT/PORT-COLOURSEL-ICON_root`)

| arm | hashes |
|---|---|
| H_DEF / H_CSI | `8284be46 / 67931834 / 31fe043e` both |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 |
| B_CSI_K0 / K1 / K26 | `1d5bf4f9` x3 |

## 5. walk_04 (nav demo, 04D settings baseline, `coloursel/run_csi.py`)

Red pixels (r > 150, g < 70, b < 70) and yellow pixels in the icon boxes of the bar row (y 205..235 on the original,
258..290 in the exe, the bar sits about 55 px lower in the exe, row PORT-COLOURSEL-BAR-Y):

| run | walk_04 sha256/16 | red, column 1 | red, far left | yellow, column 4 |
|---|---|---|---|---|
| original (WALK04) | `d40b38fdd6a30c50` | 112 | 0 | 0 |
| hand, OFF | `2871487f1fcef51c` | 0 | 0 | 0 (grey icon, far left) |
| hand, ON | `4d6dac65bf13ffc4` | **97** | 0 | 0 |
| matched, OFF | `da29bdef4b0bc8f1` | 0 | 0 | 305 |
| matched, ON | `4d6dac65bf13ffc4` | **97** | 0 | 0 |

With the knob ON, hand and matched give the same file. In both pairs (hand OFF/ON, matched OFF/ON), the other 11
captures are byte-identical. The 97 against 112 red pixels comes from the icon's row: the icon sits on the exe's lower
bar, over a different background. Pixel identity with the original is not expected until BAR-Y is fixed.

## 6. Limits, and the flip

- Left/right on the colour select: the exe moves 0x0067ea98 (`exe_main.cpp:2779-2802`). With the knob ON the icon reads
  0x0067f0c0, so it does not follow left/right. [UNCERTAIN] which address the original's left/right writes on this
  screen. Missing evidence: a watch on 0x0067f0c0 and 0x0067ea98 while left/right is pressed on the original. That
  blocks a flip.
- Joypad profiles (other rows) are still drawn at the choice-0 position, as before.
- Decision for the lead: a flip after the left/right trace. Options: (a) trace left/right next and port it under the
  same knob, then flip, or (b) keep the knob OFF. Recommendation **(a)**.

## 7. Commits

| commit | content |
|---|---|
| 334af4519 | PREREG (alone) |
| 637b0a510 | the knob, the comment fix, `CSI`, `coloursel/icon_trace.py`, `coloursel/watch_addr.py` |
| this commit | `coloursel/run_csi.py`, this RESULT |

## 8. Raw files (`D:/MashedWT/PORT-COLOURSEL-ICON_data`, not committed)

| file | sha256 |
|---|---|
| icon1.json / icon1.log | 96e90f96a75adcc9d8f19ad57fe13c3f4e50f5b66fd01a015535e6f0a7049439 / c366fbef963443f1147cb3a7c68f998669be7ebbbae3823b31c20b56982fe01e |
| watch_f0c0.json / watch_f0c0.log | f8b071f8ee815c840f5c28a63c08774a1af44867878f49964b8b497984479b75 / e618504216ae336cf6a19c7de355ce0cb0f910b5ac895cd153c4c437b90c2a25 |
| exe/mashed_re.exe | 8d627cd2a6658c1d650c8d309e80317a4601f97643d6ad9cd77f2addfe61a6b3 |
| build.log / run_csi.log | 5b1471e4e7f91423ec40c3ed912abaec2ed0e70991fb585a6967263a43f0933e / aab3c8c96ffdc5854716be76a5b82506af09e37731470b531982bf64adf4e311 |
| arms/H_DEF.step.csv, arms/H_CSI.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv / ptrace.log | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c / 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, arms/B_CSI_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| walk_hand_off walk_04 | 2871487f1fcef51cfd6787187ecb3d418a8a8d78859113e37383a3b19b495726 |
| walk_hand_on, walk_matched_on walk_04 | 4d6dac65bf13ffc455fd893c88f4e18edbb6c1d4e871e1442ab880634067324d |
| walk_matched_off walk_04 | da29bdef4b0bc8f12ef75ae9396ccba6a7fc0a41d182c751f46c0c582779b5ec |
