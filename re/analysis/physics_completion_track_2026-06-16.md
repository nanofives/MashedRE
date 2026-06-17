# Physics-completion track — status + turn-key port spec (2026-06-16)

Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
(Ghidra pool11, read-only). This is the path from "A5 verified in isolation" to a
real physics-driven car in **mashed_re.exe** (`UpdateCar` still runs the scaffold —
see [[project-ports-done-convergence-pending]]).

## Per-frame chain (confirmed this session)

`FUN_00470c70`(dispatcher, 16 cars) → per active car `FUN_00470670`(**A4**) →
{ `FUN_0046ddb0` (**A5**) , `FUN_00467650` (**A6a**) , `FUN_00468980` (**A6b**) } →
parked-state velocity damp. Spawn-time: `FUN_0046b540` (**A3**) populates the 0xd04
record. RW-math leaves (`FUN_004c3ac0/39b0/4d20/3df0`) are **A2, ported + C4**.

## Status

| Piece | RVA | State |
|---|---|---|
| A2 RW math | 4c3ac0/39b0/4d20/3df0 | **PORTED + C4** (Math/) |
| A5 force integrator | 0x0046ddb0 | **PORTED** (Vehicle/ForceIntegrator.cpp), verified in isolation; deps stubbed |
| **A4 control integrator** | 0x00470670 | **PORTED THIS SESSION** (Vehicle/VehicleControl.cpp) — see below |
| A6a vel/ang integration | 0x00467650 | **NOT ported** — exact decomp captured; ~280-line x87 float10 body |
| A6b aero stabilization | 0x00468980 | **NOT ported** — small (~40 lines); calls FUN_004a3384 (asin), FUN_004c4d20 |
| A3 init/spawn | 0x0046b540 | **NOT ported** — decomp in re/analysis/bucket_vehicle_00453f30_00482030/0046b540.md + vehicle.md §4.1 |
| A7 constant harvest | — | partial (A4's 11 done; A6a/A6b's ~40 pending) |
| bind A5 deps → Math/ | — | pending (ForceIntegratorStubs.cpp: Rw_* are identity, globals = 0) |
| A8 wire UpdateCar | — | pending (also add Vehicle/*.cpp to the **exe** source list — exe list omits them today) |

## A4 (DONE) — what landed

`Vehicle/VehicleControl.cpp` :: `VehicleControlIntegrate(int* self, float dt,
uint8_t* input, void* xform)` is the verbatim FUN_00470670 control half: slide
measure (+0xb0c), input filter (+0xb24/+0xb28), drive/reverse torque to
+0x1a8/+0x26c + the 16-slot rings (+0x1ac/+0x270), parked damp (state +0x9f0==2).
Constants memory_read + named in `vc::`:
kInputScale 1/256 (005ceaa8), kAirSpeed 64 (005cd6d4), kBoostMul .75 (005cc950),
kInput5Thr 128 (005cc9d0), kFilterClamp 6000 (005ceaa4), kGripMul ~1/6000
(005cea58), kElseMul 1.5 (005cc348), kParkedDamp .9 (005cc9c8), kHalf .5 (005cc32c),
kOne 1 (005cc320), kZero 0 (005d757c).
Three callees + DAT_007f101c are inert stubs in ForceIntegratorStubs.cpp (link only).

## Remaining work (next session(s))

1. **A6a `FUN_00467650`** — the big one. Verbatim port (Vehicle/ → Vehicle_Integrate2).
   Reads ~30 `_DAT_005ce../005cc..` consts (HARVEST each: 005ce1e8, 005cea40, 005cea3c,
   005cea34/30/38/2c, 005cc55c/328, 005cc9b4, 005cd694, 005cc564, 005cea1c, 005cc990,
   005cea14/18, 005ccac4, 005cc9f4, 005cd03c, 005cea10, 005cea0c, 005cd120, 005ce18c,
   005ce264, 005cc31c, 005cea08, 005ccabc, 005cea04, 005cea00, 005cd0ac, 005ccd04,
   005cc948, 005cc56c, 005ce9fc/f8/f4/f0, 005cc750, 0088e5f0/610, 0088e668/66c). Uses
   `float10` (x87 80-bit) intermediates — **match the original's x87 evaluation** (see
   the RwMatrixRotateInner.cpp precedent: plain C++ is 1-ULP off; inline __asm where
   needed). Per-wheel loop stride 0x31 ints (piVar12 += 0x31), 4 wheels. Branches keyed
   on track-id literals at +0x1f0 (e.g. -0x69e1a6, -0xe17f4c, -0x5f7f80…). Callees:
   FUN_004c3ac0 (|v|, C4), FUN_004c39b0 (norm, C4), FUN_004c4d20/004c3df0 (C4),
   FUN_004a2c48 (input smoother), FUN_0040e350/0040e340 (game mode).
2. **A6b `FUN_00468980`** — aero stabilization. Small. Callees FUN_004a3384 (asin/acos —
   decompile + port), FUN_004c4d20, FUN_004c39b0. Reads 006146fc/00614700/00614704 (up/
   fwd axes), 005cc33c/320/558, 005ccae0/ad8.
3. **A3 `FUN_0046b540`** — init. Fixed wheel geometry consts + track-type override table
   @ 0x00613140 (5-int stride, -1 term; key = FUN_0040ce80(FUN_00430790())). Writes mass
   1000 (+0x50), inertia (+0x54/+0x58), wheel blocks, contact double-buffer (+0x9a4/+0x9a8).
   Standalone needs the track-type/mode getters fed from the selected area (not DFF).
4. **Bind A5 deps** — replace ForceIntegratorStubs identity Rw_* with Math/ calls; set
   g_grav*/g_susp*/g_torqueRingPhase from real values (gravity table; FUN_00470c70 inits
   _DAT_0088e610 = dt*_DAT_005cea80, _DAT_0088e5f0 = _DAT_005ccd08/_DAT_0088e610).
5. **A8 wire** — add Vehicle/{VehicleControl,ForceIntegrator,...}.cpp + the A6/A3 files to
   the **exe** source list (build.bat); call A3 at car spawn, A4 per frame from UpdateCar
   over the 16-car array; map the standalone car state ↔ the 0xd04 record fields; supply
   the WS-B contact arrays (B4 wheel solver). Then diff-original a matched-input race (C4).
   **Callee arg binding** (resolve here): A4's decomp calls `FUN_0046ddb0(param_2,iVar1,
   param_4)` with the vehicle implicit in EAX — confirm self/dt/xform mapping vs the A5
   port signature during the diff.

## Reality

This is the project's single largest subsystem; A6a alone is a 280-line x87-sensitive
port that must be diff-original-verified. Realistically 2–3 more focused sessions
(A6a; A6b+A3; bind+A8+verify). A4 + the chain map + the constant plan are the start.
