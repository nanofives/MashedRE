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
  RESOLVED in the follow-up below: normal driving = +0x10 == 0 (wheel-force rebuild arm).

## Follow-up 2026-08-25: the ESI[4] / +0x10 gate

Ghidra pool slot Mashed_pool14 (read-only), image base 0x00400000. Same NO-GUESSING discipline.
`unaff_ESI` in FUN_0046e9e0 = the vehicle record base; ESI[4] = int at record **+0x10** = global `DAT_008815b0`
(0x8815b0 - 0x8815a0 = 0x10). Getter `FUN_0046c770` (0x0046c770 reads DAT_008815b0), setter `FUN_0046c790`
(0x0046c790 writes `(&DAT_008815b0)[slot*0x341]=param_2`). Do NOT confuse with `FUN_0046c7b0` (0x0046c7b0)
which is the getter for **+0x4** (DAT_008815a4), the separate "car active" flag.

### VERDICT (Q2): normal driving player car takes the +0x10 == 0 arm = OMEGA REBUILT FROM WHEEL FORCES.

Evidence chain (all cited, static):
1. Per-vehicle init `FUN_0046b540` (0x0046b540 — mass/suspension/handling setup) writes BOTH
   `(&DAT_008815a4)[slot*0x341] = 1` (+0x4 = 1) and `(&DAT_008815b0)[slot*0x341] = 0` (**+0x10 = 0**),
   literally, in the same block. So a freshly-prepared race car has +0x10 = 0.
2. The ONLY writer of +0x10 to a nonzero value is `FUN_0046c790(slot,1)`, called from exactly one site:
   `FUN_00422fd0` (0x00422fd0) does `FUN_0046c790(param_1,1)` — a "(re)place car at start" routine
   (also sets grid position 0x42480000=50.0 via FUN_004215c0 x2, FUN_0045ba00(slot,2)).
3. Every FUN_00422fd0 invocation is an event/mode transition, never the steady driving tick:
   - dispatcher FUN_00470c70: only when `DAT_007f0fd0 == 1` AND target slot's +0x4 (`FUN_0046c7b0(DAT_007f0fd4)`)
     `== 0` — i.e. a mode-1 activation of cars that are NOT already +0x4-active.
   - FUN_00424eb0 (elimination/round counter, gated by game modes DAT_007f0fd0 ∈ {4,7,8,9,10}): only for
     slots whose state marker `local_20[slot] == 0xff` (eliminated/finished).
   - FUN_004039f0 (countdown/bonus progress): only when the countdown value `DAT_007f0fe4` crosses < 0.
   - FUN_0040d590 / FUN_0040eee0 / FUN_00410d10 hold the remaining ~18 call sites (0x40d6xx / 0x40f0xx /
     0x410dxx); FUN_0040eee0 is invoked at the TAIL of FUN_00424eb0's all-eliminated branch. These are the
     party/elimination/round-advance machine, not the ordinary per-frame update. [see residual below]
4. +0x10 is reset to 0 ONLY by a full `FUN_0046b540`. The lighter respawn init `FUN_0046baa0` (0x0046baa0)
   does NOT write +0x10 (verified: its store list has no DAT_008815b0), so once set to 1 it persists until
   a full re-init.
5. CORROBORATION from A6a `FUN_00467650`: the wheel-torque accumulate into the angular-velocity triple is
   guarded by `if (*(int *)(unaff_ESI + 0x10) == 0)` (the `+0x9bc/+0x9c0/+0x9c4 +=` block near 0x4688xx).
   The port's already-verified "+0x9c0 computed correctly during driving" result lives inside that branch;
   it can only be the live branch during normal driving, so +0x10 == 0 there. This also dissolves the
   apparent tension: with +0x10==0 A6a WRITES the physical angular velocity (consumed by the +0x9e8 magnitude
   cache, particle rotation, collision), while FUN_0046e9e0 independently rebuilds the body-matrix omega from
   the same wheel-force model. With +0x10!=0 the triple is externally seeded and A6a leaves it, and
   FUN_0046e9e0 KEEPS it as omega — a scripted/kinematic body-rotation mode (eliminated cars, mode-placed cars).

Mechanical role of +0x10: a per-car mode flag, values {0,1} only. 0 = wheel-force-driven body (normal);
1 = start-placed / scripted-angular-velocity body. Not a counter, not a pointer.

### +0x10 writer table

| RVA | value written | mechanical description |
|-----|---------------|------------------------|
| 0x0046b540 (FUN_0046b540, per-car init) | 0 | `(&DAT_008815b0)[slot*0x341]=0` alongside +0x4=1; the "prepare car for race" setup |
| 0x0046c790 (FUN_0046c790, setter) | param_2 | `(&DAT_008815b0)[slot*0x341]=param_2`; bounds-checked slot<16; sole caller FUN_00422fd0 passes 1 |
| via FUN_00422fd0 (0x00422fd0) | 1 | calls FUN_0046c790(slot,1); "(re)place car at start" — mode-1 activation, elimination(0xff) respawn, countdown cross |

Readers/tests of +0x10 (for completeness): FUN_0046e9e0 `if(ESI[4]==0)` (the arm gate);
FUN_004709a0 `CMP [rec+0x10],1` at 0x00470b37 and FUN_00470c70 `*piVar==1` at ~0x004710xx — both the
"car live" gate `+0x4!=0 || +0x10==1`; getter FUN_0046c770; FUN_0046c7d0 reads it at 0x0046c811.

### Q3 — the +0x10 == 0 arm ("omega rebuilt from wheel forces"), full decomp

Setup before the gate (runs regardless):
```
fVar1   = param_1 * _DAT_005cc948;            // param_1 = substep dt; _DAT_005cc948 = 0x39aec33e ~ 3.334e-4
scale   = fVar1 * _DAT_005cc32c;              // _DAT_005cc32c = 0x3f000000 = 0.5   -> seed scale
local_24= scale * ESI[0x26f];  // +0x9bc      (angular-vel seed X)
local_20= scale * ESI[0x270];  // +0x9c0      (angular-vel seed Y)
local_1c= scale * ESI[0x271];  // +0x9c4      (angular-vel seed Z)
FUN_0046d700(&local_18, *ESI); // local_18/14/10 = body FORWARD row from the orientation matrix (*ESI = matrix ptr)
```
Then `if (ESI[4]==0)` WIPES the seed and rebuilds omega from steering/wheel forces:
```
local_24=local_20=local_1c=0;
grip = ESI[0x279]*_DAT_005ce1e8;  clamp grip <= _DAT_005cc320;                 // ESI[0x279]=+0x9e4 linear speed mag
ESI[0x33f]=0;  if (in[4]!=0 && in[5]!=0){ ESI[0x33f]=1; grip=_DAT_005cc320; }  // in=param_2 bytes; [4]&[5] set => grip=max
if (ESI[0x278] != DAT_005d757c) {   // +0x9e0 != 0.0  (grounded/steerable gate)
  // LEFT steer col: s = (float)in[0]  (+ ESI[8] trim, clamp _DAT_005cd04c);  if ESI[0xc]!=0 scale by (_DAT_005cc320 - ESI[0xc]*_DAT_005cc328)
  //   torque_L = s * dt * _DAT_00613108 * grip * _DAT_005cd03c * _DAT_005cc948
  //   omega  = forward(local_18/14/10) * torque_L
  // RIGHT steer col: s = (float)in[1] (+ ESI[9] trim, clamp);  same ESI[0xc] scale
  //   torque_R = s * dt * _DAT_00613108 * grip * _DAT_005cd03c * _DAT_005cc948
  //   omega -= forward * torque_R          // opposite sign => net yaw from L/R steer differential
}
if (in[4]!=0 && in[5]!=0) omega *= _DAT_005cc348;                              // both bytes => spin scale
if (ESI[0x278] != 0x40800000) { omega.x += k*ESI[0x26f]; omega.z += k*ESI[0x271]; } // +0x9e0!=4.0: re-add residual ang-vel X/Z
if (dot(ESI[0x275..277], ESI[0x26c..26e]) < _DAT_005cd0fc && (in[4]==0||in[5]==0)) omega = -omega; // reverse when moving backward
```
in[] = param_2 byte descriptor; per WS-C map [0]/[1] = steer L/R, [4]/[5] = accel/brake. ESI[0xc]=+0x30,
ESI[8]=+0x20, ESI[9]=+0x24 (per-wheel steer trims). `_DAT_00613108` = 100.0 (set by FUN_0046b540).

### Q4 — the +0x10 != 0 arm, and the +0x144 accumulator

- +0x10 != 0: the entire `if(ESI[4]==0){...}` block is skipped, so omega retains the seed
  `0.5*_DAT_005cc948*dt * (+0x9bc,+0x9c0,+0x9c4)` computed above. The +0x9bc/+0x9c0/+0x9c4 contribution is
  KEPT UNMODIFIED. Confirmed.
- The +0x144/+0x148/+0x14c accumulator (ESI[0x51/0x52/0x53]) is updated and consumed in BOTH arms — it is
  OUTSIDE/after the gate:
  ```
  if (ESI[0x278]==DAT_005d757c) ESI[0x51..53] += local_24..1c;           // +0x9e0==0.0
  damp = (_DAT_005ccd08 - dt*_DAT_005cc35c) * _DAT_005cc948;
  ESI[0x51..53] = (local + ESI[0x51..53]) * damp;                        // persistent damp
  k2 = dt * _DAT_005ce018;                                               // _DAT_005ce018 = 0x3b03126f ~ 0.002
  if (in[1]==0 && in[0]==0) omega += k2 * ESI[0x51..53];                 // add accumulator ONLY when NOT steering
  ```
  So the accumulator term is present in both arms; its ADD into omega is gated on steer bytes [0]&[1]==0.
- Brake/handbrake gate bytes: param_2[4] and param_2[5] (accel/brake per WS-C). Both-nonzero =>
  grip clamp + `_DAT_005cc348` spin scale; the reverse-sign gate additionally requires (in[4]==0 || in[5]==0).
  The 7/`ESI[1]==1` special case (FUN_0040e350()==7, +0x4==1) overrides omega with a forward-axis spin
  `(2*dt)*_DAT_005cc948*ESI[0x279]*_DAT_005ce268`.

### Q5 — constants

- `_DAT_005cc32c` = 0x3f000000 = **0.5** (at 0x005cc32c). It is the exact global read as the omega-seed scale
  in FUN_0046e9e0 (`scale = dt*_DAT_005cc948 * _DAT_005cc32c`). [UNCERTAIN] identity with "Integrate2's 0.5
  floor": the VALUE is 0.5, but I did not open Integrate2 to confirm it dereferences THIS address 0x005cc32c
  rather than another 0.5 literal — needs Integrate2's RVA to check the operand.
- `_DAT_005ce018` = 0x3b03126f = **~0.0020001** (at 0x005ce018) — the accumulator-term scale (omega += dt*this*accum).
- (context) `_DAT_005cc948` = 0x39aec33e ≈ 3.334e-4 (the per-substep dt normalizer used by both the seed scale
  and the linear-velocity position term).

### Residual [UNCERTAIN]

I did NOT exhaustively decompile FUN_0040d590 / FUN_0040eee0 / FUN_00410d10 (dense FUN_00422fd0 call sites)
to PROVE none fires for the player slot on the ordinary race-start/grid path without a following FUN_0046b540
re-init. All read context points to them being the party/elimination/round-advance machine (FUN_0040eee0 is
called from FUN_00424eb0's all-eliminated tail; FUN_00424eb0 special-cases modes {4,7,8,9,10}), so normal
single/championship driving should not reach them for the player. If it did, a player could enter driving with
+0x10==1 (seed-kept arm). EXACT PROBE to harden before the code change: Frida-hook FUN_0046e9e0 entry (or read
`*(int*)(DAT_008815a0 + playerSlot*0xd04 + 0x10)`) per frame across a real normal-race driving session; a
steady 0 confirms the wheel-force arm. This is a one-boot behavioral read, not a diff.
