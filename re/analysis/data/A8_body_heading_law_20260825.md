# A8 — How the vehicle BODY HEADING is produced (integrate +0x9c0 vs align-to-velocity)

Date: 2026-08-25. Ghidra pool slot Mashed_pool13 (read-only), MASHED.exe image base 0x00400000.
NO-GUESSING: every claim below cites an RVA/offset literally present in the decompilation.

## VERDICT

**INTEGRATES +0x9c0.** The vehicle's world orientation is a double-buffered 4x4 transform matrix
inside the vehicle record. Each physics substep it is advanced by a first-order rotation integration
`row_new = row_old + (omega x row_old)` in **FUN_0046e9e0** (0x0046e9e0), where the angular-velocity
vector **omega is seeded from the record's angular-velocity triple +0x9bc/+0x9c0/+0x9c4**. There is NO
"align body heading to the velocity heading" law on this (main) physics path. The one place that computes
a wrapped heading difference and applies it as an angular velocity is the *separate*, DAT_006ce274-gated
RWP-bridge FUN_0047eb30, which does **not** read the record's +0x9bc/+0x9c0/+0x9c4 at all.

## Record base / offset arithmetic (established, not assumed)

- Vehicle record base = DAT_008815a0; stride = 0xd04 bytes = 0x341 floats (matches WS-A1 note).
  Verified two independent ways:
  - FUN_0047eb30 iterates `local_8c = &DAT_00881f68`, step `+0x341` floats; 0x00881f68-0x008815a0 = 0x9c8.
  - FUN_0046baa0 (spawn init) zeroes `DAT_00881f5c/60/64` = record+0x9bc/+0x9c0/+0x9c4 (0x881f5c-0x8815a0=0x9bc).
- +0x9b0/+0x9b4/+0x9b8 = LINEAR velocity (mag cached at +0x9e4 by FUN_00467650 @0x00467664).
- +0x9bc/+0x9c0/+0x9c4 = ANGULAR velocity (mag cached at +0x9e8 by FUN_00467650 @0x00467679, `FUN_004c3ac0(ESI+0x9bc)`).
- Orientation matrix = double-buffered 4x4 at record+0x928 and record+0x968, buffer index at record+0x9ac.
  Established in FUN_0046f6c0 @0x0046f6d8: `iVar12 = *(ESI+0x9ac)*0x40 + 0x928 + ESI`; matrix translation
  read at iVar12+0x30/+0x34/+0x38; matrix passed to broadphase FUN_00538c80 and rotation-correction FUN_004c52f0.

## Live per-tick call graph (dispatcher = FUN_00470c70, called from FUN_00425a40)

FUN_00470c70 (0x00470c70) order:
1. FUN_00467350(param_1)                      -- pre-step
2. **FUN_0047eb30()** (RWP bridge)            -- runs only if DAT_006ce274 != 0 (see below)
3. per-vehicle `FUN_00470670(...)` -> **FUN_00467650 (A6a)**  writes angular vel +0x9bc/+0x9c0/+0x9c4
4. `FUN_004c3ac0(rec+0x9b0)`->+0x9e4, `FUN_004c3ac0(rec+0x9bc)`->+0x9e8   (magnitude caches)
5. substep loop `FUN_004709a0(dt, param2)` -> **FUN_0046e9e0** (orientation integrator) + FUN_0046f6c0 + FUN_00469aa0

## FUN_0047eb30 (0x0047eb30) — literal behaviour (the port's cited function)

- Whole body gated by `if (DAT_006ce274 != 0)`; steps an RWP world via FUN_0055deb0(DAT_006ce274, DAT_0061331c=0.05)
  then reads back matrices. It is the coupling bridge to the RWP/qhull island, NOT the main integrator.
- Per vehicle it writes an RWP structure's two vec3s (0x20 stride):
  - linear = `(targetPos - currentPos) * _DAT_005ccd6c`  -> `*(*(*(piVar2+0x10)+8) + piVar2[1]*0x20)` (+0x00)
  - angular X=0, Z=0, **Y = wrapped(headingA - headingB) + up-alignment cross term** -> same base +0x10
- The wrapped heading difference is built with two atan2 calls **FUN_004233e0**:
  - headingA @0x0047ec?? `FUN_004233e0(local_b4, local_ac)` on a FUN_0046d510-produced target vector
  - headingB `FUN_004233e0(local_88, local_80)` on `FUN_004c3df0(&local_88, &DAT_00614708, 1, matrix)` = body forward
  - angle wrap uses _DAT_005ccac4 / _DAT_005cd09c / DAT_005d757c.
- **Crucially: FUN_0047eb30 contains NO access to +0x9bc/+0x9c0/+0x9c4** (confirmed: none of the
  0x9bc/0x9c0 instruction hits fall in 0x0047eb30..0x0047f1db; its record helpers FUN_0046cb30/FUN_0046d510/
  FUN_0046d4a0 are not in the reader set either). So this heading-difference law is the RWP servo's OWN
  angular velocity, independent of the record's angular-velocity triple. This is the function whose
  behaviour the port comment paraphrased as "sets body angVel.y = wrapped(bodyHeading - velHeading)";
  it is a real law but it belongs to the DAT_006ce274 RWP path, not to +0x9c0.

## FUN_0046e9e0 (0x0046e9e0) — the orientation integrator (THE answer)

Register-passed: ESI=record, EDI=source matrix, EBX=dest matrix (the +0x928/+0x968 double buffer).
- Position integration from LINEAR velocity: `EBX[0xc..0xe] = EDI[0xc..0xe] + k*dt*ESI[0x26c..0x26e]` (ESI[0x26c]=+0x9b0).
- omega seed from ANGULAR velocity:
  `local_24 = (dt*_DAT_005cc32c)*ESI[0x26f(+0x9bc)]; local_20 = ...*ESI[0x270(+0x9c0)]; local_1c = ...*ESI[0x271(+0x9c4)]` (@0x0046ea86 FMUL +0x9bc, @0x0046ea90 FLD +0x9c0, @0x0046ece2 FMUL +0x9bc).
  - If `ESI[4]==0`: this seed is overwritten to 0 and omega is rebuilt from wheel forces.
  - If `ESI[4]!=0`: the +0x9bc/+0x9c0/+0x9c4 contribution is KEPT.
- Persistent accumulator: `ESI[0x51..0x53]`(+0x144/+0x148/+0x14c) = `(local + ESI[0x51..])*damp`; then
  `omega += dt*_DAT_005ce018*ESI[0x51..0x53]` (gated on brake/handbrake bytes == 0).
- Rotation integration (first-order `dR = omega x R`), each basis row r:
  `EBX[r] = EDI[r] + (omega x EDI[r])`  for r in rows 0 (idx0..2), 1 (idx4..6), 2 (idx8..10):
  ```
  *EBX      = (omega.y*EDI[2] - omega.z*EDI[1]) + EDI[0]
  EBX[1]    = (omega.z*EDI[0] - omega.x*EDI[2]) + EDI[1]
  EBX[2]    = (omega.x*EDI[1] - omega.y*EDI[0]) + EDI[2]
  (rows 4..6 and 8..10 identical form)
  ```
  where omega = (local_24, local_20, local_1c). Final `thunk_FUN_004c4680()` (matrix cleanup/ortho).
- This is unambiguously an angular-velocity->orientation-matrix integration, and +0x9c0 is a direct input
  to omega. It is the only reader of the triple that writes an orientation.

## All accessors of the +0x9bc/+0x9c0/+0x9c4 triple (image-wide)

Search method: literal `0x9c0` (16 hits) and `0x9bc` (17 hits) instruction operands over .text, PLUS the
indexed/computed-base forms (LEA [reg+0x9bc] passed to vector helpers, and ESI[0x26f..0x271] float-index).
Function mapping via function_at. Note the record-base variants (ESI[0x26f]) ARE covered because they render
as +0x9bc/+0x9c0 in the search and as [0x26f..0x271] in decomp for the same bytes.

| RVA | Function | integrates / tests / writes | evidence |
|-----|----------|------------------------------|----------|
| 0x0046e9e0 | FUN_0046e9e0 | **INTEGRATES** into orientation matrix | omega seed @0x46ea86/ea90/ece2; `EBX[r]=EDI[r]+omega x EDI[r]` rows 0/1/2 |
| 0x00467650 | FUN_00467650 (A6a) | writes (accumulate torque) + tests/clamps + magnitude | @0x468586-59c `+0x9bc += torque`; damp @0x468862/876,0x468911/925; zero @0x468960; mag `FUN_004c3ac0(ESI+0x9bc)`@0x467679; range test in `if(ESI+0x9bc<..)` |
| 0x00468d80 | FUN_00468d80 | tests/reads (builds debris/particle rotation) | `if(param_1[0x26f]==0 && [0x270]==0 && [0x271]==0){zero} else FUN_004c4d20(local_40,param_1+0x26f,..)`; not a body-orientation write |
| 0x00470c70 | FUN_00470c70 | tests/reads (magnitude cache) | `FUN_004c3ac0(piVar14+0x26b)` -> +0x9e8 |
| 0x00468980 | FUN_00468980 | writes 0 (aero stabilization, airborne) | when `+0x9e0==0 && +0x9f0!=0`: `*(+0x9c4)=0;*(+0x9c0)=0;*(+0x9bc)=0` then rebuild rotation from LINEAR vel |
| 0x0046baa0 | FUN_0046baa0 | writes 0 (per-slot spawn init) | zeroes DAT_00881f5c/60/64 = record+0x9bc/9c0/9c4 |
| 0x00469df0 | FUN_00469df0 | writes (store; collision-response set) [role not fully traced] | FSTP [ESI+0x9c0]@0x46ad4f, MOV [ESI+0x9bc]@0x46ad43, MOV [EDI+0x9bc/0x9c0]@0x46ada9/adb5 |
| 0x00404830 | FUN_00404830 | writes int 0x2 -> **DIFFERENT object** [UNCERTAIN] | `MOV [EAX+0x9c0],0x2`@0x404d32; integer 2 into +0x9c0, not the float vehicle record; base not shown to be DAT_008815a0 |

None of the readers other than FUN_0046e9e0 feed an orientation; the rest are the source (A6a), magnitude
caches, particle/debris rotation, spawn/aero resets, or a collision-response store.

## Orientation writer (Q3)

- Orientation STORAGE: record double-buffered 4x4 transform at +0x928 / +0x968 (index at +0x9ac). Rotation in
  first 0x30 bytes, translation at +0x30.
- Per-tick WRITER of the rotation: **FUN_0046e9e0 (0x0046e9e0)** — `EBX[rows]=EDI[rows]+omega x EDI[rows]`.
  Fed by: angular velocity +0x9bc/+0x9c0/+0x9c4 (when ESI[4]!=0) + wheel-force torque + persistent
  +0x144/+0x148/+0x14c accumulator. It is fed by the angular-velocity vector, NOT by a velocity heading.
- Downstream copy of record+0x928 matrix into the RW atomic/frame that the renderer draws: NOT traced in
  this session. [UNCERTAIN — the exact RW-frame push RVA]. FUN_0046da80/FUN_0046f6c0/FUN_00538c80 read the
  +0x928 matrix as the authoritative body transform for collision/wheel probes, which confirms +0x928 is the
  live body transform; the RwFrame sync point was not located.

## Reconciliation with the port

- Port A6a (FUN_00467650) computes +0x9c0 correctly (angular velocity). In the ORIGINAL that value is
  consumed by FUN_0046e9e0 to integrate the body orientation matrix. The port "never consumes +0x9c0"
  precisely because it did not port the FUN_0046e9e0 omega x R matrix integration; it substituted a
  velocity-heading first-order lag.
- The port comment "the original sets body angVel.y = wrapped(bodyHeading - velHeading)" describes
  FUN_0047eb30's RWP-servo law, which (a) is gated on DAT_006ce274 (the RWP world path), and (b) does not
  read +0x9c0. It is therefore not the law governing +0x9c0 on the main path.

## What is NOT established (honesty)

- The RELATIVE weight at runtime of the direct +0x9c0 term vs the +0x144 accumulator inside omega was not
  measured (static only). The MECHANISM (angular integration consuming +0x9c0) is certain; the numeric
  dominance of the direct term in a given frame is not.
- The RW-frame copy RVA (record+0x928 -> atomic LTM) was not located.
- FUN_00469df0's exact write semantics (collision impulse -> angular velocity) were not fully decompiled.
- Whether ESI[4] (the gate that keeps vs discards the +0x9bc seed in FUN_0046e9e0) is 0 in normal driving
  was not determined; but both branches integrate omega x R (angular integration), not velocity alignment.
