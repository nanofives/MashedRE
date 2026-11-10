# RESULT PORT-COLOURSEL-BAR-Y (2026-11-09)

PREREG: `PREREG_COLOURSEL_BAR_Y.md` (committed alone, 4038e7626). Branch `orch/PORT-COLOURSEL-BAR-Y` from
`orch/PORT-COLOURSEL-ICON` 69e213e1d. Code commit ed3c6b8f8.

## 1. Answer

**GREEN.** The cause is a missing branch, not a wrong offset.

- In the cup flow (`0x00430760` != 0, game mode 2/10/3/4/5, test at 0x00433726..0x0043372d), the original's
  colour-select draw `0x004335f0` takes a **single-row** path. Its bar top is record y - 13.0 (0x00433817..0x0043381d,
  0x005cd8d8 = 13.0), and `0x004332a0` sets that record y to the constant 220.0 every frame (0x00433428). So the bar top
  is at 207 and the icon at y 219.
- The exe has only the multi-row model: `g_input_active_slots` rows, bottom-anchored with the last bar top at 261.25
  (a fit to a 4-row capture), and the number y from the multi-row base. On this machine that is one row at 261/262, about
  55 px low.
- Knob `MASHED_COLOURSEL_BAR_Y=1` adds the single-row path. With it ON, walk_04's bar border rows move from 262 / 287 to
  **207 / 233**; the original has 207..208 / 231..232.
- With `MASHED_COLOURSEL_ICON=1` as well, player 1's red keyboard icon sits in column 1 on that bar (98 red px in the
  original's icon box, 112 on the original).
- Only walk_04 changes among the 12 captures. The gates are unchanged with either knob OFF or ON.

## 2. Change (ed3c6b8f8)

- `Frontend/MenuNavSM.cpp` `ColourSelBarY_SingleRow()`: knob (env `MASHED_COLOURSEL_BAR_Y`, read once) and `Q_ModeIsCupFlow()`
  (the existing port of 0x00430760's mode set).
- `exe_main.cpp` colour-select rows: when it is true, `nrows4 = 1`, last (only) bar top `(220 - 13) * kVScale`, and the
  number y 220.0 (the record y, 0x00433ecc). The icon keeps its offset from the bar. When it is false, the block is
  unchanged.
- run_arms.py: stacks `CSB` (this knob) and `CSIB` (this knob + `MASHED_COLOURSEL_ICON=1`).

Build `8cd559db` (`MASHED_NO_DEPLOY=1`): rva-lint `NEW=0`, Build OK.

## 3. Gates (one run, private root `D:/MashedWT/PORT-COLOURSEL-BAR-Y_root`, `coloursel/run_csb.py`)

| arm | hashes |
|---|---|
| H_DEF / H_CSB / H_CSIB | `8284be46 / 67931834 / 31fe043e` all three |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 |
| B_CSB_K0 / K1 / K26 | `1d5bf4f9` x3 |
| B_CSIB_K0 | `1d5bf4f9` |

## 4. walk_04 (nav demo, hand path, 04D settings baseline)

| run | walk_04 sha256/16 | bar border rows at x 560 | red px, column-1 box at y 205..235 | the same at y 258..290 |
|---|---|---|---|---|
| original (WALK04) | `d40b38fdd6a30c50` | 207-208 ... 231-232 | 112 | 0 |
| OFF | `2871487f1fcef51c` | 262 ... 287 | 0 | 0 |
| BAR_Y ON | `4d53d93f87f0332d` | **207 ... 233** | 0 (grey icon, the icon knob is off) | 0 |
| ICON ON | `4d6dac65bf13ffc4` | 262 ... 287 | 0 | 97 |
| both ON | `546fa766d2e3a3bc` | **207 ... 233** | **98** | 0 |

In each knob run the other 11 captures are byte-identical to OFF. Visual crop (original over both ON):
`D:/MashedWT/PORT-COLOURSEL-BAR-Y_data/bar_cmp.png`. With both knobs on, the bar sits under the tiles, the red keyboard
icon is in column 1, and the `1` is beside it, as on the original.

Residuals seen in the crop, not in this step's scope:

- The original's bar has a darker border on all four sides. The exe draws only its top and bottom lines. That also
  accounts for the 233 against 232 bottom row (bar height 26.2 against the original's).
- The background video frame differs, as in every capture pair.

## 5. Parity harness

`re/frida/menu_draw_burst.py --screen 4` reaches screen 4 through a direct `FUN_0043d2a0(4, 0)` push. That skips
LAB_0043f468 (0x0043f468..0x0043f4db), so the game mode stays at its title value and 0x0067f0c0 is not set. 0x004335f0
then takes the multi-row path, not the cup-flow path this row is about. So a draw-list diff of this screen in its
real state needs an input-driven original capture, which the harness does not provide. No GREEN draw-list diff is
claimed. The evidence is the static law (section 1) plus the backbuffer measurements (section 4). Proposed harness item:
`menu_draw_burst.py --nav confirm,confirm,confirm` (drive nav_agent.js's input override, as `walk04/orig_walk04.py`
does, before the burst).

## 6. Flip

Both knobs are default OFF. A flip of `MASHED_COLOURSEL_BAR_Y` alone is safe for the layout. A flip of
`MASHED_COLOURSEL_ICON` waits for the left/right trace (PORT-COLOURSEL-ICON section 6). Static note for that trace:
0x004332a0 has two paths on `[0x007f1a0c] == 0x1000` (0x004332bc). The 0x1000 path moves the per-profile records
0x0067eaf0 + 12*k on left/right edges (0x004332e0..0x004333e9: dec / inc of `[eax-0xc]`, `[eax]`, `[eax+0xc]`, clamped
0..6). The cup-flow path starts at 0x00433443 and reads 0x0067f0c0 and the 0x007f1a14 list. Which of the two moves
0x0067f0c0 on left/right is still to be traced.

## 7. Commits

| commit | content |
|---|---|
| 4038e7626 | PREREG (alone) |
| ed3c6b8f8 | the knob, `CSB` / `CSIB` stacks |
| this commit | `coloursel/run_csb.py`, this RESULT |

## 8. Raw files (`D:/MashedWT/PORT-COLOURSEL-BAR-Y_data`, not committed)

| file | sha256 |
|---|---|
| exe/mashed_re.exe | 8cd559dba000f2475078f5a01d27ec0fed8db2df3e2820a958590a2d6751a758 |
| build.log / run_csb.log | d730f2f89d650bb2ea115e877c01bf840800a27e5b4781ca1b050a75f319abe9 / 61112127b7970cd468063d5370af79726d30358f6e3a629f4f8b7a1affb27c18 |
| arms/H_DEF.step.csv, H_CSB.step.csv, H_CSIB.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv / ptrace.log | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c / 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0, B_CSB_K0, B_CSIB_K0 step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| walk_off walk_04 | 2871487f1fcef51cfd6787187ecb3d418a8a8d78859113e37383a3b19b495726 |
| walk_bar walk_04 | 4d53d93f87f0332d3d757d584b0fbaa9bf5ea54a6883b70fd5da920157bba9b2 |
| walk_icon walk_04 | 4d6dac65bf13ffc455fd893c88f4e18edbb6c1d4e871e1442ab880634067324d |
| walk_both walk_04 | 546fa766d2e3a3bc5a11265d270d8e6d2276b70369ce4695e4a8de76157836e2 |
| bar_cmp.png | 2ee62e6207b4fafc213dfac8a89be2ca202c1af5ed16a158de01852d1c43e95a |
