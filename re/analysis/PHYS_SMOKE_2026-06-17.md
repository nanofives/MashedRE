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

---

## WS-PHYS-MOTION — RESOLVED: the car MOVES (2026-06-17, branch ws-a-verify3-motion)

### Verdict: Y — real physics drives. Speed 0 -> ~112; grounded reaches 4.0; pos trajectory live.
MASHED_REAL_PHYSICS=1 race (MASHED_RACE_DEMO/PLAY_DEMO/GOTO=6, Arctic, PLAY-DEMO
= full accel+steer). Per-frame record diag (compile with -DMASHED_PHYS_DIAG, writes
phys_diag.log; CL='/DMASHED_PHYS_DIAG' build):
```
sp=34.9  vel=(0.73,0.00,34.90)   pos=(-25.2,0.0,15.8)  fwd=(0.02,0.00,1.00)
sp=80.7  vel=(-60.85,0.00,-33.80) pos=(-28.6,0.0,27.1)  fwd=(-0.69,0.00,-0.72)
sp=92.6  vel=(83.66,0.00,14.95)   pos=(-29.3,0.0,11.1)  fwd=(0.44,0.00,0.90)
sp=112.6 vel=(-106.87,-0.00,0.83) pos=(-38.6,-4.1,1.5)  fwd=(-0.91,0.00,-0.41)
```
Speed ramps with throttle; velocity tracks the heading; position wanders (the car
circles because PLAY-DEMO holds steer=1). No crash; clean exit. grounded count
+0x9e0 reaches 0x40800000 (=4.0); susp scale +0x9e5f0 ~8600-17000 (non-zero).

### Root cause: it needed MORE than the input byte-map fix — a zeroed world xform.
The recovered WIP (973d20ab) correctly wired input[4]=accel/[5]=brake to A6a and a
SetGrounded() hint (wheel-state +0x198/.. = 1, grounded count +0x9e0 = 4.0). But the
diag with that alone showed `b14=(0,0,0) drvDir=(0,0,0) fwd=(0,0,0) sp=0` — the drive
force accumulator stayed zero EVEN THOUGH input + grounded + committed-mode (+0x168=2)
were all correct.

The chain (verified vs original FUN_00470670/FUN_0046ddb0/FUN_00467650, Ghidra pool11):
- A6a (FUN_00467650) drive block accumulates `+0xb14 += piVar12[0x1f..0x21] * drive`,
  where piVar12[0x1f..0x21] (byte +0x220) is the per-wheel DRIVE DIRECTION.
- That drive direction is written by A5 (FUN_0046ddb0) Phase 0: when wheel steer==0,
  `piVar12[0x2d..0x2f] (== same byte +0x220) = self forward (+0x9d4)`, and the forward
  itself is `FUN_004c3df0(self+0x9d4, &DAT_00614708 /*(0,0,1)*/, 1, xform)` — i.e.
  forward = xform * (0,0,1).
- **The standalone port passed the WRONG xform.** VehicleControl.cpp passed `wheelBlock`
  (the +0x928 wheel-matrix ring slot) as A5's xform; the original passes A4's `param_4`
  (the world xform) — `FUN_0046ddb0(param_2/*dt*/, iVar1/*wheelBlock*/, param_4/*xform*/)`
  at 0x004708... The +0x928 block is NEVER initialized standalone (RW scene-graph
  populates it; no RW device runs), so it is a ZERO matrix -> forward = (0,0,0) ->
  drive direction = (0,0,0) -> b14 = 0 -> no force -> no motion.

### Fix (this branch)
- VehicleControl.cpp: pass A4's `xform` param (not `wheelBlock`) to A5, matching the
  original arg order.
- VehiclePhysicsRun.cpp: synthesize the world xform each frame from the car's yaw
  (BuildYawMatrix -> RwMatrix with at=(cos,0,sin)=forward, up=(0,1,0), right=(sin,0,-cos)),
  pass it down the chain. (The original's xform is the live RW vehicle matrix; the
  standalone has no RW device, so the yaw matrix is the faithful substitute — only the
  forward + wheel right-axis transforms read it.) [UNCERTAIN] the original xform SOURCE
  (A4 param_4 traces to the dispatcher's param_2 which vehicle.md calls `&dt`) is not
  fully resolved; the yaw-matrix substitute makes the standalone correct regardless.

So motion needed: (a) the input byte-map fix [4]/[5] (WIP), (b) the SetGrounded hint
(WIP), AND (c) the world-xform fix (this session). All three are required.

NOTE the diag block in VehiclePhysicsRun.cpp is gated behind -DMASHED_PHYS_DIAG (inert
in the shipped build) and writes phys_diag.log (cwd) — kept for the WS-A-VERIFY-3
telemetry lane.
