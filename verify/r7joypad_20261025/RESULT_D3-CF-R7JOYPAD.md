# RESULT D3-CF-R7JOYPAD: the joypad branch of ctrl byte 0xa is ported under `MASHED_R7_INPUT` (default OFF). Defaults equal the refs. With both R7 knobs ON, a pad press (button 0) leaves sub-state 7 on the press frame, same as Enter (2026-10-25)

Verdict: **DECISION**. The port is GREEN with the knobs OFF (G-CTRL and B equal the `merge_train.json` refs exactly). The
flip of `MASHED_R7_EXIT` + `MASHED_R7_INPUT` is a decision row (section 5).

Branch `orch/D3-CF-R7JOYPAD` from `orch/integration` @ `aed0e1bc7`. `orch/D3-CF-R7INPUT` @ `5a466becb` is not an ancestor of
integration (`git merge-base --is-ancestor` = 1). Its K357 part `a30d080b5` is (merged by `62e456875`). Its port commit
`2893ffa13` was not, so it is cherry-picked here as `1deedd619` (exe_main.cpp + its arm runner only, no evidence files).
Joypad port `5006f7baa`, runner `668cea7f3`, PREREG `7fe80153a` (committed alone, before any arm). Exe sha256
`81783c3976aee38e522a4bb8eb0129ef0155098a68feb854ff9d6710bc728e0a` (`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not
redeployed). Private roots `D:\MashedWT\D3-CF-R7JOYPAD_root` (arms, D2) and `..._root13` (13 tracks). Every MASHED run under
`mashed_lock.py` (D2 with `--solo`). The gate run queued behind D4-DC-39b0-L13, the 13-track run behind its solo window.

## 1. The original's joypad read (PREREG section 1 has every site)

Keyboard Enter is binding 0x1c at `0x00498698`. Next to it, for a slot i below the controller count, `FUN_00498510` sets type 1
(`0x0049861a`), device index i (`0x0049861d`) and action-4 binding **0** (`0x00498640` `mov dword ptr [esi+0x10], 0x0`) = button 0.
`FUN_00497310`'s type-1 branch (`0x00497323`) reads bit `code & 7` of byte `[0x007730d4 + device*4 + (code >> 3)]`
(`0x004973da..0x004973f6`). That dword is filled each frame by `FUN_004972b0` -> `FUN_004957a0` from record + 0x408
(`0x00495819`), which `FUN_00495870` rebuilds after Poll / Acquire / GetDeviceState(0x50) from `DIJOYSTATE.rgbButtons[0..15]`
(`0x00495886..0x00495afe`). Order per frame: `FUN_004967e0` = poll `FUN_00495fe0`, snapshot `FUN_004972b0`, then the cook.

## 2. The port (`exe_main.cpp`, default OFF)

`MASHED_R7_INPUT=1` keeps each enumerated game controller (up to 4) in `g_joy[]` with `c_dfDIJoystick`, polls it each cook
(`PollJoypadButtons`: Poll, Acquire on failure, GetDeviceState; a failed read keeps last frame's bits, as `0x004958bc` does),
and stores `0xff` / `0` at `0x007f1042 + i*0x4c` for a type-1 slot from button 0 of device i. Deviation, stated in the PREREG:
cooperative level 6 (BACKGROUND | NONEXCLUSIVE) where the original passes 5, so a concurrent `MASHED.exe` keeps its pad. Knob
OFF: the device is released as before, nothing is polled or written. `MASHED_R7_INPUT_PAD_PRESS=<n>` (arm test) ORs bit 0 into
joypad 0's bits on the n-th sub-state-7 frame. The `R7IN` line gains ` type=<1|2>`.

## 3. Results (`data/run_gates.txt`, `data/run_d2.txt`, `data/t13.out`, `data/r7_log_lines.txt`)

Machine state for every arm: `input slots: game controllers 2, types 1 1 2 0, active 3` (the PREREG expected 1 controller,
`types 1 2 0 0`). Slots 0 and 1 are pads, the keyboard is slot 2. The pad rows are scored (the PREREG skips them only for 0
controllers).

| id | arm | measured | prediction |
|---|---|---|---|
| P0 | `H_DEF` | `ef5f2e10 / 57f7ff83 / b4f4b55a` (15941 / 53273 / 26636 lines) | **holds** |
| P0 | `B_DEF_K0 / K1 / K26` | `c37d4c0c / 0a8f9806 / 7013671d` | **holds** |
| P0b | `B_I_K0` | `c37d4c0c`, no `R7IN` line | **holds** (the pad branch now stores 0 into blocks 0 and 1 every frame, with no hash change) |
| P0c | `B_X_K0`, `B_XI_K0` | `3a03670c`, `3a03670c` | **holds** |
| P1 | `B_XIP150_K0` (keyboard) | `R7IN byte0xa slot=2 ... r7frame=150 test=1 det_frame=551 type=2`, `R7X EXIT det_frame=551 round 1 -> 2`, again at `det_frame=1058` `round 2 -> 3`. Step `dbe10a3e` | **mechanism holds, slot index mispredicted**: the PREREG wrote `slot=1` from R7INPUT's 1-controller machine. With 2 controllers the keyboard is slot 2 |
| P2 | `B_XIP50_K0` (keyboard) | `R7IN ... r7frame=50 test=1 det_frame=451 type=2`, no `R7X EXIT`, step `3a03670c` = `B_X_K0` | **holds** |
| P1p | `B_XIPP150_K0` (pad) | `R7IN byte0xa slot=0 val=0xff r7frame=150 test=1 det_frame=551 type=1`, `R7X EXIT det_frame=551 round 1 -> 2 match_winner=-1`, again at `det_frame=1058` `round 2 -> 3`. Step `dbe10a3e` = `B_XIP150_K0` | **holds** (exit on the press frame, step CSV equal) |
| P2p | `B_XIPP50_K0` (pad) | `R7IN ... slot=0 ... r7frame=50 test=1 det_frame=451 type=1`, no `R7X EXIT`, step `3a03670c` = `B_X_K0` | **holds** |
| P3 | every arm | no `test=0` line in any B or H log. Each D2 log has one `R7IN byte0xa slot=2 ... r7frame=0 test=0` at det_frame 72 / 69 / 70 | **holds**: that is D2's scripted Enter tap on the keyboard slot before the race (the PREREG's exception). [UNCERTAIN] R7INPUT saw it at det_frame 18 / 12 / 19; the shift is not explained here, and two of three motion_diag files are byte-equal to R7INPUT's |
| report | `H_XI` | `5943c5f1 / b493262b / 09551e6a` (14659 / 56873 / 28436 lines) | equal to R7INPUT and K357's `H_U7PX` |
| report | `B_XI_K1 / K26` | `2516d903 / 53778664` | equal to R7INPUT |
| P4 | `D2_XI_1 / 2 / 3`, `--solo`, `score_d2_v2.py port` (`data/score_d2_XI.txt`) | **PASS 3/3**: slip1500 0.1900 x3, slip2000 0.2549 / 0.2548 / 0.2548, drvmed 1970.58 x3, H1 398/400 x3. motion_diag 1636 / 1637 / 1637 lines, sha8 `b7b66701 / 0ecd557e / 0ecd557e`. Run 1 and 2 equal R7INPUT's `D2_XI_1 / 2` byte for byte, run 3 equals run 2 | **holds** |
| P5 | 13 tracks, `run13.py run ... MASHED_R7_EXIT=1 MASHED_R7_INPUT=1`, scored vs `cites_rres.json` (`data/tracks_XI_score.txt`) | 13/13 OK after 1 attempt, 0 RED, INSTRUMENT GATES PASS. Score table **identical** to `orch/D3-CF-R7INPUT:verify/r7input_20261025/data/tracks_XI_score.txt` (41 / 41 non-blank lines) | **holds** |

No piz-collision crash (U-9677), no re-run, no TIMEOUT.

## 4. Scope and owed

- [UNCERTAIN] slot table: the port seeds `0x007f1a14 + v*0x10 = v`. The pad press reaches the gate through v = 0 because
  slot 0 is a pad on this machine. In the original the human car's entry is the profile that confirmed in the frontend.
- Not measured: a physical button press. `MASHED_R7_INPUT_PAD_PRESS` feeds the same bits after the poll. The poll runs on 2
  attached pads in every I / XI arm. Not ported: the axes (`0x0077311c`), the block zeroing (`0x00496568`), the shadow copy
  (`0x00496552`), the contcfg override (`FUN_004971b0`).
- Trackers (through re-classify, not done here): FUN_00496530's byte-0xa store is now ported for type-1 and type-2 slots,
  default OFF. FUN_00495870 (buttons only), FUN_004972b0 (button snapshot) and FUN_00497310's type-1 default branch have a
  port form behind the knob. No C-level change claimed.
- Evidence committed: `data/run_gates.txt`, `run_d2.txt`, `score_d2_XI.txt`, `t13.out`, `tracks_XI_score.txt`,
  `r7_log_lines.txt` (every `input slots` / `R7IN` / `R7X` / round line of every arm log), `RUNLOG.txt`, `raw_sha256.txt`.
  Raw data stays on D:. `data/raw_sha256.txt` lists the SHA-256 of all 38 arm files in `D:\MashedWT\D3-CF-R7JOYPAD_data\arms`
  (B step CSVs, H step / gates / ptrace, D2 motion_diag, every arm log) and 205 files in `D:\MashedWT\D3-CF-R7JOYPAD_data\t13`
  (the 104 `.bmp` frame shots are not hashed). Cited raw files:

| file | SHA-256 |
|---|---|
| `D:/MashedWT/D3-CF-R7JOYPAD_data/arms/H_DEF.step.csv` | `ef5f2e105f649cdbf1a900340eb128ac8317fce986d36538248e3b686d1178ae` |
| `D:/MashedWT/D3-CF-R7JOYPAD_data/arms/H_DEF.gates.csv` | `57f7ff835cd9b7d774b939f08e091cf4b5faeca58cdcbd7fad0b41563fe134c6` |
| `D:/MashedWT/D3-CF-R7JOYPAD_data/arms/H_DEF.ptrace.log` | `b4f4b55a20e57e3a37b1643703bfa8e06d6973561e943bdaf369112aaefeb28f` |
| `D:/MashedWT/D3-CF-R7JOYPAD_data/arms/B_XIPP150_K0.step.csv` | `dbe10a3e8da1323d9891f0980d1bc9a0ee1cfb23c91287e6393b0e66e4be26b6` |
| `D:/MashedWT/D3-CF-R7JOYPAD_data/arms/H_XI.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `D:/MashedWT/D3-CF-R7JOYPAD_data/arms/B_DEF_K0.step.csv` | `c37d4c0cfbdd5843f946feec67c0d3559ac17613d2520cf86ac53f6f79935f84` |
| `D:/MashedWT/D3-CF-R7JOYPAD_data/arms/B_DEF_K1.step.csv` | `0a8f9806be171e483c1626fc0f985716a6412dd803e33587f408024e3870f1ec` |
| `D:/MashedWT/D3-CF-R7JOYPAD_data/arms/B_DEF_K26.step.csv` | `7013671dfea95c72b49f484ddae75cefeed87cb0f95d34495a3ba59aec6e6dc0` |
| every other cited file (`B_*`, `H_XI.gates.csv` / `.ptrace.log`, `D2_XI_*.motion_diag.log`, `t13/*`) | `data/raw_sha256.txt` |

## 5. Decision for Mariano (filed by the lead as one ledger decision row)

Flip `MASHED_R7_EXIT` and `MASHED_R7_INPUT` default ON together? With both OFF the gates equal the refs. With both ON and no
input: B `3a03670c / 2516d903 / 53778664`, G-CTRL `5943c5f1 / b493262b / 09551e6a` (the same values R7INPUT measured before
the joypad branch, so the pad branch adds no change with no press). D2 PASS 3/3, two runs byte-equal to R7INPUT's, 13 tracks
0 RED and identical. A press on Enter (keyboard slot) or on button 0 (pad slot 0) leaves sub-state 7 on the press frame once
the 100-tick counter is met, and the two step CSVs are equal.

Options:
1. Flip both. New refs: G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / 2516d903 / 53778664`.
2. Flip `MASHED_R7_INPUT` only (`B_I_K0` = `c37d4c0c`, nothing reads the byte without the exit gate; H and B K1 / K26 with I
   alone not measured).
3. Keep both OFF.

Recommendation: (1), merged alone under the decided-flip protocol. It closes K357's stall cost for keyboard and pad play.
With the flip, the default build opens and polls every attached game controller (non-exclusive).
