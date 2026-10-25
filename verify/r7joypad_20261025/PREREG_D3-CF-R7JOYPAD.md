# PREREG D3-CF-R7JOYPAD: the joypad branch of the sub-state-7 input (ctrl byte 0xa, type-1 slots) under `MASHED_R7_INPUT`, then both R7 knobs ON re-measured on integration (2026-10-25)

Branch `orch/D3-CF-R7JOYPAD` from `orch/integration` @ `aed0e1bc7`. `orch/D3-CF-R7INPUT` @ `5a466becb` is **not** an ancestor
of integration (`git merge-base --is-ancestor` = 1). Its K357 part (`a30d080b5`) is (merged by `62e456875`). Its port commit
`2893ffa13` is not, so it is cherry-picked here as `1deedd619`. Joypad port `5006f7baa`. Exe `81783c39` (sha256 prefix,
`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not redeployed). Gate refs from `re/orchestrator/merge_train.json`:
G-CTRL `ef5f2e10 / 57f7ff83 / b4f4b55a`, B `c37d4c0c / 0a8f9806 / 7013671d`. This PREREG is committed alone, before any arm.

## 1. The original's joypad branch (Ghidra, pool slot 10, read-only)

Per frame `FUN_004967e0` calls `FUN_00495fe0` (poll), `FUN_004972b0` (snapshot), then `FUN_00496530` for each active slot,
which stores `FUN_00497310(i, 4)` at `0x007f1042 + i*0x4c` (`0x004965ac..0x004965b7`, the store the R7INPUT port already
writes for the keyboard).

- `FUN_00498510` (slot setup): slot i < J (J = game controllers): type 1 (`0x0049861a` `mov [esi+0x34], ebx`, EBX = 1),
  device index = i (`0x0049861d` `mov [esi+0x38], edi`, that is `[cfg + 0x140]`), action-4 binding 0
  (`0x00498640` `mov dword ptr [esi+0x10], 0x0`). Keyboard slot i == J: binding 0x1c (`0x00498698`).
- `FUN_00497310` type-1 branch: `cmp ecx,1` at `0x00497323`, code = `[cfg + 0x108 + 4*param_2]` (`0x00497330`), device =
  `[cfg + 0x140]` (`0x00497337`). `param_2 - 9 > 3` unsigned (`0x00497344..0x00497359`) goes to `0x004973da`:
  `al = [0x007730d4 + device*4 + (code >> 3)]`, test bit `code & 7`, result `0xff` or `0` (`0x004973da..0x004973f6`).
- `FUN_004972b0`: for d < `[0x00772fac]`, `FUN_004957a0(d, 0, 0x0077311c + d*8, 0, 0x007730d4 + d*4)`, which copies the
  dword at `0x00772290 + d*0x448` (`0x00495819`) to `0x007730d4 + d*4`.
- `FUN_00495fe0`: for d < `[0x00772fac]`, `FUN_00495870` with ESI = `0x00771e88 + d*0x448` (`0x00495fed`, `0x00495ffd`).
- `FUN_00495870` (device = `[ESI]`): Poll (vtbl `+0x64`, `0x00495886`). If < 0, Acquire (`+0x1c`, `0x00495890`), repeated
  while it returns `0x8007001e` (`0x00495893..0x004958ab`). GetDeviceState(`0x50`, buf) (`+0x24`, `0x004958b4..0x004958b7`).
  If < 0, return (`0x004958bc` `jl 0x00495e67`) with the bitmap untouched. Else zero 0x20 bytes at ESI + 0x408
  (`0x00495908..0x0049592f`) and set bit k for each k in 0..15 with byte `[buf + 0x30 + k]` negative (`0x00495970`, loop
  `cmp edi,0x11` / `jl` at `0x00495af3..0x00495afe`, step 8, two passes). Record + 0x408 = `0x00771e88 + 0x408` =
  `0x00772290`.
- `FUN_00495ee0` (enum callback): CreateDevice into `0x00771e88 + d*0x448`, SetDataFormat(`0x005d076c`) (vtbl `+0x2c`),
  SetCooperativeLevel(`[0x00772fb0]`, 5) (vtbl `+0x34`). Bytes at `0x005d076c`: `18 00 00 00 10 00 00 00 01 00 00 00 50 00
  00 00 2c 00 00 00 10 b3 49 00` (dwSize 0x18, dwObjSize 0x10, dwFlags 1, dwDataSize 0x50, dwNumObjs 0x2c, rgodf
  `0x0049b310`).

## 2. The port (`exe_main.cpp`, `MASHED_R7_INPUT`, default OFF)

- `CountGameCtrlCb`: with the knob ON, device j < 4 is kept in `g_joy[j]` with `SetDataFormat(&c_dfDIJoystick)` and
  `SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)`. **Deviation**: the original passes 5 (EXCLUSIVE |
  BACKGROUND). The port uses 6 so a concurrent `MASHED.exe` keeps its pad. The bits read do not depend on it. Knob OFF: the
  device is released as before, no joystick is polled.
- `PollJoypadButtons` (each cook): Poll, Acquire on failure (repeated on DIERR_INPUTLOST), GetDeviceState(`sizeof(DIJOYSTATE)`
  = 0x50). On failure the device's bits keep last frame's value. Else bits 0..15 = `rgbButtons[k] & 0x80`.
- `CookCtrlByteAFromKeyboard`: for a type-1 slot i, `val = (bits[i] bit 0) ? 0xff : 0` (binding 0, device i), stored at
  `0x007f1042 + i*0x4c`. Type-2 slots unchanged. The axes, the block zeroing and the shadow copy are not ported.
- `MASHED_R7_INPUT_PAD_PRESS=<n>` (arm test): ORs bit 0 into joypad 0's bits on the n-th sub-state-7 cook frame (only when
  slot 0 is type 1). The `R7IN` log line gains a trailing ` type=<1|2>`.

On this machine the boot log reads `input slots: game controllers 1, types 1 2 0 0`, so slot 0 (the player car through the
port's `v -> v` slot table, `TrackRenderer.cpp:7788`) is the pad and slot 1 the keyboard. The gate
(`TrackRenderer::RoundEnd7ExitTick`, `0x00410903..0x004109b9`) reads byte 0xa of block `[0x007f1a14 + v*0x10]` for v 0..3,
so a pad press reaches it through v = 0. Every arm log is checked for that line. If it reads `game controllers 0`, slot 0 is
the keyboard, the pad arms (P1p, P2p) are **NOT SCORED** and reported as such, and the other rows still hold.

## 3. Arms (`verify/r7joypad_20261025/run_arms.py`, root `D:\MashedWT\D3-CF-R7JOYPAD_root`, `mashed_lock.py`)

Stacks: `I` = `MASHED_R7_INPUT`, `X` = `MASHED_R7_EXIT`, `XI` = both, `XIP<n>` = XI + `MASHED_R7_INPUT_PRESS=<n>`,
`XIPP<n>` = XI + `MASHED_R7_INPUT_PAD_PRESS=<n>`.

| id | arm | prediction |
|---|---|---|
| P0 | `H_DEF`, `B_DEF_K0 / K1 / K26` | G-CTRL `ef5f2e10 / 57f7ff83 / b4f4b55a`, B `c37d4c0c / 0a8f9806 / 7013671d` |
| P0b | `B_I_K0` | `c37d4c0c`, no `R7IN` line |
| P0c | `B_X_K0`, `B_XI_K0` | equal to each other, expected `3a03670c` (R7INPUT's value). Equal to each other but not to `3a03670c` = an integration delta, reported, not a failure of this step |
| P1 | `B_XIP150_K0` (keyboard) | as in R7INPUT: `R7IN slot=1 ... test=1 ... type=2` and `R7X EXIT` on the same `det_frame`, round 1 -> 2. Step CSV expected `dbe10a3e` |
| P2 | `B_XIP50_K0` (keyboard) | `R7IN ... r7frame=50 test=1`, no `R7X EXIT`, step CSV equal to `B_X_K0` |
| P1p | `B_XIPP150_K0` (pad) | `R7IN byte0xa slot=0 val=0xff r7frame=150 test=1 ... type=1` and an `R7X EXIT` line on the same `det_frame` (counter 300 > 199, `0x004109b9`), `round 1 -> 2`. Step CSV expected equal to `B_XIP150_K0` (same exit frame). A different step CSV with the exit lines holding is reported with its first differing row, not a failure |
| P2p | `B_XIPP50_K0` (pad) | `R7IN ... slot=0 ... r7frame=50 test=1 ... type=1`, no `R7X EXIT`, step CSV equal to `B_X_K0` |
| P3 | every arm | no `R7IN` line with `test=0`, except D2's scripted Enter tap before the race (R7INPUT section 3 note, `exe_main.cpp` `NavDemoTap(DIK_RETURN)`). Any other `test=0` line is a real key or button press: that arm is re-run |
| report | `H_XI`, `B_XI_K1 / K26` | expected R7INPUT's `5943c5f1 / b493262b / 09551e6a` and `2516d903 / 53778664`. These are the expected new refs if both knobs flip |
| P4 | `D2_XI_1 / 2 / 3` under `run --solo`, `verify/d2rebase_20261019/score_d2_v2.py port` | PASS 3/3. motion_diag at least 1600 lines, else re-run (up to 2) |
| P5 | 13 tracks, `verify/rsky_20261016/run13.py run ... MASHED_R7_EXIT=1 MASHED_R7_INPUT=1`, root `D:\MashedWT\D3-CF-R7JOYPAD_root13`, scored against `verify/rres_20261016/cites_rres.json` | 0 RED, INSTRUMENT GATES PASS, score table equal to `verify/r7input_20261025/data/tracks_XI_score.txt` (on `orch/D3-CF-R7INPUT`). Rows that differ are reported per track |

Every MASHED run under `mashed_lock.py run --label D3-CF-R7JOYPAD` (D2 with `--solo`), started only when no solo lock is
held. Piz-collision crashes (U-9677) are re-runs. Raw arm data stays in `D:\MashedWT\D3-CF-R7JOYPAD_data`; the RESULT lists
each cited raw file's path and SHA-256.

## 4. Outcome rules

- P0 fails: the port changed the default. RED, stop.
- P1p or P2p fails (scored): the joypad branch or its timing is wrong. RED for the joypad port, D2 / 13 tracks still reported.
- P1 / P2 fail: the cherry-pick or the rewrite broke the keyboard branch. RED.
- P4 / P5 out of band with the knobs ON: reported as the cost of the knobs, input to the flip decision.
- All hold: the port is GREEN with knobs OFF, and the flip of both knobs goes back as DECISION with the measured hashes.
  Nothing is flipped here.

## 5. Scope

- [UNCERTAIN] the slot table: in the original the human car's `0x007f1a14` entry is the profile that confirmed in the
  frontend. The port seeds `v -> v`. The pad press reaches the gate through v = 0 here because slot 0 is the pad on this
  machine, not because the port reproduces the original's mapping.
- Not measured: a physical button press (no hand on the pad in unattended arms). `MASHED_R7_INPUT_PAD_PRESS` feeds the same
  bits after the poll. The poll itself is exercised in every XI arm (the controller is attached), and a failed poll leaves
  bits 0, which the 0-press rows check.
