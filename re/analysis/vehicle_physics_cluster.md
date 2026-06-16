# Vehicle physics cluster — RE map + verbatim-port roadmap (2026-06-16)

Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
(Ghidra pool2, read-only). This is the **item-1 (gameplay fidelity)** core: the
standalone currently uses a SCAFFOLD kinematic handling model (D3d9Render/
TrackRenderer::UpdateCar — harvested-rate, single-body); the original is a full
**RenderWare-Physics rigid-body + per-wheel** simulation. This documents the real
cluster so the verbatim port can be phased.

## Call graph (per-frame vehicle update)

- **FUN_00470670** — control INPUT integrator (`this`=vehicle in EAX; param_3 =
  input byte[]: [0]=accel, [1]=brake/reverse, [5]=?). Clears the per-axis control
  output slots (+0x1a8/+0x26c/+0xb14..0xb20), computes the speed-normalized
  forward drive force from input × per-vehicle accel (+0x190) × tuning consts,
  writes drive torque to +0x1a8/+0x26c (+ a 16-slot ring at +0x1ac/+0x270), then
  dispatches:
  - **FUN_0046ddb0**(dt, ctx) — the physics core: per-wheel RW contact query
    (FUN_004c3df0 transform, FUN_004c4d20 matrix, FUN_004c3ac0 |v|, FUN_004c39b0
    normalize), surface-grip/handbrake scaling, drive-torque distribution, the
    angular-velocity (+0x26c..0x26e) drag/integrate, per-wheel suspension +
    friction forces (loop over 4 wheels, stride 0x31 ints = 196 B), steer torque
    (FUN_00472650), reads the RW-Physics collision contact arrays per car
    (DAT_008815a4/008822a8/00882fac/00883cb0).
  - **FUN_00467650**(p1,p2,ctx,input) — [unmapped: 2nd integration step]
  - **FUN_00468980**(p2,input) — [unmapped: 3rd step]
  - end: if state +0x9f0 == 2, damps the velocity vec (+0x9b0..0x9b8) by
    _DAT_005cc9c8 (a stopped/parked decay).

## Vehicle struct (partial; `this` base, byte offsets unless noted)

| off | field (observed use) |
|---|---|
| +0x14/0x15/0x16 (int[]) | per-vehicle mass / scale factors (drive-force product) |
| +0x190 (400) | per-vehicle accel/force magnitude (input × this) |
| +0x9a8 | current-wheel-set index (×0x40 + 0x928 = wheel block) |
| +0x9b0..0x9b8 | **linear velocity vector** (world) |
| +0x9d4..0x9dc | **forward vector** (world) |
| +0x9e4 | **max speed** (speed-normalize denom) |
| +0x9f0 | motion state (2 = parked/stopped) |
| +0x1a8/+0x26c | drive/steer torque output (this-frame) |
| +0x1ac/+0x270 (×16) | torque ring buffers (DAT_007f101c&0xf phase) |
| +0xb0c | slide measure (vel·fwd residual) |
| +0xb14..0xb28 | control-output + per-input scratch |
| +0x256/+599/+600 (int[0x10] stride) | wheel/contact world positions |
| +0x278 (int) | wheels-in-contact count (×_DAT_005cc320) |
| +0x279 | (mass-ish; drive-force + impulse scale) |
| +0x2b4 + 0x2b5..(×3,2 slots) | angular-velocity history ring |
| +0x2c0..0x2c2 | rumble/output vec |
| +0x340 | (state flag affecting wheel slip) |
| wheels: +0x5b + n×0x31 | per-wheel block (contact flag +0xb, dir +0xf,
|  | transform +5.., friction/force scratch +8..0x10, grip +0x1a..) |

Struct stride between cars in the contact loop = **0xd04 bytes** (~3.3 KB).

## Dependencies (must port first / vendor)

- **RW math primitives**: FUN_004c3df0 (RwV3dTransformPoints), FUN_004c4d20
  (RwMatrix build), FUN_004c3ac0 (RwV3dLength), FUN_004c39b0 (RwV3dNormalize).
  **WS-A2 DONE 2026-06-16** — all four ported + C4 (diff-original GREEN + canonical
  install-observe, build 26200): FUN_004c3ac0 already C4 (Math/Vec3.cpp Vec3Magnitude);
  the other three now Math/RwV3dNormalize.cpp (13/13), Math/RwMatrixRotate.cpp (10/10,
  x87 fsin/fcos inline-asm), Math/RwV3dTransformPoints.cpp (7/7 dispatch thunk). The
  Rodrigues inner FUN_004c4a50 is ALSO ported + C4 (Math/RwMatrixRotateInner.cpp, 10/10;
  the 3x3 build is inline __asm verbatim x87 because plain C++ is 1-ULP off on the R11
  diagonal — MSVC spills (1-y²)·omc to f32 under FPU register pressure); RwMatrixRotate
  now calls it by C++ symbol, so it works standalone for mode 0. Concat modes 1/2 still
  dispatch the RW device matrix-mult, so they need RW device init in the standalone (WS-E).
  Also Math/RwV3dTransform.cpp, RwV2d, RwSqrt, RwMatrixScale (the 8 pre-existing C4 leaves).
- **RW-Physics collision** contact arrays (DAT_008815a4..) — the qhull/RW-Physics
  3.7 island (see [[qhull-rwphysics-island]], ~165 KB, 0x57c5b0..0x5a5820). The
  per-wheel ground + car↔car contacts feed FUN_0046ddb0. This is item-1's
  COLLISION sub-item and is a prerequisite for faithful handling.
- The vehicle **init/spawn chain** that allocates + fills the ~3.3 KB struct
  (mass, wheel layout, accel/grip/suspension params from the .DFF/handling data)
  — unmapped; required before the update functions read valid fields.
- ~Hundreds of tuning floats (DAT_005cc3xx, DAT_005ceaxx, DAT_005cd0xx...).

## Phased port plan (item 1 = this, multi-session)

1. Port the RW math primitives the cluster needs (**DONE — WS-A2 2026-06-16**:
   FUN_004c39b0/004c4d20/004c3df0 ported + C4; FUN_004c3ac0 already C4).
2. Define the vehicle struct (full 0xd04 layout) as a C++ type.
3. Port the vehicle init/spawn chain (struct population from handling data).
4. Port FUN_00470670 -> FUN_0046ddb0 -> FUN_00467650 -> FUN_00468980 verbatim,
   harvesting every DAT_005c constant.
5. Wire RW-Physics collision contacts (the qhull island) as the contact source.
6. Replace TrackRenderer::UpdateCar's scaffold with the ported cluster; verify
   bit-identity vs the original via diff-original (C4).

REALITY: this is the single largest remaining subsystem — a full vehicle +
RW-Physics port, realistically many sessions. The current scaffold is playable;
this is the fidelity replacement.
