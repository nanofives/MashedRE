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
