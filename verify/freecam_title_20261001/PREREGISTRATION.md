# Pre-registration — verify a9da810a in a built exe (2026-10-01)

Commit under test: `a9da810a` "standalone: remove the placeholder free-fly camera; window
title shows what the run is testing". Touches only `mashedmod/src/mashed_re/exe_main.cpp`
(+65 / -134). Ancestor of HEAD `ce36144e`: **yes** (`git merge-base --is-ancestor` exit 0).

This file is committed BEFORE any game is launched. Checks, expected values, pass/fail
criteria and the known channel limitations are all fixed here.

## Build provenance

- Last commit touching `mashedmod/src`: `46bf1453` 2026-10-01T03:02:49-03:00.
- Pre-existing `mashedmod\build\mashed_re.exe` was stamped 2026-10-01 02:59:24, i.e.
  **older** than that commit, so freshness could not be established. Rebuilt once via
  `mashedmod\build.bat` (exit 0, `=== Build OK ===`), new stamp 2026-10-01 11:02:19.
- Exe under test is pinned at `verify/freecam_title_20261001/bin/mashed_re.exe`,
  SHA-256 `9FF13F2797C95281107DCE737739EE6E9B48926B61570289F721F490D986E5F9`, 1,970,688 bytes.
- Two commits after `a9da810a` touch `exe_main.cpp` (`c9615225`, `e40a4ce0`); neither
  reintroduces the removed code (verified by grep on HEAD, see T5/T6 static legs).

## Measurement channels and their known limits (declared up front)

**Title read (IN USE).** `GetWindowTextW` on the HWND resolved from the PID *this harness
spawned* (`EnumWindows` + `GetWindowThreadProcessId`). Never by window name. This channel is
validated by T1: a title that carries a string only this run set (`MASHED_TITLE`) proves the
harness is reading the right window.

**Key delivery (NOT AVAILABLE — declared before running).** Every keyboard read in the
standalone goes through DirectInput `IDirectInputDevice8A::GetDeviceState`
(`exe_main.cpp:1023`, device created `:987`, cooperative level
`DISCL_BACKGROUND|DISCL_NONEXCLUSIVE` `:1000`). `PostMessage`/`SendMessage` of `WM_KEYDOWN`
does **not** write the DirectInput device-state buffer, so posted keys cannot reach `g_keys[]`
at all. The live human read is additionally gated on `g_active` (window focus) at
`:2884-2885` and `:1844`. Therefore:

- A "posted W/A/S/D/Q/E/R produced no camera movement" result is a **null-input-channel
  GREEN** and is NOT evidence. It would come out identical on a build where the free camera
  was still present. It is recorded as a sanity run only, never as the verdict.
- Driving the removed free-fly inputs for real needs physical keys or global `SendInput`,
  both of which mean taking focus / leaking keys into other sessions' games. Per the task
  rules that leg is reported **UNMEASURABLE**, not worked around.

**Output channel for camera pose.** The standalone has no per-frame camera pose log
(`MASHED_CAM_POSE` is an *input* that pins the camera, `TrackRenderer.cpp:5146`). The
available output is the backbuffer (`MASHED_DBG_BBDUMP` + `MASHED_DBG_BBDUMP_OUT`). Before
any pose comparison is believed, the output channel is validated by a positive control: a run
with `MASHED_CAM_POSE` set to a different pose must produce a DIFFERENT bmp than the
unperturbed run. If that control does not differ, every pose comparison in T5 is reported
UNMEASURABLE rather than GREEN.

## Checks

### T1 — title with MASHED_TITLE set
Env: `MASHED_MUTE=1 MASHED_WIN_POS=left-bl MASHED_TITLE=T1 freecam title verify`.
PASS iff the title read from the spawned PID's HWND equals exactly
`Mashed RE | T1 freecam title verify | menu`.
Source: label = `MASHED_TITLE` when set (`exe_main.cpp:1638-1639`); format
`"Mashed RE | %s | %s"` (`:1678`); state `menu` for `GameMode::Frontend` (`:1666`).
Note `MASHED_WIN_POS` must NOT appear in the label when `MASHED_TITLE` is set.

### T2 — title without MASHED_TITLE, other MASHED_* set
Env: `MASHED_MUTE=1 MASHED_WIN_POS=left-bl MASHED_SIM_HZ=60` (no `MASHED_TITLE`).
PASS iff the label (field 2 of the title) lists the `MASHED_*` variables with the
`MASHED_` prefix stripped, as `NAME=value`, space-separated, and contains
`WIN_POS=left-bl` and `SIM_HZ=60` while containing neither `MUTE` nor `TITLE`
(`:1642-1661`, skips at `:1645-1646`, prefix dropped at `:1647`).
The parent shell has **no** ambient `MASHED_*` variables (checked: empty), so the label is
exactly the set this run passes.

### T3 — title with no MASHED_* vars
Env: `MASHED_MUTE=1` only (MUTE is excluded from the label, and an unmuted run is
forbidden, so this is the minimum legal environment). No `MASHED_WIN_POS`, so the window
lands at its default position; accepted for this check.
PASS iff the title equals exactly `Mashed RE | manual play | menu` (`:1661`).

### T4 — state field follows GameFlow
`GameMode` is `{Frontend, LoadingRace, InRace, Results}` (`Race/GameFlow.h:14`), mapped to
`menu` / `loading race` / `race` / `results`, with `race (paused)` when
`GameFlow_IsPaused()` (`exe_main.cpp:1664-1672`).
PASS iff, polling the title through a race run
(`MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_RESULT_DEMO=1`), the observed state sequence is a
subsequence of `menu -> loading race -> race -> results` with no out-of-order state.
`race (paused)` is **pre-declared unreachable** in this harness: the only writer of the pause
flag is an Esc rising edge read from DirectInput under focus, and it is explicitly skipped
whenever `g_race_demo`/`g_det_clock` is set (`:1869-1878`). It is reported UNMEASURABLE.
Frame index: a separate run with `MASHED_DETERMINISTIC=1` must append ` | frame N` with N a
multiple of 30 (`:1674-1676`), and N must increase between two reads.

### T5 — the free camera is gone
Primary leg (static, decides the verdict): on the pinned exe's source tree at HEAD,
`ci.dt = dt` (`:2887`) must be the ONLY assignment to any `CamInput` field in
`exe_main.cpp`; no `move_fwd`/`move_strafe`/`move_up`/`yaw_delta`/`pitch_delta`/`reset_orbit`
write and no `VK_RBUTTON` read may remain. Combined with the consumer gate
`wants_free = move_fwd!=0 || move_strafe!=0 || move_up!=0 || yaw_delta!=0 || pitch_delta!=0`
(`TrackRenderer.cpp:5176-5177`) and `free_` only ever set inside `wants_free`
(`:5179-5187`), an all-zero `CamInput` makes `free_` unreachable, so `chase_cam = car_ready_`
(`:5497`) and the race camera drives the view.
PASS iff all of that holds AND the pinned exe renders a race whose backbuffer matches a
repeat run (camera on a stable race-camera path), with the output channel validated by the
`MASHED_CAM_POSE` positive control above.
Posted-key sanity run is recorded but, per the channel declaration, cannot pass or fail T5.

### T6 — arrows still drive the car; B11/B12 gone
(a) Arrow->car mapping intact: `di.accel`/`di.steer` from `DIK_UP/DOWN/RIGHT/LEFT`
(`:2936-2938`) must still be present, and `git show a9da810a` must show those lines as
unchanged context (the commit removed only the two `ci.yaw_delta/pitch_delta = 0.f` lines
that followed them).
(b) The car-drive path actually moves the car: a run with a scripted drive injection
(`MASHED_PLAY_DEMO=1`) must change the player's position/speed versus a no-input control run
of the same length. This validates `DriveInput` end-to-end through a channel that *is*
available (env var). It does not by itself prove the arrow-key mapping; (a) covers that, and
real arrow-key delivery is UNMEASURABLE for the same DirectInput reason as T5.
(c) B11 arrow-key title-quad nudge and B12 held-key title removed: `g_titleQuadOffsetX/Y`,
`kTitleQuadStep`, `UpdateTitleFromKeyboard`, `UpdateQuadFromKeyboard` absent from HEAD source,
AND the old title strings absent from the pinned exe while the new ones are present.

## Process hygiene

Every launch records its PID; only those PIDs are killed (`Stop-Process -Id`). No kill by
name. Every run sets `MASHED_MUTE=1`. `MASHED_NAV_DEMO` is never used.
