# RESULT D3-CF-PADCOOP-FLIP: the pad cooperative level is always the original's 5

**Verdict: GREEN.** The port now passes 5 = DISCL_EXCLUSIVE | DISCL_FOREGROUND (`0x00495faf push 5`) on every kept
joypad. The old 0xa (DISCL_NONEXCLUSIVE | DISCL_BACKGROUND), `PadCoopOrigOn()` and `MASHED_PAD_COOP_ORIG` are deleted
(decision L-20, `log/orch/DECISIONS.md`). No gate hash moved: G-CTRL `5943c5f1 / b493262b / 09551e6a` (on the merge
train's allowed retry, see P0), B `3a03670c / 2516d903 / 53778664`, equal to `re/orchestrator/merge_train.json`
`gctrl.ref` / `b.ref`. R7 exit via pad button 0 and via Enter both hold. No new refs for L0 to commit.

Branch `orch/D3-CF-PADCOOP-FLIP` from `orch/integration` @ `c2bf772f8`. Code `b1f9b777f`, PREREG `d3a2b8090` (committed
alone, before the arms), runner `6e67a497e`. Exe sha8 `f9c46750`, built with `MASHED_NO_DEPLOY=1`. Private root
`D:/MashedWT/D3-CF-PADCOOP-FLIP_root`.

## 1. The change (`mashedmod/src/mashed_re/exe_main.cpp`, `CountGameCtrlCb`)

`const DWORD level = DISCL_EXCLUSIVE | DISCL_FOREGROUND;   // 0x00495faf push 5`. The comment above `g_joy` names the
deleted value and knob. The `pad coop:` / `pad state:` log lines stay. `git grep PAD_COOP_ORIG` outside
`verify/padcoop_20261025/` and the ledger event log: only that comment.

## 2. Results (`data/run_gates.txt`, `data/RUNLOG.txt`, `data/pad_log_lines.txt`)

Machine state for every arm: `input slots: game controllers 2, types 1 1 2 0, active 3`.

| id | arm | measured | prediction |
|---|---|---|---|
| P0 | `H_DEF` | attempt 1: step `5943c5f1` (14659), gates `b493262b` (56873), ptrace `35d0566b` (53552 lines). The ptrace file holds two interleaved player traces: `f=1` starts at lines 1 and 445, and line 610 begins mid-record. Step and gates are equal to the ref. Attempt 2 (`H_DEF-2`): `5943c5f1 / b493262b / 09551e6a` (14659 / 56873 / 28436 lines) | **holds on the retry** (`merge_train.json` `gctrl.retries: 1`). [UNCERTAIN] the second writer of attempt 1's `player_trace.log` (CWD-relative, in the private root) is not identified: the runner started one PID per arm (`data/RUNLOG.txt`), and a process watch during attempt 2 saw only pid 26028 on this exe (the other 9 were `D:\MashedWT\D4-DC-4d20-FLIP_data\exe_base\mashed_re.exe`). The level change cannot write a second player trace: it runs once per enumerated device at init |
| P0 | `B_DEF_K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` | **holds** |
| P1 | every arm log | `pad coop: device 0 level 0x5 hr 0x00000000`, the same for device 1. No `level 0xa` line | **holds** |
| P2 | `B_PP150_K0` (pad button 0) | `R7IN byte0xa slot=0 val=0xff r7frame=150 test=1 det_frame=551 type=1`, `R7X EXIT det_frame=551 round 1 -> 2`, again at 1058 `round 2 -> 3`. Step `dbe10a3e` | **holds** |
| P3 | `B_KP150_K0` (Enter, 0x1c) | `R7IN byte0xa slot=2 val=0xff r7frame=150 test=1 det_frame=551 type=2`, `R7X EXIT det_frame=551 round 1 -> 2`, again at 1058 `round 2 -> 3`. Step `dbe10a3e` (= R7JOYPAD's `B_XIP150_K0`) | **holds** |

No piz-collision crash (U-9677). Only the PIDs in `data/RUNLOG.txt` (7448, 2156, 18416, 19824, 28680, 23972, 26028) were
started, none was killed.

## 3. The unfocused path, now measured

Every arm here logs one `pad state: device <d> GetDeviceState hr 0x8007000c` per device and no later change.
0x8007000c is DIERR_NOTACQUIRED: with FOREGROUND the Acquire fails while the window is not foreground, so the pad is not
read for the whole run, and the gate hashes are still equal to the ref. This is PADCOOP's owed P3 case (window not
foreground), and the port now does what the original does there (`0x00495890` Acquire fails, `0x004958bc` returns with the
bits untouched). PADCOOP's `B_PC_K*` arms had `hr 0x00000000` (foreground). Limit: both press arms inject at the cooked
lookup (`MASHED_R7_INPUT_PRESS` / `MASHED_R7_INPUT_PAD_PRESS`), not through `GetDeviceState`, so a physical button press
under level 5 is not measured. A focused, hand-pressed run would cover it.

Consequence for later harness work: an unattended standalone run no longer reads a physical pad unless its window is
foreground. Keyboard reads are unchanged (the keyboard keeps its own level).

Trackers: no C-level change claimed, nothing for re-classify.

## 4. Raw data (stays on D:)

`data/raw_sha256.txt` lists the SHA-256 of all 19 files under `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms`. Cited raw files:

| file | SHA-256 |
|---|---|
| `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms/H_DEF-2.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms/H_DEF-2.gates.csv` | `b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f` |
| `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms/H_DEF-2.ptrace.log` | `09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1` |
| `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms/H_DEF.ptrace.log` (attempt 1, two traces) | `35d0566ba07c644800b2b695e9cc503640fa012bcc9fe49ca24a608698ec3349` |
| `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms/B_DEF_K1.step.csv` | `2516d9034a1774893bf2f59edd368c3783ad6cfe0ed491dc68b3d84a0afec7f6` |
| `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms/B_PP150_K0.step.csv` | `dbe10a3e8da1323d9891f0980d1bc9a0ee1cfb23c91287e6393b0e66e4be26b6` |
| `D:/MashedWT/D3-CF-PADCOOP-FLIP_data/arms/B_KP150_K0.step.csv` | `dbe10a3e8da1323d9891f0980d1bc9a0ee1cfb23c91287e6393b0e66e4be26b6` |
