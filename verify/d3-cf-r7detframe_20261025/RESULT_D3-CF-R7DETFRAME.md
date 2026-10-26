# RESULT D3-CF-R7DETFRAME: the D2 Enter tap frame (18/12/19, 72/69/70, 37/38/38) is set by the harness's wall-clock schedule and the render rate of each run, not by the input path. Detection latency is 0 frames in every build and in the original (2026-10-25)

Verdict: **GREEN.** Explanation only, no code change. Branch `orch/D3-CF-R7DETFRAME` from `orch/integration` @ `4ff9c8468`
carries this file and one extract. No build, no game run, no gate run (nothing executable changed).

## 1. What `det_frame` counts

`det_frame` in the `R7IN` line is `g_det_frame` (`exe_main.cpp:409`), bumped once per main-loop iteration
(`exe_main.cpp:9867`), i.e. once per rendered frame since boot. It is a frame counter, not a time.

## 2. When the D2 recipe taps Enter

The D2 arm env (`D2BASE`, `verify/r7flip_20261025/run_arms.py:226`, same in the R7INPUT and R7JOYPAD copies) sets
`MASHED_RACE_DEMO` + `MASHED_GOTO=6` and does **not** set `MASHED_DETERMINISTIC`. So `g_det_clock` is false
(`exe_main.cpp:9107`) and `DetTicks()` returns `GetTickCount()` (`exe_main.cpp:420-424`).

`RunRaceDemoStep` case 0 (`exe_main.cpp:1701-1717`): once on Challenge Select (`g_frontend_phase >= 3`, line 1701) it latches
`t_ms = DetTicks()`, returns until `DetTicks() - t_ms >= 600` (line 1715), then calls `NavDemoTap(DIK_RETURN)` (line 1717), which
launches the race. That tap is the `R7IN ... r7frame=0 test=0` line: `CookCtrlByteAFromKeyboard` runs later in the same loop
iteration (`exe_main.cpp:9911`, after `RunRaceDemoStep` at 9885 and `PollKeyboard` at 9872), sees `g_keys[0x1c] & 0x80`, and
logs the store on that frame.

So the tap fires 600 ms of **wall clock** after Challenge Select is reached, and `det_frame` = the frames rendered up to then.
The standalone has no frame limiter: `PresentationInterval = D3DPRESENT_INTERVAL_ONE` (`exe_main.cpp:2898`), so a frame waits
for the desktop's vblank, or longer when the GPU is busy.

## 3. Measured: the render rate differs per run, not per build

`data/d2_tap_lines.txt` (cited lines of all nine logs, sha256 there). `refresh` is `GetAdapterDisplayMode` of the desktop
(`exe_main.cpp:2906-2910`). The window is windowed, the app does not set it.

| step (run time) | desktop refresh | first InRace `render_fps` | tap det_frame | 0.6 s x fps |
|---|---|---|---|---|
| R7INPUT `D2_XI_1/2/3` (10:19-10:22) | 60 | 17.0 / 12.6 / 25.2 | 18 / 12 / 19 | 10 / 8 / 15 |
| R7JOYPAD `D2_XI_1/2/3` (18:37-18:40) | **120** | 75.0 / 59.0 / 91.0 (then 101-120) | 72 / 69 / 70 | 72 at 120 Hz |
| R7FLIP `D2_DEF_1/2/3` (19:56-19:59) | 60 | 54.0 / 52.0 / 51.0 (then 59-60) | 37 / 38 / 38 | 36 at 60 Hz |

- R7FLIP: 60 Hz vsync. 0.6 s x 60 = 36 frames, plus the latch frame and `GetTickCount` granularity, gives 37 / 38.
- R7JOYPAD: the desktop was at 120 Hz in that window. 0.6 s x 120 = 72, measured 69-72.
- R7INPUT: 60 Hz desktop, but the runs rendered at 12-29 fps (every SIMHZ line in those logs is 12.6-29.1). At those rates
  600 ms holds 8-17 frames, the same range as the measured 12-19. The menu-frame rate before the race is not logged, so the
  per-run count is not reproduced exactly. [UNCERTAIN] why that morning window rendered below vsync. The logs record no GPU or host load. The step was the
  only MASHED worker per its RESULT, but other processes on the machine are not logged.

The three builds differ in code (R7INPUT adds the keyboard cook, R7JOYPAD adds `PollJoypadButtons`, R7FLIP flips defaults), but
none of them touches the case 0 schedule, `DetTicks`, the present interval or the desktop mode. The spread follows the refresh
column and the fps column, and inside each step the three runs stay within 3 frames of each other.

D2 itself is unaffected: physics is fixed-step (`sim_hz` 59-60 in every log, `cfg=60`), all three steps scored PASS 3/3, and
two of R7INPUT's motion_diag files are byte-equal to U9779-M's knob-off default.

## 4. The original's detection path

Headless decomp of the anchored binary (pool slot 10, read-only): `FUN_004967e0` (callers `FUN_00429290`, `FUN_00492d20`) does,
in one pass:
1. `FUN_004972b0()`, which fills the keyboard bitmap `0x0077313c` from `GetDeviceState` via `FUN_00496100` (R7INPUT PREREG
   section 1). `FUN_00496100` calls the device's vtable slot `+0x24` (`GetDeviceState`, 0x100 bytes) and on failure slot `+0x1c`
   (`Acquire`) then `+0x24` again, and packs bit 7 of each key byte into the output bitmap.
2. `FUN_00496100(&DAT_00773058)` for the second bitmap.
3. For each slot 0..3 with `FUN_00497450(i) != 0`: `FUN_00496530()`, which stores `FUN_00497310(i, 4)` at `0x007f1042 + i*0x4c`
   (`0x004965ac..0x004965b7`). For a keyboard slot that reads bit `[cfg + 0x118]` (default binding `0x1c`, `0x00498698`) of the
   bitmap filled in step 1 (`0x004973fd..0x00497428`).

The device read and the byte-0xa store are in the same call, so a key that is down at the read is in byte 0xa on that frame:
0 frames of latency. The port has the same ordering (`PollKeyboard` at 9872, cook at 9911, same iteration), and every `R7IN`
line above sits on the tap's own frame. So the detection is faithful in all three builds.

## 5. Which number matches the original

None of the three is a property of the original. The original has no scripted tap: the frame a press lands on is set by
whoever presses, and the original detects it on that frame (section 4). What the three numbers measure is how many frames the
standalone rendered in a fixed 600 ms window. Under the original's boot recipe the d3d9 shim caps the original at 60 fps
(`MASHED_FPS_CAP`, default 60, CLAUDE.md runtime item 4), so the 60 fps regime of R7FLIP (37 / 38) is the one whose frame count
per wall-clock second equals the original's. That is a statement about the harness clock, not about input.

If a frame-exact tap is ever needed, `MASHED_DETERMINISTIC=1` makes `DetTicks()` a function of the frame index
(`exe_main.cpp:420-424`) and the tap would land at 36 + latch frames on every machine. The D2 recipe does not use it on
purpose (D2 arms are wall-clock bound, `d2-arms-are-wall-clock-bound`). No knob or change is proposed.

## 6. Scope

- Trackers: none touched. No function's confidence moves.
- [UNCERTAIN] R7INPUT's 12-29 fps window (section 3): no load data and no menu-frame fps were logged. Missing evidence: a
  per-frame timestamp in the menu. It does not change the explanation: the count is in the range 0.6 s x the logged rate gives.
- Evidence committed: this file, `data/d2_tap_lines.txt`. Raw logs stay on D: (paths and sha256 in the extract):
  `D:/MashedWT/D3-CF-R7INPUT_data/arms/D2_XI_{1,2,3}.log`, `D:/MashedWT/D3-CF-R7JOYPAD_data/arms/D2_XI_{1,2,3}.log`,
  `D:/MashedWT/D3-CF-R7FLIP_data/arms/D2_DEF_{1,2,3}.log`.
