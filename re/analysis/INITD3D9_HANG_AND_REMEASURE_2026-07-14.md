# InitD3D9 CreateDevice hang + WS-PHYS-DRIVE re-measure (2026-07-14)

Session: M2 opener WS-PHYS-DRIVE-STABILIZE. Goal was to re-measure the current
standalone drive symptom before choosing the D1 fork (Option A reconstruct vs
Option B calibrate). The re-measure was first blocked by a D3D9 boot hang.

## 1. InitD3D9 CreateDevice hang (environment blocker — FIXED)

**Symptom.** `mashed_re.exe` hung at boot, log frozen at
`InitD3D9: adapter mode 2560x1440 fmt=22 refresh=165`. The per-line flush
(`fopen`/`fclose` each line, exe_main.cpp) means the missing
`CreateDevice … -> hr=` line proves `IDirect3D9::CreateDevice(HAL, …)` **blocked
and never returned** — not a failure, a hang. Reproduced 3x (60s, 110s, and with
`MASHED_FORCE_SWVP=1` — so it was HAL device creation itself, not the vertex-
processing flag / the Jun-29 HW-first reorder `5f76c892`).

**Environment.** NVIDIA RTX 5070 Ti, primary display 2560x1440@164, driver
`32.0.15.9186` dated 2026-01-19 (i.e. *older* than the 2026-07-06 runs that
booted fine — so not a driver update). Likely a transient GPU/D3D9-state wedge
(a prior force-killed hung instance) and/or a driver-thread interaction.

**Fix (exe_main.cpp `InitD3D9`).** A plain retry loop cannot recover from a hung
synchronous call, so each `CreateDevice` now runs on a **detached worker thread**
guarded by a timeout (`CreateDeviceWatched`):
- rc 1 = created, 0 = clean failure, -1 = hung (worker abandoned; its
  `shared_ptr` result keeps itself alive so the leaked thread never touches freed
  memory; reclaimed at process exit).
- On the first HANG we stop hammering HAL and jump straight to a **REF** software
  device (different driver path). If everything hangs we fail fast (`return
  false`) so the app exits cleanly instead of hanging forever. `g_d3d` is only
  released when no worker is still outstanding (else use-after-free).
- New env knobs: `MASHED_D3D_TIMEOUT_MS` (default 6000), `MASHED_D3D_REF`
  (REF first), `MASHED_D3D_NOVSYNC` (PRESENT_INTERVAL_IMMEDIATE). `MASHED_FORCE_SWVP`
  preserved.

**Result.** Rebuilt; boot now succeeds — log shows
`InitD3D9: CreateDevice(HAL/HW,depthA,vsdef) -> OK hr=0x00000000` and the game
reaches InRace with live telemetry. (This run HAL/HW returned OK immediately on
the worker thread; the watchdog remains as protection against a recurrence.)

## 2. Re-measure — current real-physics drive symptom (CONFIRMS 2026-07-06)

Env: `MASHED_RACE_DEMO=1 GOTO=6 DRIVE_HOLD=1 PLAY_DEMO=1 TRACK_SEL=0 CAR_SEL=0
MASHED_REAL_PHYSICS=1`. Evidence: `log/drivehold_realphys_20260714.log`.

```
td 0.00–2.89  speed=0.00  pos=(-25.2,15.8)  yaw=1.5498   (race-start countdown: throttle gated)
td 3.16       speed=41.81  pos=(-25.2,15.8)  yaw=1.5498   (throttle onset)
td 3.42       speed=136.19 pos=(-25.2,16.0)  yaw=1.5498   (ramping toward the 1500 clamp; race ends)
```

- `car_speed()` (record `+0x9e4`, the internal velocity) explodes 0→41.81→136…
  toward the 1500 safety clamp on throttle onset; position advances only ~0.2 u
  in 0.26s (the "metric divergence": internal velocity explodes, visible motion
  via `io.drive_speed` stays soft-capped).
- Race ends ~0.3s after throttle onset (rule engine reacts to the runaway),
  which is why steering can't be exercised in this window (yaw flat 1.5498).
- **Identical to `WSPHYS_DRIVEHOLD_2026-07-06.md`** (0→41.81→618→1500 clamp,
  steering dead). Drive-path source is unchanged since (VehiclePhysicsRun.cpp
  last edited 2026-06-29; zero drive-path commits since), so this is expected.
- The prompt's `0/260/0` symptom is the *scaffold* pattern, already corrected
  (race-round resets) per the 07-06 note — NOT the real-physics symptom.

## 3. Steer channel — RVA-confirmed (Ghidra pool0, read-only)

Resolves the survey's byte-map conflict:
- **A4 `FUN_00470670`** reads descriptor **`*param_3`=[0]** and **`param_3[1]`=[1]**
  as the steer command → front-wheel steer angle `+0x1a8`/`+0x26c` ([0]=+angle,
  [1]=−angle via `fVar3 = -fVar3`). `param_3[5]` is a brake threshold.
- **Human cook `FUN_00496530`** writes bytes [0]–[3] as four polled
  `FUN_00497310` inputs (+ active flags [0x0c]–[0x0f], analog floats +0x14/+0x18),
  with a conditional [0]↔[1] swap under `DAT_007f0f30`.
- ⇒ the standalone port (steer→`input[0]/[1]`, VehiclePhysicsRun.cpp:404-405)
  **matches what A4 reads**. "Dead steering" is a chain/coupling issue, not a
  channel bug. The original-side injector `scenario_launch.py` writes steer→[2]/[3]
  — a *different* channel — so it must be fixed to [0]/[1] before any original-side
  steer A/B.

## Disposition

The re-measure confirms the divergence is the two-body coupling gap (internal
velocity has no proxy-body feedback to damp it). This is the D1 fork the repo
resolved as **Option A** (reconstruct the RW-Physics proxy body = lane B5a..B5e).
Awaiting the user's scope decision: Option A (start B5a) vs an Option-B single-body
stopgap to unblock WS-A8/WS-C faster.
