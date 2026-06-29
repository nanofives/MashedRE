# WS-A — Vehicle coupling law (FUN_0047eb30): the chain→rigid-body bridge

Recovered 2026-06-29 (Ghidra pool0, read-only). Anchored to
`original/MASHED.exe.unpatched` SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.

This is the **once-per-tick bridge** that maps the A3–A6 vehicle force chain's
per-car state onto the RW-Physics rigid body, and reads the integrated body
transform back into the render struct. It is the function the standalone needs
to replace the degenerate `kWorldVel`/`kYawScale` world-coupling gains with.

`FUN_0047eb30` (`0x0047eb30..0x0047f1db`, ~1700 b) — called once per tick by the
dispatcher `FUN_00470c70` (per the C1 comment in the decomp: "called from
FUN_00470c70 unconditionally after FUN_00467350"). Strict NO-GUESSING: every
constant below is read from `.rdata` at the cited RVA.

## The per-car arrays (one struct, stride 0x341 ints = 0xd04 bytes)

The render struct and the physics record are the **same 0xd04 record**:
`DAT_00881ec8` (render-matrix double-buffer block) = `DAT_008815a0` (record base)
+ `0x928`. So the per-car loop in the bridge (`local_8c = &DAT_00881f68`,
stride `0x341`, until `< 0x885378`) walks the same records the chain integrates.

| field (abs) | record off | meaning | accessor |
|---|---|---|---|
| `DAT_008815b4` | +0x14 | per-car "live" gate (process iff `==0`) | bridge `local_8c[-0x26d]` |
| `DAT_00881ec8` | +0x928 | render-matrix block (16 ints/buffer, ×2 double-buffer) | `FUN_0046d4a0` returns active matrix ptr |
| `DAT_00881f48` | +0x80 | active render-buffer index (0/1) | — |
| `DAT_00881f50/f54/f58` | +0x88/8c/90 | stored offset3D vec3 | `FUN_0046cb30` (when body not live) |
| `DAT_00881f74` | +0xac/b0/b4 | velocity vec3 (transformed by 0x00614708) | `FUN_0046d510` |
| `DAT_008816d8` | +0x138 | COM-offset vec3 (body-local) | readback loop |
| record +0x9b0 | +0x9b0 | **chain linear velocity** (A6a writes; kVelocity) | — |
| record +0x9bc/9c0/9c4 | +0x9bc | **chain angular velocity** (A6a; +0x9c0 = yaw) | — |

The **RW-Physics proxy body** for car `i` is fetched via
`FUN_0057c210((&DAT_006c9a78)[i])` → `iVar6`; the body handle is
`piVar2 = *(int**)(*(int*)(iVar6+0x10)+4)`, and the body's transform matrix is
`puVar11 = *(int**)(*(int*)(iVar6+0x10)+8)` (matrix words `[0xc..0xe]` = body
world position). The body's linear-velocity slot is
`pfVar8 = *(int*)(*(int*)(*piVar2+0x10)+8) + piVar2[1]*0x20` (vec3 at `[0..2]`),
and its angular-velocity slot is the **same base + 0x10** (`[0..2]`).

## The accessors (all RVA-cited, decompiled this session)

- **`FUN_0046d4a0(&m, i)`** (`0x0046d4a0`, 45 b) — `m = &DAT_00881ec8 +
  activeBuf*0x10 + i*0x341`. Returns a pointer to the active render matrix.
  `m[0xc..0xe]` = render world position (`renderPos`).
- **`FUN_0046cb30(&v, i)`** (`0x0046cb30`, GetOffset3D) — if record `+0x14==0`:
  `v = stored vec +0x88/8c/90`. Else `v = FUN_0047f1e0(&v,i) · _DAT_005cd8fc`
  where `FUN_0047f1e0` (`0x0047f1e0`) reads the **body's current linear velocity**
  vec3 (same `piVar1[1]*0x20 + …` slot). So when the body is live,
  **offset3D = bodyLinVel · 300.0**.
- **`FUN_0046d510(&v, i)`** (`0x0046d510`, 83 b) — `FUN_004c3df0` transforms record
  velocity `+0xac/b0/b4` by matrix `&DAT_00614708` in place, copies into `v`.
  → world-frame velocity for heading.
- **`FUN_0046d4d0(i, src)`** (`0x0046d4d0`) — copies 16 ints `src` → BOTH render
  buffers (`DAT_00881ec8[i]` and `DAT_00881f08[i]`). This is the **readback**:
  body integrated transform → render matrix.
- **`FUN_004233e0(x, z)`** (`0x004233e0`) — atan2 returning **DEGREES** in [0,360)
  (uses 180.0/360.0/`fpatan·(180/π)`).
- **`FUN_004a3384(x)`** (`0x004a3384`) — MSVC CRT **acos** (radians).

## The coupling law (verbatim, per car per tick)

Forward half (writes body lin + ang velocity), when `piVar2 != 0`:

```
offset3D   = bodyLinVel · 300.0                       # _DAT_005cd8fc @0x005cd8fc = 300.0
springTgt  = offset3D · 0.0002 + renderPos            # _DAT_005cf014 @0x005cf014 = 0.0002
                                                       #   (== bodyLinVel · 0.06 + renderPos)
comWorld   = transform(record+0x138 COM-offset, bodyMatrix)
bodyLinVel = (springTgt − (comWorld + bodyPos)) · 20.0   # _DAT_005ccd6c @0x005ccd6c = 20.0  (PD gain)

# angular: align body heading to its velocity heading (degrees)
velHeading = atan2_deg(velWorld.x, velWorld.z)        # FUN_0046d510 → FUN_004233e0
bodyHeading= atan2_deg(bodyFwd.x,  bodyFwd.z)         # transform &DAT_00614708 by bodyMatrix
hd  = (bodyHeading − velHeading) + 180.0              # _DAT_005cd09c @0x005cd09c = 180.0
while (hd < 0)  hd += 360.0                            # _DAT_005ccac4 @0x005ccac4 = 360.0
if (hd > 180.0) hd = −(360.0 − hd)
hd  = (hd <= 0) ? −(hd + 180.0) : (180.0 − hd)         # wrap to ±180
bodyAngVel = (0, hd, 0)                                # y = heading error in DEGREES
# + cross-correction toward velocity (only if axisVel cross ≠ 0 and dot < 1):
axis = transform(&DAT_006146fc, bodyMatrix)            # an up/right axis
c    = cross(axis, velWorld);  d = dot(axis, velWorld)
d    = clamp(d, −1.0, 1.0)                             # _DAT_005cc33c=−1, _DAT_005cc320=+1
k    = acos(d) · 57.2958                               # _DAT_005cc98c @0x005cc98c = 180/π
bodyAngVel = (c.x·k,  c.y·k + hd,  c.z·k)
# then FUN_004c45f0/FUN_004c0e50/FUN_0055dff0/FUN_0055ac00 commit to the RW-Physics body
```

Off-track / mode 3|5 branches: when `FUN_0040e350()∈{3,5}` the body matrix is
overwritten from the render matrix (teleport); when the car index ≥ player count
the body's `+0x34` is set to `0xc61c4000` = **−10000.0** (drop it out of play).

Readback half (after the vendor solver integrates), per car where `+0x1c != 0`:
```
comWorld   = transform(−COM-offset · 1.0, bodyMatrix)  # _DAT_005cc33c = −1
renderPos  = bodyMatrixPos − comWorld                  # via FUN_0046d4d0 → both buffers
```

## Recovered constants (all read from .rdata this session)

| symbol | RVA | hex | value | role |
|---|---|---|---|---|
| `_DAT_005cd8fc` | 0x005cd8fc | 0x43960000 | **300.0** | offset3D = bodyLinVel · this |
| `_DAT_005cf014` | 0x005cf014 | 0x3951b717 | **0.0002** | springTgt lookahead (× offset3D) |
| `_DAT_005ccd6c` | 0x005ccd6c | 0x41a00000 | **20.0** | PD gain (body linVel = err · this) |
| `_DAT_005cc98c` | 0x005cc98c | 0x42652ee1 | **57.2958 (180/π)** | acos rad → deg (cross-correction) |
| `_DAT_005cd09c` | 0x005cd09c | 0x43340000 | **180.0** | heading wrap half-circle (deg) |
| `_DAT_005ccac4` | 0x005ccac4 | 0x43b40000 | **360.0** | heading wrap full-circle (deg) |
| `_DAT_005cc320` | 0x005cc320 | 0x3f800000 | **1.0** | dot clamp hi |
| `_DAT_005cc33c` | 0x005cc33c | 0xbf800000 | **−1.0** | dot clamp lo / negate |
| `DAT_005d757c` | 0x005d757c | 0x00000000 | **0.0** | zero literal |
| (airborne) | — | 0xc61c4000 | **−10000.0** | off-play body Y drop |

Net lookahead: `offset3D·_DAT_005cf014 = bodyLinVel · 300 · 0.0002 = bodyLinVel · 0.06`.

## What this means for the standalone (single-body reduction)

The law is a **two-body closed loop**: the A3–A6 chain integrates the render
matrix position (`renderPos` — the real accel ramp / top speed live in the chain's
`+0x9b0`), a stiff spring (gain 20, lookahead 0.06 s) drags a collision proxy body
to it, the vendor solver integrates the proxy (collision), and the readback resets
`renderPos = bodyPos`. The readback is what closes the loop and keeps it stable.

**A single body cannot transcribe the open-loop spring.** Simulated
(`anchor += chainVel·dt; tgt = anchor + v·0.06; v = (tgt−p)·20; p += v·dt`) the
verbatim form **diverges** within ~1 s (the `+v·0.06` lookahead self-amplifies
velocity ~1.2×/tick with no readback to cancel the `20·(anchor−p)` term). So the
faithful single-body realization is the **reduction**: the car body follows the
chain's integrated velocity through a critically-stable **PD relaxation using the
recovered gain 20.0** (inertia + smooth ramp), the recovered **0.06 s velocity
lookahead**, and the recovered **heading-align-to-velocity** angular law — with a
**soft** top-speed asymptote replacing the degenerate hard `kSpeedCap=45` clamp
(the ported chain's `+0x9b0` ramps unbounded going straight — a separate chain-drag
limitation; the hard clamp is what produced the "instant pin to 45"). The single
remaining calibration is the chain-internal → world unit scale (the original had
none because proxy + render share RW world units; the standalone's ported chain
self-limits in internal units). This is documented in `VehiclePhysicsRun.cpp`.

## Port (standalone) — what landed

`Vehicle/VehiclePhysicsRun.cpp` `StepCar` (behind `MASHED_REAL_PHYSICS`) +
`D3d9Render/TrackRenderer.cpp` `UpdateCar` (player + AI). Replaced:
- the hard `kSpeedCap=45` internal clamp -> a high anti-overflow safety clamp (1500)
  + a recovered **speed model**: the body forward speed `g_bodySpeed[slot]` relaxes
  toward `chainHorizSpeed*kChainScale` (clamped to a world top) at the recovered PD
  gain **20.0/s**. The chain's `+0x9b0` ramps over ~2 s -> the world speed ramps.
- the `kWorldVel` position gain -> `pos += {cos,0,sin(yaw)}*io.drive_speed*dt`.
- the `kYawScale * frameMs` yaw integration (which **spun at low fps** because the
  per-frame delta scales with `frameMs`) -> the recovered **heading-align**: the body
  faces its velocity heading `atan2(cvz,cvx)` via an fps-independent approach
  `1-exp(-kAlignRate*dt)`. Bounded (|err|<=pi) -> framerate-stable, no spin.

Calibrations (env-tunable; the unit/scale the original gets free from shared RW world
units): `MASHED_CHAINSCALE` (0.0083, internal->world), `MASHED_TOPSPEED` (12 world
u/s, ~ old 45*0.22≈10), `MASHED_ALIGNRATE` (7/s). The PD gain (20), the lookahead
intent, and the face-velocity angular law are the **recovered** constants/structure.

## Verification (2026-06-29; MASHED_MUTE=1, MASHED_REAL_PHYSICS=1)

Ran `mashed_re.exe` play-demo (full throttle + scripted steer ramp) ~24 s per track.
Telemetry: `mashed_re.log` PLAY-DEMO (pos+speed every 0.25 s).

- **Arctic** (standard): world speed **ramps 0 -> ~12 u/s over ~3 s** (pos-delta:
  1.0, 3.1, 6.2, 11.1, 12.0 u/s), heading rock-steady going straight, turns follow the
  steer with **no spin**, car covers the whole track (z 17->59->-57) — stable, no
  wedge/fly-off. (OLD path pinned to internal 45 instantly -> ~10 u/s, no ramp.)
- **training** (sloped desert): same clean ramp (0 -> ~39 units in 4.6 s straight,
  yaw -1.576 dead steady), smooth bounded turns, covers a wide area, slopes handled,
  no wedge/fly-off. Car renders + drives on-track (chase cam).
- **Egypt** (narrow): same real ramp + drives ~7 s, but the scripted HARD steer drives
  it off the road edge; in plain track-view (`gates_` empty unless `MASHED_ROUND`) the
  pre-existing off-track recovery can't pull it back -> freezes off-mesh. With
  `MASHED_ROUND=1` (gates active) the recovery redirects + the round respawns it
  (stable, no permanent wedge). Off-track-shim gap, not the coupling (GroundHeight
  handles slopes — see training).

Feel: genuine accel ramp + a stable, bounded heading that follows velocity (weightier
than the old instant-pin gain-slide). No crash on any run.
