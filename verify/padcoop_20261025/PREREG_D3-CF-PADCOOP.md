# PREREG D3-CF-PADCOOP: the joypad cooperative level

Step: D3-CF-PADCOOP (decision L-16 follow-up, `log/orch/DECISIONS.md`). Branch `orch/D3-CF-PADCOOP` from
`orch/integration` @ `4ff9c8468` (MASHED_R7_EXIT + MASHED_R7_INPUT default ON). Committed alone before any arm.

## 1. The original (read from `original/MASHED.exe.unpatched`, capstone)

- `FUN_00495ee0` (EnumDevices callback, registered at `0x0049605c` by `FUN_00496040`, `EnumDevices(4, cb, 0x00772fac, 1)`
  at `0x00496055..0x00496069`): after CreateDevice (`0x00495f10`) and SetDataFormat(`0x005d076c`) (`0x00495f86`), it
  calls SetCooperativeLevel through vtbl `+0x34` at `0x00495fb3` with `push 5` at `0x00495faf` and hwnd = `[0x00772fb0]`
  (`0x00495fa5`). `[0x00772fb0]` is written by `FUN_004960a0` at `0x004960a9` from its first argument, and zeroed by
  `FUN_00496010` at `0x00496038`. No other code reads `0x00772fb0` (3 references in `.text`: `0x00495fa6`, `0x00496039`,
  `0x004960aa`).
- dinput.h: `DISCL_EXCLUSIVE 1`, `DISCL_NONEXCLUSIVE 2`, `DISCL_FOREGROUND 4`, `DISCL_BACKGROUND 8`. So 5 =
  EXCLUSIVE | FOREGROUND.
- Correction to the R7JOYPAD text: its PREREG and RESULT call the port's value "6 (BACKGROUND | NONEXCLUSIVE)" and the
  original's "5 (EXCLUSIVE | BACKGROUND)". Both labels are wrong. The port passes `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE`
  = 0xa, the original passes 5 = EXCLUSIVE | FOREGROUND.
- What FOREGROUND changes: DirectInput refuses Acquire on a FOREGROUND device while its window is not the foreground
  window, and GetDeviceState then fails. `FUN_00495870` returns at `0x004958bc` on a failed GetDeviceState with the button
  bitmap untouched. So the original reads the pad only while focused (with `no_focus_pause` it keeps running unfocused and
  keeps the last bits). The port reads it with the window unfocused.

## 2. The change (`exe_main.cpp`, default OFF)

`MASHED_PAD_COOP_ORIG=1` passes `DISCL_EXCLUSIVE | DISCL_FOREGROUND` (5) in `CountGameCtrlCb`. Default: 0xa as before.
Both paths log `pad coop: device <j> level 0x<l> hr 0x<hr>` once per kept device, and `PollJoypadButtons` logs
`pad state: device <d> GetDeviceState hr 0x<hr>` each time that result changes (first frame included). The comment above
`g_joy` now carries the correct flag names. No other behaviour change.

## 3. Arms (`verify/padcoop_20261025/run_arms.py`, root `D:\MashedWT\D3-CF-PADCOOP_root`, every run under `mashed_lock.py`)

Stacks: `PC` = the knob, `PP150` = `MASHED_R7_INPUT_PAD_PRESS=150`, `PCPP150` = both.

| id | arm | prediction |
|---|---|---|
| P0 | `H_DEF` | G-CTRL `5943c5f1 / b493262b / 09551e6a` (the `merge_train.json` ref). A miss is a DECISION, not a fix |
| P0 | `B_DEF_K0 / K1 / K26` | B `3a03670c / 2516d903 / 53778664` |
| P1 | every DEF arm log | one `pad coop: device j level 0xa hr 0x00000000` per attached game controller (this machine enumerated 2 in R7JOYPAD) |
| P2 | `B_PC_K0 / K1 / K26` | log `level 0x5 hr 0x00000000` per device. Step hashes equal P0's B values (no hand on a pad, so the bits are 0 whether the read succeeds or not) |
| P3 | `B_PC_K0` log | report only, not predicted: the `pad state` hr sequence. Expected a failing hr (`DIERR_NOTACQUIRED` 0x8007000c) on any frame the window is not foreground, S_OK while it is. Whether the window is foreground depends on the desktop at run time |
| P4 | `B_PP150_K0`, `B_PCPP150_K0` | sub-state 7 exits on the press frame (`R7X EXIT det_frame=551 round 1 -> 2`, as R7JOYPAD's `B_XIPP150_K0`), and the two step CSVs are equal (the synthetic press is ORed after the poll, so the read result does not matter) |
| P5 | `D2_DEF_1 / 2 / 3` under `run --solo`, `verify/d2rebase_20261019/score_d2_v2.py port` | PASS 3/3. motion_diag at least 1600 lines, else re-run (up to 2) |
| P6 | 13 tracks, `verify/rsky_20261016/run13.py run` (default, no knob), root `D:\MashedWT\D3-CF-PADCOOP_root13`, scored against `verify/rres_20261016/cites_rres.json` | 0 RED, INSTRUMENT GATES PASS, table equal to `verify/r7joypad_20261025/data/tracks_XI_score.txt` |

Piz-collision crashes (`+0x1509b4`, `+0xe66db`) are re-runs. Kill only PIDs this step spawned.

## 4. Verdict rule

P0 holds and P1..P2, P4..P6 hold: GREEN, the knob stays OFF, and the RESULT states the deviation and the knob (no flip asked
here, a flip is a decision row). P0 misses: DECISION. A P2 hash difference from P0's B values: RED for the knob, reported.
