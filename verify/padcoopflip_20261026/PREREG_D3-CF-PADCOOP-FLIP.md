# PREREG D3-CF-PADCOOP-FLIP: the port always passes the original's pad cooperative level

Decision L-20 (`log/orch/DECISIONS.md`, Mariano, 2026-10-26): level 5 ON, delete the old value and the knob, re-run the
pad and keyboard checks, decided-flip protocol if a hash moves.

## 1. The change (`mashedmod/src/mashed_re/exe_main.cpp`, `CountGameCtrlCb`)

`SetCooperativeLevel(g_hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND)` (5, `0x00495faf push 5`) on every kept device. The old
0xa (`DISCL_NONEXCLUSIVE | DISCL_BACKGROUND`) and `MASHED_PAD_COOP_ORIG` / `PadCoopOrigOn()` are deleted. The
`pad coop:` / `pad state:` log lines stay.

Branch `orch/D3-CF-PADCOOP-FLIP` from `orch/integration` @ `c2bf772f8`. Runner `verify/padcoopflip_20261026/run_arms.py`
(copy of `verify/padcoop_20261025/run_arms.py`: paths changed, `PC` / `PCPP150` removed with the knob, `KP150` added).
Private root `D:/MashedWT/D3-CF-PADCOOP-FLIP_root`, raw data `D:/MashedWT/D3-CF-PADCOOP-FLIP_data`.

## 2. Arms and predictions

| id | arm | prediction |
|---|---|---|
| P0 | `H_DEF` | G-CTRL `5943c5f1 / b493262b / 09551e6a` (`merge_train.json` `gctrl.ref`, one retry allowed) |
| P0 | `B_DEF_K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` (`b.ref`). PADCOOP's `B_PC_K*` (knob ON = this exe's only path) gave these |
| P1 | every arm log | `pad coop: device <d> level 0x5 hr 0x00000000` per kept device, no `level 0xa` line |
| P2 | `B_PP150_K0` (pad button 0, R7 exit) | `R7IN ... slot=0 ... r7frame=150 test=1 det_frame=551 type=1`, `R7X EXIT det_frame=551 round 1 -> 2`, again at 1058 `round 2 -> 3`. Step `dbe10a3e` |
| P3 | `B_KP150_K0` (Enter = 0x1c, R7 exit) | `R7IN ... r7frame=150 test=1 det_frame=551 type=2` (keyboard slot = controller count), `R7X EXIT det_frame=551`, again at 1058. Step `dbe10a3e` (R7JOYPAD's `B_XIP150_K0`) |

Outcome: all hashes equal = GREEN. Any gate hash moving = decided flip: re-run the moved arms, record the new hashes on
two runs, report GREEN-FLIP. A press arm that does not exit = RED.

Limit, stated before the run: both press arms inject at the cooked lookup (`MASHED_R7_INPUT_PRESS` /
`MASHED_R7_INPUT_PAD_PRESS`), not through `GetDeviceState`, so they test the exit path with level 5 in place, not a
physical press. The unfocused path (failed Acquire keeps last bits, `0x004958bc`) stays unmeasured, as in PADCOOP P3.
