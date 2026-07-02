# WS-C — opponent-AI spline lookahead (FUN_00443dc0 / FUN_00443300)

Goal: make standalone opponents drive the racing line without stalling. Root cause
(measured 2026-06-30, `MASHED_LAP_DIAG`): real-AI + real-physics already produces
believable pack racing for ~15-20 gates, then individual cars **stall mid-lap** —
they cut corners off-mesh because the standalone replaced the AI's real spline
lookahead with a crude 1-raw-point target (`kLookahead=1`), so the verbatim
`ControlStep` bands full-lock the steer at every bend.

The real lookahead is `FUN_004161e0` → `FUN_00443dc0(spline, &ownXZ, outXZ, v, 1, 0)`.
All facts below are Ghidra MCP reads on slot Mashed_pool0 (read-only, 2026-06-30),
anchored to MASHED.exe SHA-256 BDCAE093…3C0E. NO-GUESSING.

## FUN_00443300 — Catmull-Rom cubic (XZ), verbatim
P0..P3 = pts[idx-1 .. idx+2] (all wrapped mod count = `*(spline+0x200)`); points are
float2 at `spline + i*8`. Param `t` clamped to [0,1]. Per-axis:
```
P(t) = ( 2*P1 + (P2-P0)*t
       + (((P1*3 - P0) - P2*3) + P3) * t^2 * t        // t^3
       + ((P2*4 + (2*P0 - P1*5)) - P3) * t^2 ) * 0.5  // t^2, scale
```
Constants (memory_read): 0x005cc31c=**3.0**, 0x005cc358=**5.0**, 0x005cc35c=**4.0**,
0x005cc32c=**0.5**, 0x005cc320=**1.0**, DAT_005d757c=**0.0**. (Standard Catmull-Rom.)

## FUN_00443dc0 — lookahead target finder (called with param_5=1, param_6=0)
param_1=spline base, param_2=own XZ ptr, param_3=OUT target XZ, param_4=vehicle v,
param_5=1 (index continuity on), param_6=0 (debug-draw OFF → all `if (param_6!=0)`
blocks, incl. the `FUN_004671a0`/`FUN_004b55a0` render calls, are skipped).

1. **Nearest point** — min ||pts[i]-own||² over all `count` points → `nearest`.
   (also caches each dist into a local[64] buffer, later reused as the point buffer.)
2. **Per-vehicle index continuity** — `prev = DAT_008032d4[v*5]` (= addr 0x008032d4 +
   v*0x14, the +0x00 field of the 0x14-stride per-car array). Reject a jump >1
   (`diff = nearest-prev`; if `diff != 1-count && (diff<-1 || 1<diff) && prev<count`
   keep `prev`), then store `nearest` back. Prevents teleporting around the loop.
3. **Sub-segment select** — Catmull(nearest, t=0.01) vs Catmull(nearest-1, t=0.99);
   keep whichever is closer to own (sets the working segment + point).
4. **Ternary closest-param search (16 iters)** — bracket [lo=0, hi=1]; each iter, if
   the hi-end point is closer move `lo=(2*lo+hi)*k` else `hi=(2*hi+lo)*k`, k=0x005ce034
   =**0.3333**. Yields the closest param + point on the segment.
5. **(param_6 only)** debug wall-tile draw — SKIPPED.
6. **Forward walk (16 points)** — from the closest param, step param by 0x005cc9a0=
   **0.05** each iter (wrap >1.0 → advance segment). At each, Catmull→point; score =
   dot( normalize(point-prev), normalize(prev-own) ); if score<0 → score=-param; clamp
   ≤1.0. Store the 16 points + 16 scores. (normalize via FUN_004c3c60; length via
   FUN_004c3bf0 — both fast-LUT, reproduced with std::sqrt in the standalone shim.)
7. **Pick** — target = the lookahead point with the **max score** (argmax, strict-<, so
   first max wins; init 0 → if all scores ≤0 the target is the first forward point).
8. **LOS wall-march (anti-corner-cut)** — march own→target in steps of 0x005cc564=
   **0.25**, quantize each marched XZ to the tile grid: cell = `(ROUND(x*4)+0x1f0>>3)*0x80
   + (ROUND(z*4)+0x1f0>>3)`, `tileId = (int16)DAT_007f1a9c[cell]`; if `0<tileId<0x200`,
   read sub-cell `DAT_007f9a9c[(tileId*8 + ((z*4 ROUND)-0x10 & 7))*8 + ((x*4 ROUND)-0x10
   & 7)]`; flag 0 or 3 = blocked. On block, step the target back 2 lookahead points and
   retry; `FUN_00416230(v, atClosest)` writes the per-car +0x00 flag at 0x0089a500+v*0x74.
   The tile grid **and** sub-cells ARE loaded by `AiData_LoadInto(0x007f1a9c)` (Ai/AiData.h:
   kAiTileGridOff=0x0 → 0x007f1a9c 128×128 int16; kAiSubCellOff=0x8000 → 0x007f9a9c
   0x200×8×8 char), so this is portable. [DECOMP NOTE: the 2nd `FUN_004a2c48`=ROUND in
   each march loop lost its explicit `*4.0` multiply in Ghidra; by symmetry it is
   ROUND(marchedZ*4.0). Verify against the listing before porting Phase 8.]

## Port plan (staged)
- **Stage 1 (DONE):** Phases 1-7 in Ai/AiStandalone.cpp (`CatmullRom`, `Normalize2`,
  `SplineLookahead`), replacing the [G4] `s_prog` 1-point crutch in `ControlStep`.
- **Stage 2 (DONE):** Phase 8 LOS, reimplemented behaviorally as a `Ai::Host::los_clear`
  callback backed by the track collision (`GroundHeight` march) instead of the original's
  tile grid — far more robust than porting the decompiler-mangled grid-index spaghetti,
  and it steps the target back exactly as the original (`iVar8 -= 2`).

## OUTCOME (measured 2026-06-30) — partial; blocked downstream on physics fidelity
With the faithful lookahead + LOS, opponents (real-AI + real-physics) drive believable
**pack racing for ~15-20 gates** off the line, then individual cars **brake-latch frozen**
mid-lap (traced via periodic `MASHED_AI_DIAG`): e.g. v1 drives the start straight + first
corner fine (speed ~1000), then stops dead at a fixed XZ with **constant yaw** and
`ctrl[255,0,255,255]` (accel+brake+full-steer) and never recovers.

Root cause is NOT the lookahead and NOT corner-cutting (LOS confirmed the target stays
on-mesh there): it is the verbatim "mildly off" band (`FUN_00416250`, err 30..180°) setting
**accel AND brake together** (trail-braking). In the original physics chain this scrubs
speed while the car keeps rolling; the standalone **WS-A8 chain lets the brake net the car
to speed 0**, and a stopped car has no steering authority (`yaw += steerAngle * speed`), so
err stays in-band → permanent deadlock. Confirmed adapter-side: dropping the brake when
nearly stopped did NOT restart the car — the WS-A8 chain won't accelerate from a dead stop
under full-steer either. So **faithful AI needs faithful low-speed accel/trail-brake
physics**, which is WS-A8 C4 work, a separate workstream.

Decision fork for "opponents that complete laps" (stop-and-ask):
- **(A) Faithful all the way** — promote the WS-A8 chain's accel+brake+low-speed response to
  C4 so the faithful bands work as the original intended. Highest fidelity, biggest cost.
- **(B) Faithful navigation + robust actuation** — keep this faithful spline lookahead as the
  AI's target (the real racing line), but drive opponents with the proven gate-ribbon-style
  motion model (steer-toward-target turn-rate + velocity-shaped speed, no accel+brake
  deadlock) instead of the verbatim descriptor bands. Delivers visibly-real racing now;
  the gate-ribbon already laps cleanly, this just upgrades its target from gates to the
  real spline. Medium cost, no physics-fidelity dependency.
- **(C) Defer** — keep the gate-ribbon default; leave faithful-bands-on-real-physics behind
  `MASHED_REAL_AI`+`MASHED_REAL_PHYSICS` until WS-A8 is C4.

This change (faithful `SplineLookahead` + `los_clear`) is the necessary navigation half and
is committed regardless of which fork is chosen — it is the real target source for both (A)
and (B).

## RESOLUTION — Option B chosen + DONE (2026-06-30): faithful nav + robust motion
User picked (B). Opponents now drive the FAITHFUL racing-line target with a robust motion
model, default once the `.AI` banks load. Pieces (TrackRenderer.cpp / AiStandalone.cpp):
- `Ai::Ai_ComputeTarget(v, ownX, ownZ)` exposes the faithful lookahead (SelectSpline + the
  ported `FUN_00443dc0`) as the AI's "where to go".
- The opponent drive (`faithful_nav` branch in `UpdateCar`) replaces the verbatim
  descriptor-band / physics-chain path: steer toward the target (turn-rate) + velocity-shaped
  speed FLOORED at 45% cruise (no accel+brake brake-latch deadlock). `.AI` loads by default
  (`MASHED_GATE_RIBBON_AI=1` reverts to the gate ribbon).
- Robustness layers added after measuring real failure modes (`MASHED_LAP_DIAG` / `MASHED_AI_DIAG`):
  1. **Self-healing index continuity** — reseed the per-vehicle progress index (`DAT_008032d4`)
     when the car is >6 segment-lengths from the stored point (covers mid-track spawn /
     respawn / knock-off; the original seeds it at spline placement, the standalone didn't).
  2. **Forward-progress safety net** — where the racing line passes near itself (start/finish),
     the geometric-nearest seed can land on the wrong leg and the target points BACKWARD;
     if the target opposes the gate-ribbon race direction (>~105°), retarget the next gate.
  3. **Ring-probe off-mesh recovery** (mirrors the player's `RecoverOffMesh`) — on an off-mesh
     step, find an on-mesh heading near the target and COMMIT a nudge, jumping to the verified
     ring point if the small nudge lands in a gap (escapes thin mesh islands).
  4. **Anti-stuck marshal recovery** — if a car's gate progress stalls >2.5s (a lookahead
     corner-case the racing line skirts), relocate to the next gate ahead on the ribbon,
     facing forward, at cruise. Robust by construction (every trigger advances +2 gates).
- AI cars spawn as a small starting grid just ahead of the start line on the forward-seeding
  side, facing the race direction (was: scattered at gates 2/4/6, which seeded one car onto
  the spline's return leg).

**Verified (Arctic, no-round, 120 s, all-AI-driven):** all 3 opponents race the full racing
line and complete laps — a1 lap 3, a2 lap 3, a3 lap 4 — smooth monotonic progress, no freezes
(was: 0-1 of 3 before). Round mode renders the 4-car standings + elimination scoring
(verify/ai/_ai_race_optionB.png). Player feel is UNCHANGED — the player still uses its own motion
path; only opponents changed. The fully-faithful path (Option A: verbatim bands on a C4 physics
chain) remains future work.
