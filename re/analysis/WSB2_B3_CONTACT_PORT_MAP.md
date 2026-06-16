# WS-B2/B3 — car↔world + car↔car contact port map (2026-06-16)

Anchored to MASHED.exe SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`
(Ghidra pool3, read-only, session 2026-06-16). Every RVA / constant / offset
below was verified by MCP this session. Implements the **ratified Option B** of
`COLLISION_GATE_BRIEF.md`: port the used contact subset as first-party code, no
qhull, no 0x55-band.

Port lives in `mashedmod/src/mashed_re/Collision/`:
- `CarWorldContacts.cpp` — B2 (`0x0046c5f0 0x00468b40 0x0046cc40 0x00468d80 0x004694e0 0x00469aa0`)
- `CarCarContacts.cpp` — B3 (`0x00469df0`)
- `ContactConstants.h` — every `_DAT_005c*`/`_DAT_0061*` tuning scalar (cited)
- `ContactDeps.h` / `ContactStubs.cpp` — leaf vec math + residual-dep stubs
- `ContactSolvers.h` — public entry points (for B4 wiring)

## Per-frame call tree (verified this session)

```
FUN_00470c70  (16-car dispatcher, stride 0x341 ints)
  └ FUN_004709a0  (per-substep contact entry; up to 2 substeps/car; gated game-mode)
       ├ FUN_0046e9e0  (WS-A integration step — not B2/B3)
       ├ FUN_0046f6c0  (wheel solver) ──┐ runs FIRST: builds the terrain batch
       │     ├ FUN_00538c80 (RW broadphase walk of the collision world)  ← PRODUCER
       │     │     callback &LAB_00468b80, user &DAT_00828320; resets DAT_0088e60c=0
       │     │     then appends triangle entries → the batch the solvers iterate
       │     ├ FUN_004c3d90 (RW vtable contact query → DAT_0088e600 / DAT_0088e620)
       │     ├ FUN_0046cc40 (WheelTerrainContactClassifier)   ← B2
       │     └ FUN_0046c5f0 (TriangleFaceNormal)              ← B2
       ├ FUN_00469aa0  (history shift + dispatch + scan)      ← B2
       │     ├ FUN_004c3d90 (×2 contact queries)
       │     ├ FUN_00468d80 (VehicleTerrainContactSolver)     ← B2
       │     │     └ FUN_00468b40 (ContactHistoryLookup)      ← B2
       │     └ FUN_004694e0 (VehicleObjectContactSolver)      ← B2
       │           └ FUN_00485370/360/420 (dynamic-object list)
       ├ FUN_0046ef70  (WS-A integration step — not B2/B3)
       └ FUN_00469df0  (VehicleCarCarContact)                 ← B3
             gate: game-mode ∈ {6,7,10,0xb} AND centroid distance²
             < ((rA+rB)*_DAT_005cc950)²   [_DAT_005cc950 = 0.75 @0x005cc950]

  separately, FUN_00470670 (control-input) → FUN_0046ddb0 (force integrator)
       READS the per-car contact arrays the above solvers fill. ← WS-A CONSUMER
```

`FUN_004709a0` callers: `FUN_00470c70` only. `FUN_0046ddb0` callers: `FUN_00470670` only.

## Headline: the car↔world geometry producer is first-party, not qhull

The COLLISION_GATE_BRIEF B1 ruling holds and is sharpened here: the terrain
triangle batch the solvers iterate (`DAT_0088e60c` count) is produced **per
substep** by `FUN_00538c80` (an RW broadphase walk of the collision world) +
callback `LAB_00468b80` — invoked inside the wheel solver `FUN_0046f6c0`
(`0x0046f7fb`-region), which sets `DAT_0088e654 = in_EAX` (the world object) and
resets `DAT_0088e60c = 0` first. **No qhull, no 0x55-band on this path.** The
collision triangles come from the track collision world (COLLI*.BSP, parsed in
R3). `FUN_004c3d90` is a *separate* RW indirect-vtable contact query that fills
the rigid-body contact normal/velocity scratch (`DAT_0088e600`/`DAT_0088e620`),
NOT the triangle batch.

This means B4 wiring must give the standalone an equivalent broadphase that fills
the 0x90-byte batch layout (below) from its already-loaded COLLI*.BSP. That
producer (`FUN_00538c80` + `LAB_00468b80`) is the residual `Rw_BroadphaseWalk`
stub — the only genuinely new engine glue B2 needs beyond the RW math already
ported under `Math/`.

## Contact-batch entry layout (0x90 bytes / 0x24 floats)

Iterated by `FUN_00468d80` (from `param_2+8`, i.e. float index 2) and
`FUN_0046cc40` (from `param_2`). Float-index semantics from the solver reads:

| float idx | meaning |
|---|---|
| `[0..2]` | triangle vertex A |
| `[3..5]` | triangle vertex B |
| `[6..8]` | triangle vertex C |
| `[9..0xb]` | unit face normal |
| `[0xc..0xd]` | per-contact surface metadata (material id? — [UNCERTAIN U-3630]) |
| `[0xd]` | sentinel: `-1.7147562e+38` (=0xff010101, first-frame) / `-1.7014636e+38` (=0xff0000ff, persistent) — gate the debug class only |
| `[0x19..0x21]` | three SAT half-plane edge normals (as read by `FUN_00468d80`) |
| `[0x1b..0x23]` | three SAT half-plane edge normals (as read by `FUN_0046cc40`) |
| `+0x34 (byte)` | contact key matched by the history scan `FUN_00468b40` |

Per-tick scratch globals (`.bss`, filled by the broadphase callback):
`DAT_0088e5e0` per-wheel "already classified" int[4]; `DAT_0088e600` contact
query scratch; `DAT_0088e624` 4×vec3 transformed wheel positions; `DAT_0088e650`
active contact count (≤4); `DAT_0088e60c` terrain entry count.

## Vehicle record offsets the solvers touch (int-word indices from base DAT_008815a0+car*0xd04)

| index | byte | role |
|---|---|---|
| `[0]` | 0x000 | car id (object-solver vehicle-id filter `entry[0xe]==*v`) |
| `[4]` | 0x010 | flag gating car-car spin path (`vehX[4]==0`) |
| `[0x14]` | 0x050 | mass (object impulse `entry[0x13..]=mass*vel`) |
| `[0x51..0x53]` | 0x144..0x14c | angular velocity (car-car `*600` ; wheel friction accum) |
| `[0x57..0x59]` | 0x15c..0x164 | centre-of-mass (radius-arm origin) |
| `[0x65..0x66]` | 0x194..0x198 | per-wheel block base (stride 0x31), wheel-active flag |
| `[0x130]` | 0x4c0 | per-slot contact records (stride 0x10, 18 slots) |
| `[0x133]` | 0x4cc | 3 two-frame history manifolds (stride 0x90, FUN_00469aa0) |
| `[0x12b]` (299) | 0x4ac | 18-slot contact scan array (stride 0x10) |
| `[0x26a]` | 0x9a8 | active geometry slot index (car-car centroid slot) |
| `[0x26b]` | 0x9ac | wheel-ring/substep matrix-block selector |
| `[0x26c..0x26e]` | 0x9b0..0x9b8 | linear velocity (the impulse target) |
| `[0x26f..0x271]` | 0x9bc..0x9c4 | rotation-axis field (terrain arm rotate; car-car spin-stop) |
| `[0x272..0x274]` | 0x9c8..0x9d0 | face-axis / approach-velocity vector |
| `[0x27b]` | 0x9ec | active-contact-count writeback (FUN_00469aa0) |
| `[0x27c..0x27d]` | 0x9f0..0x9f4 | state flags gating debug-event class |
| `[0x27e..0x280]` | 0x9f8..0xa00 | contact-point origin array |
| `[0x28a..0x295]` | 0xa28..0xa54 | 4-vertex bounding hull (car-car SAT) |
| `[699]` | 0xaf0 | bounce/impact timer (set 0x1e0=480 on car-car hit) |
| `+0xbfc` | — | 32-slot contact-history table (key[i], active flag[i+0x20]) |

## Tuning constants (raw value @ address — all read this session)

`1.0 @5cc320` · `0.5 @5cc32c` · `0.01 @5cc328` · `-1.0 @5cc33c` · `0.7 @5cc340`
· `-2.0 @5cc34c` · `10.0 @5cc55c` · `0.25 @5cc564` · `100.0 @5cc568` · `2.0
@5cc574` · `3.6 @5cc754 (SandSpWheel arm)` · `2^32 @5cc94c` · `0.75 @5cc950` ·
`57.29578 @5cc98c (180/π)` · `9.98e-6 @5cc990` · `0.3 @5cc99c` · `1000.0 @5cc9fc`
· `-10.0 @5cca40` · `360.0 @5ccac4` · `9.99e-5 @5cd03c` · `-0.1 @5cd0fc` · `2.5
@5cd088` · `-0.4 @5cd30c` · `-0.5 @5cd50c` · `-20.0 @5cd61c` · `0.02 @5ce18c` ·
`200.0 @5ce194` · `100000.0 @5ce23c` · `0.0005 @5ce268` · `0.2778 @5cea44` ·
`0.42 @5cea48` · `1.999e-7 @5cea4c` · `600.0 @5cea50` · `-0.25 @5cea5c` · `0.2778
@5cea60` · `-0.0625 @5cea84` · `0.99899 @5cea88` · `0.1(double) @5cea90` ·
`-0.1(double) @5cea98` · `-0.005 @5ceaa0` · `0.0 @5d757c` · up=(0,1,0)
@6146fc/00614700/00614704. Full table with bit patterns in `ContactConstants.h`.

## Verification status — IMPORTANT correction to the gate

The COLLISION_GATE_BRIEF says "each session ending in a `diff-original`." That
is **not achievable for these functions via the bit-identity (path1) lane**: the
contact solvers are `__thiscall`/implicit-`this` (ESI/EDI/EAX) state-readers over
the 0xd04 vehicle record — the same class as RaceCamera/scoring
(`SESSION_VERIFICATION_AUDIT_2026-06-16.md`). They are **not isolatable leaves**,
so `run_diff.py` cannot bit-diff them. The only `run_diff`-able piece is the leaf
`FUN_0046c5f0` (TriangleFaceNormal; non-standard EAX+ESI fastcall — needs a
harness `arg_type` extension before it can run).

Real gate for B2/B3 = **installed-hook scenario telemetry** (`RH_ScopedInstall`
the original RVAs in the dev `.asi` + observe a canonical race with the inline-JMP
live — WS-H2 style), compared against the **original-side baseline trace** this
workstream captures. That baseline (the per-car contact arrays + terrain batch at
a known spot) is the verification anchor; see `re/frida/wsb_contact_baseline.py`.

C-level disposition (honest, no overclaim per [[feedback-no-overclaiming-c-levels]]):
- All 7 ported functions = **C2 faithful transcription** (compiles + links into
  `mashed_re.exe`; matches the decompiler line-by-line with cited offsets/consts).
  NOT promoted — no behavioral evidence yet.
- C3/C4 path = installed-hook scenario diff vs the baseline, once the WS-A
  consumer (`FUN_0046ddb0`) drives them (B4 depends on A5).

## Captured original-side baseline (2026-06-16) — the verification anchor

`re/frida/wsb_contact_baseline.py` (timer-polled one-shot snapshot, **no** sustained
Interceptor) spawned the original and, when MASHED auto-entered its attract/demo
race (runtime game-mode = **6**, ∈ the {6,7,10,0xb} race set) at ~t=25s, captured
an 8-frame time series → `log/wsb_contact_baseline.json`. Ground truth:

| metric | observed |
|---|---|
| game-mode (FUN_0040e350) | 6 (race) — confirms the contact path is live |
| `DAT_0088e60c` terrain entry count | 2–7 / frame (broadphase batch size as cars move) |
| `DAT_0088e650` tick active contacts | 4 every frame |
| per-car grounded count (`+0x9e0`) | 4.0 (all wheels) for all 3 active cars |
| per-car speed (`+0x9e4`) | ~2800–3550 |
| 18-slot contact scan (`+0x4ac`) | empty during smooth cruising (deep-penetration/ |
|  | impact slots populate only on hits — none in the demo window) |

So in a real race the broadphase yields a small terrain batch (2–7 tris/query),
all four wheels register contact (`tickActiveContacts=4`, `grounded=4`), and the
solvers continuously correct the linear velocity (`+0x9b0`). When B4 wires the
port and replays the demo, the standalone must reproduce these (grounded=4,
tickActiveContacts≈4, terrain batch 2–7, velocity in range) — that is the
installed-hook scenario diff. This also empirically validates that the contact
producer is the first-party broadphase (no qhull): qhull never ran and the batch
was populated each tick.

## Tracker disposition (honest — no promotion)

No C-level moved this session (no behavioral evidence for the *standalone* port):
- `0046c5f0` C2, `0046cc40` C2, `00468d80` C2, `004694e0` C2, `00469aa0` C2,
  `00469df0` C2 — faithful transcriptions now exist under `Collision/`; stay C2.
- `00468b40` is **already C3** as an `.asi` hook (`Vehicle/VehicleState.cpp`, A/B
  7/7 GREEN); the Collision copy is the self-contained standalone twin.
- `0046c5f0` C3 path: it is a genuine leaf and `run_diff`-able once the harness
  gains the non-standard EAX(apex)+ESI(out) `arg_type` — the one cheap real-C3
  datapoint available from this set without a full scenario run.

## Residual dependencies (stubbed in ContactStubs.cpp; bound in WS-B4 / WS-A)

| stub | real RVA | bind to |
|---|---|---|
| `Rw_TransformPoints` | `0x004c3df0` | `Math/RwV3dTransformPoints.cpp` |
| `Rw_VtableDispatch` | `0x004c3d90` | RW rigid-body contact query |
| `Rw_MatrixFromAxisAngle` | `0x004c4d20` | `Math/RwMatrixRotate.cpp` |
| `Rw_MatrixDerive` | `0x004c4dc0` | RW matrix derive |
| `Rw_SetRotation` | `0x004c52f0` | RW set-rotation |
| `Math_Acos` | `0x004a3384` | acos approximation |
| `Sys_TickCount` | `0x004a2c48` | tick counter |
| `Game_Mode` | `0x0040e350` | game-mode getter (already ported elsewhere) |
| `Rw_BroadphaseWalk` | `0x00538c80` + `LAB_00468b80` | COLLI*.BSP broadphase → batch |
| `Obj_ListBase/Count/ReadWorldPos` | `0x00485370/360/420` | dynamic-object list |
| `g_playerCount` | `DAT_00803320` | live player count |

Leaf vec math (`Vec3Normalize`/`Vec3Mag`/`FastSqrt`) is implemented locally;
bit-identity residual vs the original `FUN_004c3b30` FastSqrt is recorded for the
eventual installed-hook diff ([[project-wsa2-rwmath-bitident]]).

## Fidelity notes / known residuals

- Dev-only **debug-event ring** enqueues (`DAT_007e9de0..DAT_007ec9e4`, type
  3/4/6, gated by game-mode ∉ {1..5}) are **gameplay-inert** and omitted; RVA
  cited at each site. The control flow that gates a *contact write* is preserved.
- x87 `float10` intermediates → `double` (width-preserving; not bit-identical to
  the original 80-bit FPU path — a recorded residual).
- The decompiler renders float stores into `int*`-typed velocity fields as
  `field = (int)(floatexpr)`; these are ported as **float stores** (the fields are
  floats) — NOT numeric truncation.
- Car-car SAT (`FUN_00469df0`) verbatim quirk: only hull vertices 1 and 2
  increment the per-pass contact counter `local_b0[]`; vertices 3 and 4 increment
  only the global `iVar15`. Preserved verbatim.

## WS-B4 producer half — DONE + verified (2026-06-16)

The car↔world geometry producer is fully RE'd and ported. `FUN_00538c80` is an RW
collision-query dispatcher (switch on shape type `param_2[6]`; the wheel solver
passes type 3 → `FUN_00539ec0`, which walks the world and fires the callback per
triangle). The callback **`LAB_00468b80`** (0x00468b80..0x00468d7c — Ghidra never
made it a function; reached only via the callback ptr) is the per-triangle
collector. Disassembled this session:

- Batch base = the 5th arg (the query `userData`) = **`DAT_00828320`** (resolved).
- Entry stride 0x90 (`count*9<<4`); `DAT_0088e60c++` per entry.
- Fill (float idx): `[0..2]`=v0 `[3..5]`=v1 `[6..8]`=v2 (from collTriangle vert ptrs
  +0x1c/+0x20/+0x24); `[9..0xb]`=face normal (collTriangle[0..2]); `[0xc]`=material;
  `[0xd]` (byte 0x34)=surface key (the history-match key); `[0xf]`=100000.0; `[0x10..0x12]`=0;
  `[0x1b..0x1d]`=N×(v1−v0), `[0x1e..0x20]`=N×(v2−v1), `[0x21..0x23]`=N×(v0−v2)
  (the 3 SAT half-plane edge normals; sat0.x = N.y·e0.z − N.z·e0.y confirmed @0x468c8f).

Ported as `Collision/ContactProducer.cpp` (`FillBatchEntry` verbatim + a broadphase
loop over the standalone's COLLI*.BSP triangles). Plus a wheel-pos correction: the
transformed-wheel-pos array bases at **`DAT_0088e620`** (the wheel solver writes 4
vec3 there via `FUN_004c3d90(&DAT_0088e620,…,4,…)`); the classifier's `local_8c`
starts at `+1` float (`DAT_0088e624`) and reads `[-1]/[0]/[1]` = wheel x/y/z. (The
prior `DAT_0088e624` label was off by one float; fixed in the port.)

**Verified** by `Collision/contact_selftest.cpp` (builds standalone, runs): 4 wheels
just above one ground triangle, inside its XZ projection, approach into the surface
→ producer emits 1 batch entry, classifier registers **4 active contacts**, latches
each wheel's face normal `(0,1,0)` + depth `0.1`, sets all 4 skip flags → **PASS**.
This reproduces the captured baseline's signature (4 active wheel contacts on flat
ground) and is the first behavioral evidence the contact-source path is correct —
WITHOUT the WS-A consumer or vehicle-struct init. (The real-track baseline *diff*
still awaits WS-A struct population, since the live classifier needs real
transformed wheel positions in `DAT_0088e620`.)

## Next (B4, depends on A5)

Wire B2/B3 as the contact source for the ported vehicle sim (replace the
ground-raycast scaffold in `TrackRenderer::UpdateCar`): bind the residual stubs to
the real RW math + a COLLI*.BSP broadphase that fills the batch, drive the solvers
from the WS-A `FUN_0046ddb0` consumer, then diff vs the captured original
telemetry. B4 is blocked on WS-A's consumer port (A5).
