# RESULT PORT-TOP-04D-WALK04 (2026-11-09)

Branch `orch/PORT-TOP-04D-WALK04` from `orch/integration` 4abc2ce60. Question from PORT-TOP-04D (`RESULT_PORT-TOP-04D.md`,
"0x004924f0: compiled, not run by default"): when 0x004924f0 runs matched, walk_04_colourselect puts player 1's keyboard
icon on the right, highlighted. With the stand-in it is on the left, in white. Which state does the original show?

## 1. Answer

**Neither exe state is the original's. The matched state is not right.** On the original, player 1's keyboard icon is on
the **left**, in the first colour column (under the red devil, x about 148 to 195), tinted **red**, with the `1` beside
it. The bar sits right under the colour row, about 55 px higher than in both exe captures (border rows y 208..232 on the original, about 262..286 in both exe captures, read at x 560).

| capture | file (sha256/16) | P1 icon column | icon tint | bar border top (x 560) |
|---|---|---|---|---|
| **original** (this step) | `d40b38fdd6a30c50` | column 1 (red), left | red | y 208 |
| exe stand-in, hand (04D) | `2871487f1fcef51c` | left of column 1 | white | y about 262 |
| exe matched 0x004924f0 (04D batch 1) | `da29bdef4b0bc8f1` | column 4 (yellow), right | yellow | y about 262 |

Side by side (half size, original / hand / matched): `D:/MashedWT/PORT-TOP-04D-WALK04_data/walk04_full3.png`. The bar rows at full
size: `D:/MashedWT/PORT-TOP-04D-WALK04_data/walk04_bars.png` (PNGs are gitignored, SHA-256 in section 6). Pixel counts: original vs either exe capture 303,466 px differ over the full frame (the
background video frame also differs), so only the layout is read here. Hand vs matched is the 2188 px of 04D.

So the question "left or right" has the answer left, but the hand state is not the original's state either: its icon
is outside the colour columns and untinted, and both exe captures place the bar lower.

## 2. Why: the original's 0x007f0a40 comes from the save file, not from 0x004924f0

`walk04/orig_walk04.py` snapshotted the dwords 04D names at each screen. On the original they hold the same values at
title, main menu, single player and colour select:

- 0x007f0a40..0x007f0a7c: `0 1 0 2 2 0 0 0 0 0 0 1 0 0 0 2`. These are **not** 0x004924f0's defaults (`ctl[13][5] = -1`
  and the rest, 04D), and **not** zeros (the hand path).
- 0x007f0f10 = 2 (the 0x00431b10 value, as the matched body writes). 0x007f0f2c..0x007f0f3c = 0.

These 64 bytes equal `original/gamesave.bin` at file offset **150080** byte for byte (the root's copy, sha256/16
`bd18788182b2343e`). So on the original the save load overwrites what 0x004924f0 wrote into the 0x007f0a40 table.
The exe does neither: `exe_main.cpp:189-190` keeps that span in the save-span model and does not mirror it back to
0x007f0a40. The hand path leaves zeros there, and the matched path leaves 0x004924f0's defaults.
[UNCERTAIN] Which writer copies the save bytes into 0x007f0a40 on the original, and when. A `--watch-write` run on
0x007f0a40 during boot names it (memory note "find record writers with --watch-write"). This step did not run it, and
it did not map which table field drives the icon's column or tint.

The bar's height difference is present in both exe captures, so it is independent of 0x004924f0. It is not
investigated here.

## 3. What running 0x004924f0 matched would need

The matched state is not right, so the brief's conditional does not apply. For the lead, the fact that follows from
section 2: running 0x004924f0 matched is safe for walk_04 only if the save restore then writes the 0x007f0a40 span
into the image as the original does. With that write, the original's table is the save bytes regardless of
0x004924f0's defaults, and the matched body becomes value-neutral on this screen, as it is on the original. Proposed
order: (1) a watch-write capture on the original for 0x007f0a40, (2) a port step that mirrors the save span to
0x007f0a40 behind a default-OFF knob, with walk_04 compared to `d40b38fdd6a30c50` in layout, (3) only then run
0x004924f0 matched by default. WinMain's default path is unchanged by this step.

## 4. Method

- Private root `D:/MashedWT/PORT-TOP-04D-WALK04_root` (robocopy of `original\`, stock `gamesave.bin`, d3d9 shim with
  `MASHED_ORIG_BBDUMP_REQ`). One boot of the original, hooks OFF (`MASHED_RE_NO_AUTO_HOOK=1`), under test-slot and
  `mashed_lock.py run --label PORT-TOP-04D-WALK04`. Spawned pid only, killed by pid.
- Input: `re/frida/nav_agent.js` confirm (control 4, player 0) three times, gated on the menu depth (title 1, main menu
  2, single player 3, colour select 4), the same three Enters as the exe walk (`exe_main.cpp:1547-1556`, cursor 0 at
  each screen). Dumps through the shim's on-demand channel after 2 to 3 s of settle.
- Difference from the exe walk: the exe walk restores `mashed_re_settings.bin` from the TOP-03 baseline and injects
  keyboard edges (DIK_RETURN). The original run uses the stock `gamesave.bin` and the device-independent control
  override. The original still shows a keyboard icon for player 1.

## 5. Commits

| commit | content |
|---|---|
| this commit | `walk04/orig_walk04.py`, this RESULT |

No `mashedmod/` change, so no gates were run.

## 6. Raw files (`D:/MashedWT/PORT-TOP-04D-WALK04_data`, not committed)

| file | sha256 |
|---|---|
| orig1/walk_01_title.bmp | a5197270e2469ad0fe7407efd229a850beaaab8c4bc35c69fc4d50f50922b744 |
| orig1/walk_02_mainmenu.bmp | 0f41ed610755796b51a31e49c2902801c2ad59aa8f8cfb7067cbb85e9d9a1724 |
| orig1/walk_03_singleplayer.bmp | 38490b0708e5f106328d3834ba7f82bc69424526888e9909df8887cb150f4ec3 |
| orig1/walk_04_colourselect.bmp | d40b38fdd6a30c50fe0761ca26c35d0d8b2760be5ccd821a9a2fe038ceb85fa0 |
| orig1/walk.json | 951963db46df346dfe77011cc7f54ff2784597c55cce4559fd239ee3dbbf7f10 |
| orig1.log | d76d04dd918096fbf2eae06d1fe6297db517244c8039648fb38f85fd9a72cd65 |
| walk04_full3.png | 27ac95efc016f54988b7b457f48bb22bf9a9d9ddca8d32e62c0a5f43c943b0aa |
| walk04_bars.png | ff2e2585ae2bb2f32329687dc4e37629a65745920aebda38ba631b915ed6531a |
| exe hand, `D:/MashedWT/PORT-TOP-04D_data/navdemo_hand1/walk_04_colourselect.bmp` | 2871487f1fcef51c (04D) |
| exe matched, `D:/MashedWT/PORT-TOP-04D_data/batch1/navdemo_knob1/walk_04_colourselect.bmp` | da29bdef4b0bc8f1 (04D) |
