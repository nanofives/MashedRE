# WS-PHYS-SMOKE — standalone real-physics runtime smoke (2026-06-17)

Ran `mashedmod/build/mashed_re.exe` from repo root (main @ 7d1536c8 + this branch's
parent), race-demo into Arctic (MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1 MASHED_GOTO=6
MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0), 30s bounded wait + force-kill.

## Result: real physics CRASHES; scaffold is the clean baseline.

| Run | MASHED_REAL_PHYSICS | Outcome |
|-----|---------------------|---------|
| A (ON)  | 1 | **AV crash** exit `0xC0000005` at ~t=2.2–3.0s |
| B (OFF) | unset | **clean** exit code 0; car drives + steers |

### B (scaffold) — baseline GOOD (mashed_re.log PLAY-DEMO):
```
t=1.0 yaw=1.550 pos=(-25.2,15.8) speed=0.00
t=2.1 yaw=1.550 pos=(-25.2,15.8) speed=0.00
t=3.2 yaw=1.597 pos=(-25.2,16.2) speed=2.15
t=4.2 yaw=2.317 pos=(-27.2,22.4) speed=9.29
t=5.3 yaw=3.672 pos=(-38.3,25.6) speed=13.44
t=6.3 yaw=5.297 pos=(-43.6,13.3) speed=14.93
```
(The demo's scripted accel ramps in at ~t=3.2 — speed=0 at t=1–2 is normal/pre-accel.)

### A (real physics) — CRASH before the accel phase:
```
R4 track load OK: Arctic.piz tris=16480 verts=16229 sectors=12 mats=13 gates=94
PLAY-DEMO t=1.1 human_drive=1 car_yaw=1.550 pos=(-25.2,15.8) speed=0.00
PLAY-DEMO t=2.2 human_drive=1 car_yaw=1.550 pos=(-25.2,15.8) speed=0.00
<process AV — no further frames>
```
Crash dump `mashed_re.exe.40152.dmp` (scripts/parse_minidump.py):
`code=0xC0000005 eip=0x0005ad26 (READ addr=0x2000) eax=0 ecx=0x00b53f88; 86 modules;
MASHED.exe not loaded (correct — standalone); no AcLayers/apphelp shims.`

## Reading
- The race path + track load + demo all work (scaffold proves it). The crash is
  **specific to the MASHED_REAL_PHYSICS chain** and fires at ~t=2.2–3s, BEFORE the
  demo applies throttle — so the dynamics quality could NOT be assessed; the crash is
  the blocker. `MASHED_REAL_PHYSICS` correctly stays default-OFF.
- `eip=0x0005ad26` is a GARBAGE execution address (far below the image base) + a
  near-null read (0x2000): signature of a **call through a bad/uninitialized function
  pointer or a smashed return address** in the wired chain — NOT a plain field
  deref. Most likely culprits to check next: a stubbed dep invoked as a function
  pointer, `g_vehicleArrayBase` / the 0xd04 record array binding, or the
  `Collision::g_worldTris` contact-soup pointer set by VehiclePhysics_SetWorld.

## Next (feeds WS-A-VERIFY-3 / an A8-fix session, MAIN tree)
1. Reproduce under a debugger / poll_attach_catch_crash.py to get the real call site
   (the dump has no stack walk); resolve eip=0x5ad26's caller.
2. Audit the A8 wiring pointers: record-array base, contact soup, the substep loop's
   per-substep global rebind, and any reimpl dep still pointing at an inert/zero stub.
3. Only after it runs crash-free, assess drive/brake/suspension/steer quality.

---

## WS-PHYS-CRASH-FIX — RESOLVED (2026-06-17, branch ws-phys-crash-fix)

### Caller chain (captured live via re/frida/catch_standalone_phys_crash.py)
The WS-PHYS-SMOKE dump's "eip=0x0005ad26 garbage address" reading was WRONG: it
assumed image base 0x400000, but mashed_re.exe is `/BASE:0x10000`. Resolved against
mashed_re.map (Rva+Base) the fatal fault is a DATA read (NOT a bad fn-ptr):

```
eip rva 0x4ad26  _FastInvSqrt+0x36   (instr: add esi,[eax+edx*4]; eax=0, edx=0x800 -> READ @0x2000)
  <- _RwMatrixRotate+0x67
  <- ?Rw_MatrixFromAxisAngle@Vehicle  (the A5/A6a stub binding -> Math/RwMatrixRotate)
  <- ?VehicleWheelForceIntegrate@Vehicle+0xf3   (A5, Phase 0: steered-wheel matrix)
  <- ?VehicleControlIntegrate@Vehicle+0x302     (A4)
  <- ?VehiclePhysics_StepPlayer@Vehicle+0x1c3
  <- ?UpdateCar@TrackRenderer+0x417
```
(NOTE: the harness must let the boot chain's SEH-recovered `MainLoopInit` execute-
into-zero AVs at 0x495110 pass — return false on `op=='execute'` — and stop only on a
DATA fault, else it catches the benign boot AV. Both ON and OFF raise the boot AV.)

### Root cause
`Math/RwSqrt.cpp::FastInvSqrt` (and the whole RW two-level fast-sqrt LUT family:
FastSqrt, Vec3Magnitude 0x4c3ac0, RwV3dNormalize 0x4c39b0, RwV2d 0x4c3bf0/3c60) reads
its LUT root via `*(*0x7d3ff8 + *0x7d3ffc + delta)`. That LUT is built by
**RwEngineOpen, which never runs in the standalone** (no RW device). At runtime
0x7d3ffc stays 0 but 0x7d3ff8 is overwritten with a NON-zero garbage heap value
(~0xb54f88) during boot, so a "both selectors zero" guard is insufficient: the path
proceeds, derefs the garbage slot -> a ZERO table pointer -> `lut_root[0x800]` reads
[0+0x800*4]=[0x2000] -> 0xC0000005. Physics-ON hits it because the chain calls these
RW-math leaves with live (null) LUT pointers; the scaffold (OFF) uses std::* and never
touches them.

### Fix
All five RW-LUT functions now resolve + VALIDATE the actual LUT root pointer
(`RwSqrtLutRoot`/`Vec3LutRoot`/`lut_root`: require the slot and the whole 0x1000-entry
table to lie inside the image 0x10000..0xb40000). When invalid (standalone, no engine)
they fall back to a plain CPU `std::sqrt` / `1/sqrt` — the same several-ULP standalone
substitute already used for the RW *device* transform (RwV3dTransformPointsCPU). In the
dev .asi the LUT IS live, so the original bit-identical LUT path is taken unchanged (the
C4 leaves are preserved — guard is a no-op there).

### Result: race SURVIVES (Y). Motion: residual zero (handoff to SMOKE-2/VERIFY-3).
- catch_standalone_phys_crash.py: **0 data-access faults in 30s** (was AV @ t~2.2s).
- 12s PLAY-DEMO race exits code 0; full 6.6s of PLAY-DEMO frames, no AV.
- BUT speed stays ~0 (probe_phys_state.py once the race starts): wtc=3047, tec=256
  (terrain soup + batch live), A4 drive force `+0x1a8=16.93` computed, A6a accumulates
  `+0xb14=-18.19` — so the chain RUNS and produces force. Velocity stays ~0 because:
    (a) `+0x9e0` (grounded-int) = 0, never the 0x40800000 (=4.0-as-int) all-grounded
        sentinel A6a/A5's suspension-velocity blocks gate on;
    (b) `+0x278` grounded-count reads 16.93 (= the drive force), i.e. it is being
        ALIASED/overwritten — a field-mapping correctness bug, not a crash;
    (c) the linear-integration `linTerm = dt*mass(+0x54=0.001)*kDt` is vanishingly
        small with the current A3 mass init.
  These are physics-CORRECTNESS issues (grounded-state machine + field map + tuning),
  explicitly the WS-PHYS-SMOKE-2 / WS-A-VERIFY-3 gate (diff-original needed), NOT the
  crash. The crash blocker is cleared; the chain is now safe to iterate on.

Repro tooling added: re/frida/catch_standalone_phys_crash.py (+_off.py),
re/frida/probe_rwsqrt_lut.py, re/frida/probe_phys_state.py.
