# RESULT D3-CF-R7INPUT: ctrl byte 0xa now has a port writer (`MASHED_R7_INPUT`, default OFF). With it and `MASHED_R7_EXIT`, a key press leaves sub-state 7 on the press frame, defaults unchanged (2026-10-25)

Branch `orch/D3-CF-R7INPUT` from `orch/integration` @ `2e80adace`. **K357 merged in**: `orch/D3-CF-K357` @ `a30d080b5` via
`d41cdb478` (`git -c merge.autoStash=false`, merge base `b5c84688e`, K357 was not an ancestor of `2e80adace`). The merge keeps the
U9779 flip of integration and adds `MASHED_R7_EXIT` (default OFF). Port `2893ffa13`, PREREG `36edf0b72` (committed alone, before
any arm). Exe `d9a1c7c5` (`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not redeployed). Private roots
`D:\MashedWT\D3-CF-R7INPUT_root` (arms, D2) and `..._root13` (13 tracks). Every MASHED run under `mashed_lock.py` (D2 with
`--solo`), after the lead's go. This was the only worker running MASHED in that window. Decision L-10 = (1): no default changed.

## 1. The writer (original, see PREREG section 1 for every site)

`FUN_00496530` (per slot i, from `FUN_004967e0`) stores `FUN_00497310(i, 4)` at `0x007f1042 + i*0x4c` (`0x004965ac..0x004965b7`).
For a keyboard slot (type 2, `0x004973fd`) that is `0xff` when bit `[cfg + 0x118]` of the keyboard bitmap `0x0077313c`
(`FUN_004972b0` / `FUN_00496100`, from `GetDeviceState`) is set, else 0. The default binding for action 4 is `0x1c`
(`0x00498698`). `original\` has no `contcfg%d.bin`, so the original here uses that default too.

## 2. The port (`exe_main.cpp`, `CookCtrlByteAFromKeyboard`, default OFF)

`MASHED_R7_INPUT=1`: each frame, for every slot with `g_input_slot_type[i] == 2`, store `(g_keys[0x1c] & 0x80) ? 0xff : 0` at
`0x007f1042 + i*0x4c`. Byte 0xa only. Not ported: the joypad branch, the block zeroing (`0x00496568`), the shadow copy
(`0x00496552`), the contcfg override. Knob OFF writes nothing. `MASHED_R7_INPUT_PRESS=<n>` is an arm-test press on the n-th
sub-state-7 frame. New log lines: `R7IN ...` per nonzero store, `R7X EXIT det_frame=.. round a -> b match_winner=..` under
`MASHED_R7_EXIT`. On this machine the keyboard is slot 1 (`game controllers 1, types 1 2 0 0`), so the store lands in block 1.

## 3. Results (`verify/r7input_20261025/run_arms.py`, `data/run_gates.txt`, `data/arms/RUNLOG.txt`)

| id | arm | measured | prediction |
|---|---|---|---|
| P0 | `H_DEF` | `ef5f2e10 / 57f7ff83 / b4f4b55a` (15941 / 53273 / 26636 lines) | **holds** |
| P0 | `B_DEF_K0 / K1 / K26` | `c37d4c0c / 0a8f9806 / 7013671d` | **holds** |
| P0b | `B_I_K0` | `c37d4c0c`, no `R7IN` line | **holds** |
| P0c | `B_X_K0`, `B_XI_K0` | `3a03670c`, `3a03670c` (= K357's `B_U7PX_K0`) | **holds** |
| P1 | `B_XIP150_K0` | round 1 ends (scores 4,5,7,8). `R7IN slot=1 val=0xff r7frame=150 test=1 det_frame=551` and `R7X EXIT det_frame=551 round 1 -> 2 match_winner=-1` on the same frame. Round 2 ends (2,7,8,7), the test press fires again at its r7frame 150 (`det_frame=1058`) and exits on that frame too, `round 2 -> 3`. Step CSV `dbe10a3e` (1492 lines), differs from `B_X_K0` | **exit-on-press holds twice. The PREREG's "exactly one `R7IN` line" does not**: the test injector re-arms on every sub-state-7 episode, and this arm reached a second one. The count, not the mechanism, was mispredicted |
| P2 | `B_XIP50_K0` | `R7IN ... r7frame=50 test=1 det_frame=451`, no `R7X EXIT`, step CSV `3a03670c` = `B_X_K0` | **holds** (counter 100 < 200 on the press frame, `0x004109b9`) |
| P3 | every gate / B / H arm | no `test=0` line | **holds** |
| report | `H_XI` | `5943c5f1 / b493262b / 09551e6a` (= K357's `H_U7PX`) | equal to reference |
| report | `B_XI_K1 / K26` | `2516d903 / 53778664` (= K357's `B_U7PX_K1 / K26`) | equal to reference |
| P4 | `D2_XI_1 / 2 / 3`, `--solo`, `score_d2_v2.py port` (`data/score_d2_XI.txt`) | **PASS 3/3**: slip1500 0.1900 x3, slip2000 0.2549 / 0.2548 / 0.2550, drvmed 1970.58 / 1970.58 / 1966.66, H1 398/400 x3. motion_diag 1636 / 1637 / 1634 lines, sha8 `b7b66701 / 0ecd557e / 7f0e420a`. The first two equal U9779-M's `D2_DEF_1 / 2` byte for byte. The third is a new wall-clock variant inside the known range (1965.67-1970.58) | **holds** |
| P5 | 13 tracks, `run13.py run ... MASHED_R7_EXIT=1 MASHED_R7_INPUT=1`, scored vs `cites_rres.json` (`data/tracks_XI_score.txt`, `data/t13.out`) | 39 frames, 878 rows, 0 RED, INSTRUMENT GATES PASS, 13/13 OK after 1 attempt. Score table **identical** to `verify/u9779m_20261025/data/tracks_M_score.txt` (41 / 41 non-blank lines) | **holds** |

P3 note for D2: each D2 log has one `R7IN ... r7frame=0 test=0` line at det_frame 18 / 12 / 19. That is not a keystroke. It is the
D2 recipe's own scripted Enter (`RunRaceDemoStep`, `NavDemoTap(DIK_RETURN)`, `exe_main.cpp:1694`, which launches the race from the
menu), stored before any race. The arms were not re-run: a re-run reproduces the same tap, and two of the three motion_diag files
are byte-equal to the knob-off default. No piz-collision crash (U-9677), no TIMEOUT in any run.

## 4. Scope and owed

- [UNCERTAIN] joypad: with a controller on slot 0, the original's player 1 would also exit with button 0 (type-1 default binding 0,
  `0x00498640`). The standalone polls no joystick, so that is not ported.
- [UNCERTAIN] slot table: the port seeds `0x007f1a14 + v*0x10 = v`. The gate sees the keyboard block through v = 1 (an AI car) on
  this machine. In the original the human car's entry is the profile that confirmed in the frontend. On a machine with no game
  controller the keyboard is slot 0 = the player car, and the store reaches the gate through v = 0.
- Trackers (through re-classify, not done here): FUN_00496530's byte-0xa store (`0x004965ac..0x004965b7`) is ported default OFF
  behind `MASHED_R7_INPUT`. This resolves K357's owed item "the port never writes byte 0xa of a ctrl block".
- Evidence: `data/arms/` (B step CSVs, per-arm logs, `RUNLOG.txt` with the H / D2 line counts and hashes), `run_gates.txt`,
  `run_d2.txt`, `score_d2_XI.txt`, `t13.out`, `tracks_XI_score.txt`. H step / gates / ptrace, D2 motion_diag and the `t13/` dir
  are on `D:\MashedWT\D3-CF-R7INPUT_data` (hashes in `RUNLOG.txt`).

## 5. Decision for Mariano (filed as a ledger decision row)

Flip `MASHED_R7_EXIT` and `MASHED_R7_INPUT` default ON together? With both OFF the gates are unchanged. With both ON and no
input, the B / G-CTRL hashes are K357's U7PX values: B `3a03670c / 2516d903 / 53778664`, G-CTRL
`5943c5f1 / b493262b / 09551e6a`. D2 is PASS 3/3, two runs byte-equal to the default, and the 13 tracks are identical. A key
press on the bound key (Enter) leaves sub-state 7 on the press frame once the 100-tick counter is met. That closes K357's stall
cost for keyboard play.

Options:
1. Flip both, with refs moved to B `3a03670c / 2516d903 / 53778664`, G-CTRL `5943c5f1 / b493262b / 09551e6a`.
2. Flip `MASHED_R7_INPUT` only. Without the gate nothing reads the byte (`B_I_K0` = `c37d4c0c` measured; H and B K1 / K26 with
   `I` alone not measured).
3. Keep both OFF.

Recommendation: (1). The joypad path stays unported, so a controller-only player would still stall in sub-state 7.
