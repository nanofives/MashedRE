# WS-A5 — vehicle force integrator FUN_0046ddb0: RE foundation (2026-06-16)

The B4 consumer half. `FUN_0046ddb0` (0x0046ddb0..0x0046e9d0, 3104 b) is the
per-wheel physics core: it is the function that **reads the contact arrays WS-B2/B3
fill** and turns them into the per-wheel suspension/steer/drive forces + the
velocity/airborne updates. Wiring B2/B3 as the contact source (B4) means making
this consume the ported producer's output. This brief is the RE foundation
(verbatim decomp captured this session, pool3); the verbatim port is the next step.

Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.

## Signature / shape

`void FUN_0046ddb0(float param_1 /*dt*/, undefined4 param_2 /*transform matrix*/)`;
`unaff_EDI` = vehicle record (int*). Called only by `FUN_00470670` (control-input
integrator); `FUN_00470670` is the per-car entry that also runs `FUN_00467650`
(drivetrain) + `FUN_00468980` (3rd step). Per-frame driver tree:
`FUN_00470c70 → {FUN_004709a0 (contacts, WS-B), FUN_00470670 → FUN_0046ddb0 (this)}`.

## How it consumes the WS-B2/B3 contact output (the B4 link)

1. **Per-car contact summary** — reads 4 cars' `{active@+4, count@+8}`:
   `DAT_008815a4/008815a8` (car0), `DAT_008822a8/8822ac` (car1),
   `DAT_00882fac/82fb0` (car2), `DAT_00883cb0/83cb4` (car3) — stride 0xd04. The
   max/min contact count across cars scales the grip term (`local_6c`/`fVar5`
   pick `_DAT_005cea70 / _DAT_005cc32c / _DAT_005cc320` by count ∈ {0,1,2,3}).
2. **Grounded-wheel count** — `unaff_EDI[0x278]` (+0x9e0) accumulated from
   `piVar12[0xb]` (wheel contact flag, +0x2c per wheel) over 4 wheels; `==4.0`
   (0x40800000) ⇒ all-grounded branch (drive torque not halved; full suspension).
3. **Per-wheel suspension force outputs** — writes `DAT_00881560/64/70/7c/88`
   (the `DAT_00881560` block = the contact velocity query target the wheel solver
   filled) and the per-wheel steer-torque slots `[0x83]/[0xb4]/[0xe5]/[0x116]`.
4. **Contact-point world positions** — reads `DAT_00881ef8/efc/f00` +
   `DAT_00881f48..f58` (the +0x256 contact-pos array) for the per-wheel arm.

So B4 = run the WS-B contact path (producer + classifier + solvers) to fill these
arrays, then let this integrator consume them — replacing the kinematic scaffold.

## Callees (RE'd this session)

| RVA | role | status |
|---|---|---|
| 0x004c3df0 | RwV3dTransformPoints | ported (Math/) |
| 0x004c4d20 | RwMatrix from axis+angle | ported (Math/RwMatrixRotate) |
| 0x004c3ac0 | Vec3 magnitude | ported |
| 0x004c39b0 | Vec3 normalize | ported |
| 0x0046c5f0 | TriangleFaceNormal | ported (Collision/, WS-B2) |
| 0x00442ce0 | **catch-up/rubber-band grip multiplier** — modes 4/9/8 → 1.0; else per-player `DAT_008989b0[car]` (==0 gate) + race-tier `FUN_0046dbe0(car)` vs `FUN_0046dbe0(DAT_008989c8)` → `1.0 + coeff*x`, coeff ∈ {`_DAT_005cc318`, 0.5, 0.25}. Resolves [UNCERTAIN U-2687]. | RE'd |
| 0x00442c80 | **rubber-band activation gate** — returns 1 iff game-mode==6 ∧ mode∉{4,9,8} ∧ `DAT_008989b0[car] > _DAT_005cc9b8`. Resolves [UNCERTAIN U-3563]. | RE'd |
| 0x00472650 | random float in ±range (wheel impulse jitter) | leaf |
| 0x0046dbe0 | race-position/tier getter (for rubber-band) | needs map |

## Residual dependencies for the verbatim port

- Per-frame scalars `_DAT_0088e610` (gravity·dt term), `_DAT_0088e5f0`
  (suspension scale), `_DAT_00803334/38/3c/40` (gravity vector + magnitude),
  `DAT_007f0fd0` (player count), `DAT_007f0ff8`/`_DAT_005cea7c/78/74` (a
  time-based grip ramp), and ~40 `_DAT_005c*` tuning floats (to harvest).
- The rubber-band per-player float array `DAT_008989b0` + `FUN_0046dbe0`.
- The vehicle-struct fields it writes must already be init'd (FUN_0046b540).

## Verification gate (same class as B2/B3)

`FUN_0046ddb0` is an implicit-`this` (EDI) state-reader/writer over the 0xd04
record — NOT an isolatable leaf, so run_diff bit-identity does not apply. Gate =
installed-hook scenario telemetry vs the captured baseline
(`re/analysis/wsb_contact_baseline.json`): with the integrator live in a demo
race, the per-car grounded count / velocity evolution must match. The WS-B
self-test already proves the upstream contacts (grounded=4) are produced
correctly; this port closes the loop.

## Port — DONE + structurally verified (2026-06-16)

Ported verbatim into `Vehicle/ForceIntegrator.{h,cpp}` (EDI→`int* self`; float
stores via `vF()` reinterpret; float10→double; the surface-jitter dispatch uses
the integer surface keys recovered from the asm — `0xffff32ff`→random,
`0xffa08080`→0.1, `0xffaa8080`→0.01, `0xff961e5a`/`0xff1e80b4`→slip-flag?0.01:0.2,
`0xffc81e5a`→0.2, default 0.25). All ~40 constants harvested + cited in
`ForceIntegrator.h`. `FUN_00442ce0`/`FUN_00442c80`/`FUN_0046dbe0` ported
(RubberBandGrip/Gate/CarContactCount); reuses `Collision::TriangleFaceNormal` for
the steer-feedback normal. Residual RW-math/PRNG/runtime-global deps stubbed inert
in `ForceIntegratorStubs.cpp`. Builds clean into `mashed_re.exe`.

**Verified** by `Vehicle/forceint_selftest.cpp` (builds + runs): the full
integrator runs end-to-end on a populated 4-car array → **grounded count = 3.0**
(consumes the per-wheel contact flags), per-car contact summaries read correctly
(3, 2), rubber-band gate/grip match the decompiler, no OOB/crash → **PASS**. This
proves the transcription is structurally sound and consumes the WS-B contact
arrays correctly. Exact physics fidelity stays gated on installed-hook scenario
telemetry vs the captured baseline.

C-LEVEL: C2 faithful transcription (not promoted; no scenario evidence yet).

## Remaining for B4 (the wiring)

1. Bind the residual stubs to the real Math/ RW primitives + a PRNG + the live
   gravity/susp/player-count globals + the vehicle-array base.
2. Wire `FUN_00470670 → FUN_0046ddb0` and run the WS-B contact path (producer +
   classifier + solvers) before it each tick to fill the contact arrays.
3. Replace `TrackRenderer::UpdateCar`'s kinematic scaffold; diff per-frame
   velocity/grounded telemetry vs `re/analysis/wsb_contact_baseline.json`
   (installed-hook scenario = the C4 gate).
