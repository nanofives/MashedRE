# Results — verify a9da810a in a built exe (2026-10-01)

Pre-registration: `PREREGISTRATION.md`, committed `3e8ee34c` before any game launched.
Static evidence: `STATIC_EVIDENCE.md`. Per-run title logs: `T*.json`, `t6_*.json`.

Exe under test: `verify/freecam_title_20261001/bin/mashed_re.exe`,
SHA-256 `9FF13F2797C95281107DCE737739EE6E9B48926B61570289F721F490D986E5F9`,
1,970,688 bytes, built 2026-10-01 11:02:19 from HEAD `ce36144e` (which has `a9da810a`
as an ancestor). Byte-identical to the `mashedmod/build/mashed_re.exe` that every run
below launched (hashes compared).

No defect found. `a9da810a` behaves as its commit message describes.

## T1 — title with MASHED_TITLE set: **PASS**

Env `MASHED_MUTE=1 MASHED_WIN_POS=left-bl MASHED_TITLE=T1 freecam title verify`, pid 41224.
Observed (`T1.json`), read with `GetWindowTextW` on the HWND of the spawned PID:

```
+ 4.21s  Mashed RE | T1 freecam title verify | menu
```

Exactly the pre-registered string. `MASHED_WIN_POS` correctly does NOT leak into the
label when `MASHED_TITLE` is set.

Incidental, pre-existing, NOT a defect and NOT introduced by `a9da810a`: two startup
titles precede it — `Mashed RE (Milestone B3 - 8 textures decoded)` at +0.20s and
`Mashed RE (Milestone B5 - atlas: 8 textures)` at +3.01s. These come from the
window-creation path (`g_windowTitle`, `exe_main.cpp:450`), which runs before the main
loop first calls `UpdateWindowTitle`. The title converges to the new format and stays
there for the rest of the run.

## T2 — title without MASHED_TITLE, other MASHED_* set: **PASS**

Env `MASHED_MUTE=1 MASHED_WIN_POS=left-bl MASHED_SIM_HZ=60`, pid 37336. Observed:

```
+ 0.60s  Mashed RE | SIM_HZ=60 WIN_POS=left-bl | menu
```

Both variables listed as `NAME=value` with the `MASHED_` prefix stripped
(`exe_main.cpp:1647`), space-separated, sorted as Windows orders the environment block.
`MUTE` is excluded (`:1645`) and `TITLE` is excluded (`:1646`) — `MUTE` was set on this
run and does not appear. The probe strips every inherited `MASHED_*` before spawning and
the parent shell had none, so this label is exactly the set the run passed.

## T3 — title with no MASHED_* vars: **PASS**

Env `MASHED_MUTE=1` only (the minimum legal environment: an unmuted run is forbidden and
`MUTE` is excluded from the label), pid 36448. Observed:

```
+ 0.60s  Mashed RE | manual play | menu
```

Exactly the pre-registered string (`:1661`).

## T4 — state field follows GameFlow: **PASS** (one sub-state UNMEASURABLE)

State sequence, env adding `MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_RESULT_DEMO=1`,
pid 2204 (`T4_states.json`):

```
+ 0.60s  Mashed RE | T4 states | menu
+ 1.40s  Mashed RE | T4 states | loading race
+ 2.21s  Mashed RE | T4 states | race
+10.02s  Mashed RE | T4 states | results
```

`menu -> loading race -> race -> results`, in order, no out-of-order state. Matches the
`GameMode` mapping at `exe_main.cpp:1664-1672`.

Frame index, env adding `MASHED_DETERMINISTIC=1`, pid 37188 (`T4_detframe.json`):

```
+ 0.60s  Mashed RE | T4 det | menu | frame 0
+ 0.80s  ... frame 30      + 1.00s  ... frame 60     + 1.21s  ... frame 90
+ 1.41s  ... frame 120     + 1.61s  ... frame 150    + 1.81s  ... frame 210
+ 2.01s  ... frame 240     ... monotonically to frame 360 at +2.81s
```

` | frame N` appended, every N a multiple of 30, strictly increasing
(`:1674-1676`). The absent `frame 180` is the probe's 0.2 s poll interval sampling
faster-than-poll title changes, not a gap in the exe's output.

`race (paused)` — **UNMEASURABLE**, exactly as pre-registered. The only writer of the
pause flag is an Esc rising edge read from DirectInput under window focus, and it is
explicitly skipped whenever `g_race_demo`/`g_det_clock` is set (`:1869-1878`), which is
every unattended path. Reaching it needs physical keys with focus. Not worked around.

## T5 — the free camera is gone: **PASS** (static + output-validated; key-delivery leg UNMEASURABLE)

**T5a static (this is what decides the verdict) — CONFIRMED.** In `exe_main.cpp`,
`ci.dt = dt` (`:2887`) is the *only* assignment to any `CamInput` field, and
`move_fwd|move_strafe|move_up|yaw_delta|pitch_delta|reset_orbit|VK_RBUTTON` have zero
matches anywhere in the file. The consumer requires a nonzero motion field to enter free
mode (`D3d9Render/TrackRenderer.cpp:5176-5177`) and assigns `free_ = true` only inside
that branch (`:5179-5187`). An all-zero `CamInput` therefore makes `free_` unreachable, so
`chase_cam = car_ready_ && !free_` (`:5497`) is true and the race camera drives the view.
This covers *all* inputs, not just the ones a test could deliver. Full grep output in
`STATIC_EVIDENCE.md`.

**T5b output channel — VALIDATED.** In-race backbuffer captures under
`MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=900`, SHA-256 of `race1/01_inrace_track.bmp`:

| arm | hash (first 24) | meaning |
|---|---|---|
| `t5_ctrl` | `CD9D2380B30A38432BCA5CDC` | control |
| `t5_repeat` | `CD9D2380B30A38432BCA5CDC` | bit-identical repeat -> capture is reproducible |
| `t5_campose` | `999B873AF1FD1BBD8DA9714F` | `MASHED_CAM_POSE=-15,150,-8,-15,4.4,-7.9` -> **differs** |

Same pattern on `01_grid.bmp` and `01_action.bmp`. So the channel is reproducible AND
camera-sensitive: a pose change does move the pixels. A later "no difference" is therefore
a real null, not a dead channel. (The pinned pose was chosen from the run's own
`CAM-DIAG bbox_center=(-15.0,4.4,-7.9) bbox_R=80.5`.)

**T5c posted-key arm — ran, NOT evidence.** 924 `WM_KEYDOWN`/`WM_KEYUP` messages for
W/A/S/D/Q/E/R plus 66 right-button drag sweeps, posted to the spawned PID's HWND
(`post_freecam_keys.py`, pid 37684). All three in-race captures came out bit-identical to
`t5_ctrl`. Consistent with the free camera being gone — but as pre-registered this result
**cannot pass or fail T5**, for two independent reasons: `PostMessage` does not write the
DirectInput device-state buffer that `g_keys[]` is read from (`exe_main.cpp:1023`), and the
arm runs under `MASHED_DETERMINISTIC`, which suppressed live input by design even before
`a9da810a`. It would have come out identical on a build where the free camera still
existed.

**T5d real free-fly key delivery — UNMEASURABLE.** Needs physical keys or global
`SendInput` plus window focus. Per the task rules, reported rather than worked around.

## T6 — arrows still drive the car; B11/B12 gone: **PASS** (arrow-key *delivery* UNMEASURABLE)

**T6a mapping intact — CONFIRMED.** `di.accel`/`di.steer` are still read from
`DIK_UP/DOWN/RIGHT/LEFT` at `exe_main.cpp:2936-2938`, and those two lines appear in
`git show a9da810a` as unchanged **context** lines (leading space). The commit removed only
the two `ci.yaw_delta = 0.f; ci.pitch_delta = 0.f;` lines that followed them, which existed
solely to take the arrows back off the deleted free camera.

**T6b the drive path moves the car — CONFIRMED, matched frame, against a control.** Both
arms deterministic, same `MASHED_DET_FRAMES=900`, compared at the single `R5 drive` sample
(`s_frame == 340`, `exe_main.cpp:3923-3927`), which both arms reach at `t=5.7s`:

| arm | `R5 drive` line |
|---|---|
| `t6_ctrl` (no input) | `t=5.7s pos=(-26.56, 0.04, 17.01) speed=0.00` |
| `t6_drive` (`MASHED_PLAY_DEMO=1`) | `t=5.7s pos=(-24.75, 0.04, 23.22) speed=28.94` |

Position moved by (+1.81, 0.00, +6.21) and speed went 0.00 -> 28.94 against the no-input
control, so `DriveInput` is live end-to-end and `a9da810a` did not break car driving. Full
logs: `t6_ctrl.full.log`, `t6_drive.full.log`.

Coverage note, worth recording: the first attempt reported `NO R5 LINE` for the control.
That was not a finding about the control — `mashed_re.log` is truncated per run
(CWD-relative, `kLogPath` at `:447`), so the byte-offset tail slicing used to isolate the
control's log was invalid and the control's log had already been overwritten by the drive
arm. Both arms were re-run individually with the whole log copied immediately after each.
An absent log line was not allowed to stand as a measurement.

Corroborated on the pixel channel (`CAPTURE_HASHES.txt`): `t6_ctrl`'s
`race1/01_inrace_track.bmp` is bit-identical to `t5_ctrl`'s (same env, so the capture is
reproducible across sessions of this run set), while `t6_drive`'s differs
(`C6F12958FD80EEAFB1A0E36A4B7C261D…`). The car moved on both the numeric and the pixel
channel.

`MASHED_PLAY_DEMO` writes `di.accel`/`di.steer` at `:2924-2928`, i.e. the same two fields
and the same consumer as the arrow-key branch, so this validates the plumbing the arrows
feed. It does not itself prove the key *read*; T6a covers that line, and delivering real
arrow keys is **UNMEASURABLE** for the same DirectInput reason as T5d.

**T6c B11/B12 removed — CONFIRMED on source and in the pinned binary.**
`g_titleQuadOffsetX/Y`, `kTitleQuadStep`, `UpdateTitleFromKeyboard`,
`UpdateQuadFromKeyboard` have zero matches in `exe_main.cpp`. Strings in the pinned exe:

| string | in exe |
|---|---|
| `Mashed RE \| %s \| %s` | PRESENT |
| `Mashed RE \| %s \| %s \| frame %u` | PRESENT |
| `manual play` | PRESENT |
| `MASHED_MUTE=` / `MASHED_TITLE=` | PRESENT |
| `Mashed RE (B11` | ABSENT |
| `Mashed RE (B13` | ABSENT |
| `B11 keys` | ABSENT |
| `quad@` | ABSENT |

The new format strings are in the shipped binary and none of the old held-key / title-quad
strings survive, so the exe under test really does carry `a9da810a`.

## Promotions

None. `a9da810a` is standalone glue in `exe_main.cpp` with no original RVA behind it — no
`hooks.csv` row, no `diff-original` leg, nothing to promote on the C0..C4 ladder.

## What is committed, and what is not

The repo ignores `*.exe` (`.gitignore:70`), `verify/**/*.bmp` (`:139`) and `*.log` (`:18`).
So the pinned exe and the 50-odd `.bmp` captures stay on disk in this directory and are
NOT in git; their SHA-256 values in `CAPTURE_HASHES.txt` are the committed evidence for
them. The two `R5 drive` logs are load-bearing for T6b and small, so they are force-added
(`t6_ctrl.full.log`, `t6_drive.full.log`). Re-running any arm is reproducible: the
deterministic captures came back bit-identical across two runs of the same env.

## Process hygiene

Thirteen launches. The eleven whose records survive (`*.json`, each recording
`pid == killed_pid`): T1 41224, T2 37336, T3 36448, T4_states 2204, T4_detframe 37188,
T5_ctrl 7196, T5_repeat 1224, T5_campose 37508, T5_postkeys 37684, t6_ctrl 8808,
t6_drive 38692. The first `t6_ctrl`/`t6_drive` pair was re-run after the log-isolation
problem above, and their JSON was overwritten by the re-runs. Every launch was tracked by
the harness and killed with `Popen.kill()` on its own PID only — no kill by name, so no
other session's MASHED was touched. Every run had `MASHED_MUTE=1`.
`MASHED_NAV_DEMO` was never set. `mashedmod/src` was not edited.
