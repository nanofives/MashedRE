# RESULT HB-MENUBURST-NAV (2026-11-09)

Branch `orch/HB-MENUBURST-NAV` from `orch/integration` 488fa1d9f. Tool change only, so no `mashedmod/` change and no
gates.

## 1. Answer

**GREEN.** `re/frida/menu_draw_burst.py --nav STEP,..` reaches a screen through the real menu inputs. The self-test
reached walk_04's cup-flow colour-select state on the original, and `drawlist_diff.py` ran against the exe's
`MASHED_DBG_DRAWSTREAM` dump with both colour-select knobs ON. The diff itself is RED (35 matched, 49 mismatched,
14 missing, 96 extra). That is the expected result for a screen the exe does not yet match. The harness now produces
row-level evidence for this state, which a `--screen 4` push could not.

## 2. Change (`re/frida/menu_draw_burst.py`)

- `--nav confirm|left|right|up|down|wait:S,...` runs after the title is up and before `--screen` / `--post-modal`. It
  reuses two in-process mechanisms the repo already has, and injects no OS input:
  - `confirm` uses the FUN_00497310 return override for player 0 and control code 4, the same as `re/frida/nav_agent.js`.
    The step is pressed until the menu depth (0x0067e9f8) rises, at most 6 tries.
  - The directions write player 0's cooked edge flag 0x007f1044 + col (0 left, 1 right, 2 up, 3 down) at FUN_004967e0
    onLeave for 3 input frames, the same as `re/frida/input_feed_cooked.py`.
- `--peek 0x..,..` reads dwords after the nav and after the burst. They are printed and written to `<out>.peek.json`.
- The default label is `nav` when `--nav` is given. Nothing changes without `--nav`.

## 3. Self-test

Original: `menu_draw_burst.py --nav confirm,confirm,confirm --frames 4 --label cs --peek 0x0067f0c0,0x0067e9fc,0x0067f0c8,0x007f1a0c,0x0067e9f8`,
root `D:/MashedWT/PORT-TOP-04D-WALK04_root`, under test-slot and `mashed_lock.py run`.

| check | value |
|---|---|
| depths | 1 -> 2 -> 3 -> 4 (three confirms) |
| 0x0067f0c0 (choice, set at 0x0043f4d1) | 1 |
| 0x0067e9fc (game mode) | 3 (in 0x00430760's set, the cup flow) |
| 0x0067f0c8 (record y) | 0x435c0000 = 220.0 |
| 0x007f1a0c (0x004332bc path selector) | 1 (not 0x1000, so the single-row path) |
| draws per frame | 98, the same in all 4 frames |

A `--screen 4` push leaves the choice and the mode at their title values, so this state was out of the harness's reach
before.

Exe: build `8cd559db` (`orch/PORT-COLOURSEL-BAR-Y`, both knobs present), nav demo `MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`
with `MASHED_COLOURSEL_BAR_Y=1 MASHED_COLOURSEL_ICON=1`, `MASHED_DBG_DRAWSTREAM=250:300`. The colour select is frames 268..295
(between the Enters at det_frame 269 and 297, 180 draws per frame). Frame f282 was used.

`drawlist_diff.py orig_cs.json exe_cs_stream.json --label-a cs_f0 --label-b f282 --scale-b 1`. The exe ran at 640x480
here, so it uses scale 1, not the 800x600 default 0.8. Summary: matched 35, mismatched 49 (44 moved, 5 reordered), missing
14, extra 96. Full listing: `verify/portstate_20261109/HB_MENUBURST_NAV_diff_cs.txt`.

The rows this lane needs:

| element | original | exe |
|---|---|---|
| bar fill | x 48, y 207, 534 x 26, `7ff06e14` (0x0043387c) | x 46.2, y 207, 538.1 x 26.2, same colour |
| top border | x 46, y 207, 538 x 2, `ffb45010` (0x004338ec) | x 46.2, y 207, 538.1 x **1.0** |
| bottom border | x 46, y 231, 538 x 2 (0x00433926) | x 46.2, y **232.2**, 538.1 x **1.0** |
| left / right border | x 46 / 582, y 207, 2 x 26 (0x004338b5 / 0x00433971) | **missing** |
| colour tiles | 6 x (69 x 20 swatch at y 182 + 64 x 64 icon at y 139), 0x004337c0 / 0x004337eb | **missing** at these rects |

These give PORT-COLOURSEL-LEFTRIGHT its border targets. The tile rows are a separate composition difference, outside
both rows.

## 4. Commits

| commit | content |
|---|---|
| this commit | `re/frida/menu_draw_burst.py` (`--nav`, `--peek`), `HB_MENUBURST_NAV_diff_cs.txt`, this RESULT |

## 5. Raw files (`D:/MashedWT/HB-MENUBURST-NAV_data`, not committed)

| file | sha256 |
|---|---|
| orig_cs.json | e9e0da22686eb7cd5a32530d0f361f16a50518b3739c44b9d2204029ef4b0012 |
| orig_cs.peek.json | 2aaa8b179f821eca7db78e302f93395d0fa4ae44880737cd0d473b9072b94011 |
| orig_cs.log | b764c943bf6208744bcd10803653cc8cfee08a811d3c70bb6a6bd5954397ca55 |
| exe_cs_stream.json | 837b77ec04aa89eef3abaed97565e051d481692c2c1e4097d7108a981d27a1b4 |
| exe_cs.log | 8f07b6849bc745bffbb69267f43a82628579661b76a79249a2c04dcf06fba2a7 |
| diff_cs.json | bc8c8af1a040e2d0694ac0c4f3ca156278fba175b8480ed82d14b8097b73fe15 |
| diff_cs.txt | f5acedac6ebaf3543f3516f057643ad0d36b36649b933581b9a764b1b095b5ef |
