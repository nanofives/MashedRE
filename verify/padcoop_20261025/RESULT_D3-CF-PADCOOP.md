# RESULT D3-CF-PADCOOP: the joypad cooperative level

**Verdict: GREEN.** The original passes 5 = DISCL_EXCLUSIVE | DISCL_FOREGROUND (`0x00495faf push 5`, call at `0x00495fb3`).
The port's default passes 0xa = DISCL_NONEXCLUSIVE | DISCL_BACKGROUND, not "6" as the R7JOYPAD text says. A default-OFF
knob `MASHED_PAD_COOP_ORIG=1` now passes the original's 5. Default gates unchanged: G-CTRL `5943c5f1 / b493262b / 09551e6a`
(on the merge train's allowed retry), B `3a03670c / 2516d903 / 53778664`, D2 PASS 3/3, 13 tracks 0 RED with a score table
identical to R7JOYPAD's. With the knob ON and no hand on a pad, the B hashes are equal to the default's. No flip asked.

Branch `orch/D3-CF-PADCOOP` from `orch/integration` @ `4ff9c8468`. Code `597b20729`, PREREG `73c9dea40` (committed alone,
before the arms). Exe sha8 `220aa7a7`, built with `MASHED_NO_DEPLOY=1`.

## 1. The original (`original/MASHED.exe.unpatched`, capstone)

- `FUN_00496040` registers `FUN_00495ee0` as the EnumDevices callback (`push 0x495ee0` at `0x0049605c`, call through vtbl
  `+0x10` at `0x00496069`, count `0x00772fac`).
- `FUN_00495ee0`: CreateDevice (`0x00495f10`), SetDataFormat(`0x005d076c`) (`0x00495f86`), then `mov eax,[0x00772fb0]`
  (`0x00495fa5`), `push 5` (`0x00495faf`), `push eax`, `push ebx`, `call [edx+0x34]` (`0x00495fb3`) = SetCooperativeLevel.
  A failing hr prints string `0x005cfeac` and continues (`0x00495fb6..0x00495fc4`).
- `[0x00772fb0]` is the hwnd argument of `FUN_004960a0` (`0x004960a5..0x004960a9`), zeroed by `FUN_00496010` at `0x00496038`.
  These are the only 3 references to `0x00772fb0` in `.text`.
- dinput.h (Windows Kits 10): `DISCL_EXCLUSIVE 1`, `DISCL_NONEXCLUSIVE 2`, `DISCL_FOREGROUND 4`, `DISCL_BACKGROUND 8`.
  5 = EXCLUSIVE | FOREGROUND.
- The read path is the same for both levels: `FUN_00495870` Poll (`0x00495886`), Acquire on failure (`0x00495890`),
  GetDeviceState (`0x004958b4`), return with the bits untouched on a failed read (`0x004958bc`). Under DirectInput's
  documented rules a FOREGROUND device cannot be acquired while its window is not the foreground window, so the original
  keeps the last bits while unfocused and the port's default keeps reading.

Correction to `verify/r7joypad_20261025/{PREREG,RESULT}_D3-CF-R7JOYPAD.md` and decision row L-16: the port's level is 0xa
(not 6), and the original's 5 is EXCLUSIVE | FOREGROUND (not EXCLUSIVE | BACKGROUND). The code comment above `g_joy` in
`exe_main.cpp` now names the flags correctly. Those two committed files are left as they are (history), this RESULT is
the correction.

## 2. The change (`exe_main.cpp`)

- `CountGameCtrlCb`: `MASHED_PAD_COOP_ORIG=1` passes `DISCL_EXCLUSIVE | DISCL_FOREGROUND`. Default: 0xa, unchanged.
- Log only, both paths: `pad coop: device <j> level 0x<l> hr 0x<hr>` once per kept device, and `pad state: device <d>
  GetDeviceState hr 0x<hr>` from `PollJoypadButtons` each time that result changes (first frame included).

## 3. Results (`data/run_gates.txt`, `data/run_d2.txt`, `data/score_d2_DEF.txt`, `data/t13.out`, `data/tracks_DEF_score.txt`, `data/pad_log_lines.txt`)

Machine state for every arm: `input slots: game controllers 2, types 1 1 2 0, active 3`.

| id | arm | measured | prediction |
|---|---|---|---|
| P0 | `H_DEF` | attempt 1: `40b4fc1f / 80a08246 / 56390628`, step 8279 lines, last frame 7837. Each of its three files is a byte prefix of attempt 2's (`cmp -n`). Attempt 2 (`H_DEF-2`): `5943c5f1 / b493262b / 09551e6a` (14659 / 56873 / 28436 lines) | **holds on the retry**. `merge_train.json` gives G-CTRL `retries: 1`, and `merge_train.ps1:182` reruns a G-CTRL run for this case ("a G-CTRL run can drop one frame's rows") |
| P0 | `B_DEF_K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` | **holds** |
| P1 | every DEF arm log | `pad coop: device 0 level 0xa hr 0x00000000`, the same for device 1 | **holds** |
| P2 | `B_PC_K0 / K1 / K26` | `level 0x5 hr 0x00000000` for devices 0 and 1. Step `3a03670c / 2516d903 / 53778664` | **holds** |
| P3 | `B_PC_K*` logs (report only) | one `pad state ... hr 0x00000000` per device and no later change: every read succeeded, so each window was foreground for the whole run | report. [UNCERTAIN] the knob's unfocused path (failed Acquire, bits kept) is not measured here. An arm that takes focus away mid-run is needed |
| P4 | `B_PP150_K0`, `B_PCPP150_K0` | both: `R7IN byte0xa slot=0 val=0xff r7frame=150 test=1 det_frame=551 type=1`, `R7X EXIT det_frame=551 round 1 -> 2`, again at 1058 `round 2 -> 3`. Step `dbe10a3e` for both, equal to R7JOYPAD's `B_XIPP150_K0` | **holds** |
| P5 | `D2_DEF_1 / 2 / 3`, `--solo`, `score_d2_v2.py port` | **PASS 3/3**: slip1500 0.1900 x3, slip2000 0.2550 x3, drvmed 1966.66 / 1965.35 / 1965.67, H1 398/400 x3. motion_diag 1634 / 1630 / 1632 lines | **holds** |
| P6 | 13 tracks, `run13.py run` (default), scored vs `cites_rres.json` | 13/13 OK after 1 attempt, 0 RED, INSTRUMENT GATES PASS. Table identical to `verify/r7joypad_20261025/data/tracks_XI_score.txt` (41 / 41 non-blank lines) | **holds** |

No piz-collision crash (U-9677). Only the PIDs in `data/RUNLOG.txt` and `data/t13.out` were started, none was killed.

## 4. What stays a deviation and why the knob stays OFF

With no pad input the two levels give equal hashes. They differ in two cases, neither exercised by any gate: (1) the
window is not foreground (original keeps the last bits, the default port keeps reading the pad), (2) another process
holds the pad exclusively (EXCLUSIVE acquire fails, the default port's NONEXCLUSIVE acquire does not). Case (1) is the
reason the default keeps 0xa: unattended harness runs are often unfocused, and the original only keeps running unfocused
because of the `no_focus_pause` boot patch (`0x004996d3`). A default flip to 5 would be a decision row. None is filed
here because no measurement separates the two levels yet.

Owed (not done here): the P3 unfocused arm. Trackers: no C-level change claimed, nothing for re-classify.

## 5. Raw data (stays on D:)

`data/raw_sha256.txt` lists the SHA-256 of 238 files under `D:\MashedWT\D3-CF-PADCOOP_data` (`arms\*`, `t13\*`, the
`.bmp` frame shots not hashed). Cited raw files:

| file | SHA-256 |
|---|---|
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/H_DEF-2.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/H_DEF-2.gates.csv` | `b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/H_DEF-2.ptrace.log` | `09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/H_DEF.step.csv` (attempt 1, truncated) | `40b4fc1f831e2eab7dc2687af1263c74f5a383cdd8d8bc698581da618bf82083` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/B_DEF_K1.step.csv` | `2516d9034a1774893bf2f59edd368c3783ad6cfe0ed491dc68b3d84a0afec7f6` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/B_PC_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/B_PCPP150_K0.step.csv` | `dbe10a3e8da1323d9891f0980d1bc9a0ee1cfb23c91287e6393b0e66e4be26b6` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/B_PP150_K0.step.csv` | `dbe10a3e8da1323d9891f0980d1bc9a0ee1cfb23c91287e6393b0e66e4be26b6` |
| `D:/MashedWT/D3-CF-PADCOOP_data/arms/D2_DEF_{1,2,3}.motion_diag.log` | `7f0e420a...`, `1c1d5b1b...`, `54a7c537...` (full values in `data/raw_sha256.txt`) |
