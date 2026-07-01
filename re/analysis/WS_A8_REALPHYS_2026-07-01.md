# WS-A8 — real-physics standalone: crash FIXED, dynamics root-caused (2026-07-01)

Re-validation of `MASHED_REAL_PHYSICS` now that the physics chain is fully C4
(A3/A4/A5/A6a/A6b — see CHANGELOG 2026-07-01). Anchored MASHED.exe SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`. Standalone
`mashedmod/build/mashed_re.exe` rebuilt this session. Branch `ws-a8-real-physics`.

## 1. Crash blocker RESOLVED ✅

The 2026-06-17 smoke (`PHYS_SMOKE_2026-06-17.md`) recorded a hard AV
(`0xC0000005`, `eip=0x0005ad26`, READ `addr=0x2000`) ~t=2.2 s with
`MASHED_REAL_PHYSICS=1`, which is why the gate stayed default-OFF.

Re-run against the current chain (`re/frida/catch_standalone_phys_crash.py`,
same env `MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1 MASHED_GOTO=6
MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0`): **0 fatal exceptions over the full 30 s
window; the race runs to a normal round-end and returns to menu.** The AV is gone —
resolved by the physics-chain fixes landed since 06-17 (A6a float10 shims + A5/A6b
work). `log/standalone_phys_crash.json` = `{fatal: null}`.

## 2. Dynamics NOT yet faithful — root-caused (the real remaining WS-A8 work) ❌

With real physics on, the demo car (`log`/`mashed_re.log`) drives straight and pins
to top speed almost instantly, and **steering does not turn the car** (`car_yaw`
frozen at 1.5498 through `steer=+0.50`). Coupling diagnostic (`MASHED_COUPLING_DIAG=1`
→ `coupling_diag.log`, 224 rows) pins the cause:

```
cv=(31.56, 0.00, 1499.67) horiz=1500.00 fwdDot=1500.00 desired=12.000 bs=12.000 yaw=1.5498 velH=1.5498
```

- **Forward speed is unbounded.** The chain's force-integrated velocity `+0x9b0`
  ramps with no top-speed damping and saturates the `kSafetyInternal = 1500` clamp
  (`VehiclePhysicsRun.cpp:480`) within ~1.5 s. In the original this is bounded by the
  2-body RW-Physics proxy coupling (`FUN_0047eb30`, PD gain `_DAT_005ccd6c=20.0` +
  0.06 s velocity lookahead — `vehicle_coupling.md`); the standalone's open-loop
  single-body *reduction* loses that damping.
- **Steering yields negligible lateral velocity.** Lateral `cvx` maxes at ~31.6 vs
  forward `cvz` ~1499.7, so `velHeading = atan2(cvz,cvx) ≈ yaw` and the "body faces
  its velocity" turn law (`VehiclePhysicsRun.cpp:582-589`) sees `err ≈ 0` → no yaw
  change. The steer byte IS delivered (`input[0]=127` for `steer=+0.5`,
  `VehiclePhysicsRun.cpp:398-406`) and A4 writes the front-wheel steer angle, but the
  single-body reduction doesn't convert it into enough heading curvature.

**This is a physics-INTEGRATION problem, not a C4-logic problem.** Each per-frame
function is verified bit-identical against the original (C4); the gap is the
standalone's single-body reduction of the original's 2-body proxy→render coupling
loop (`vehicle_coupling.md` already documents the open-loop divergence). Fixing it
faithfully is open-ended calibration/RE (reproduce the 2-body loop, or find a bounded
reduction that both damps forward speed AND preserves steer→lateral→heading).

## Disposition

`MASHED_REAL_PHYSICS` **stays default-OFF** — but the reason has narrowed from "crashes"
to "single-body coupling reduction doesn't yet reproduce turning + top-speed damping."
The scaffold kinematic model (physics OFF) remains the shipping default and drives +
steers correctly. Next-step options are a fork for the user (see session summary):
grind the coupling calibration, or shelve real-physics (crash-fixed, root-caused) and
advance the loop to another roadmap item (R1 C4 re-validation / R2 menu).
Evidence: `log/standalone_phys_crash.json`, `coupling_diag.log`, `mashed_re.log`.
