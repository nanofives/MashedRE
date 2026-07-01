# A5 / A6a airborne coverage — finding from the A6b Warzone lane (2026-07-01)

While closing A6b's airborne body (→ C4) on the newly-found jump track **Warzone**
(engine idx 11), I reused the same natural-airborne scenario to try to clear the
**A5 / A6a airborne env-caveats** (their C4s were granted on GROUNDED frames only;
airborne was "unverified"). Anchored MASHED.exe SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`. Branch `ws-phys-c4-a6b`.

## A5 VehicleWheelForceIntegrator (0x0046ddb0) — airborne NOT clean: 1-ULP float10 residual

`scenario_launch.py --track 11 --cars 4 --hold 45 --hooks 0x0046ddb0` (natural, no boost),
6 warps. A5's airborne branch triggers at `grounded != 4.0` (partial wheel-lift: observed
grounded = 1.0/2.0/3.0), which is broader than A6b's full-air (0.0), so it is reached more
readily. Result — **13 airborne frames: 5 exactly `ndiff=0`, 8 diverge by EXACTLY 1 ULP**:

- Every one of the 15 diverging field values is `|orig - mine| == 1` in the u32 bit
  pattern (verified) — i.e. a single-ULP float32 difference, never a large/logic diff.
- Diverging fields (record offsets): `+0x214` (6×), `+0x21c` (4×), `+0x2d8` (2×),
  `+0x2e0` (3×) — the per-wheel suspension-force components.
- This is the **same `[U-A6A-FLOAT10]` x87 extended-precision rounding-order residual
  class** that held A6a at C2 until it was closed with verbatim x87 shims. A5's airborne
  suspension path keeps its intermediates in the 80-bit x87 stack across the compute+store;
  the C transcription rounds to float32 early → 1-ULP cancellation drift on some frames.
  It is a **faithfulness residual, not a logic error** (grounded path is bit-identical).

Evidence: `A5_airborne_natural_float10residual_20260701.txt` (253 rows; airborne rows
carry the `w+0x…:o=…,m=…` detail lines).

A5's GROUNDED path stays bit-identical (288/288 GREEN in the combined batch, and the
grounded majority of the 253 rows here). So A5 = **grounded C4-clean, airborne 1-ULP
float10 residual (unshimmed).**

## A6a VehicleWheelDrivetrainUpdate (0x00467650) — airborne MEASURED, CLEAN ✅

A6a reached C4 on 2026-07-01 by shimming its GROUNDED suspension-force float10 residual
(72/72 GREEN, all grounded — see CHANGELOG). Its AIRBORNE path was then measured on
Warzone (A6a-only, `--track 11 --cars 4 --hooks 0x00467650`, 6 warps): **20 airborne
frames (AI slot 2, grounded=0.0), ALL `ndiff=0` bit-identical, 0 RED.** So A6a is now
BOTH-BRANCH verified (grounded 72/72 + airborne 20/20). Contrary to the earlier
inference, A6a's airborne path carries **no** float10 residual — its grounded x87 shims
(SuspOrient/SuspDotInv/SuspBaseBc/SuspForceStore) cover the airborne suspension path too.
The 1-ULP residual is **A5-only** (A5 was never shimmed — its grounded path was GREEN
without shims, but its airborne susp-force exposes the x87 rounding). Evidence:
`A6a_airborne_natural_GREEN_20260701.txt`. A6a's airborne-unmeasured caveat is CLEARED.

## Confidence implication (needs a decision — NOT changed unilaterally)

A6b's airborne body is genuinely bit-identical (aero rotation math: acos/RwMatrixRotate/
normalize) → clean C4. But A5 and A6a are different: their C4s were verified only on
grounded frames, and A5's airborne is now measured to carry a 1-ULP float10 residual
(A6a's airborne is unmeasured but same class). Two consistent dispositions:

1. **Accept-as-faithful** (keep A5 C4): upgrade A5's airborne caveat from "unverified" to
   "characterized 1-ULP float10 residual (accepted faithful, same documented x87 class as
   HudSpinCoinAnim / pre-shim A6a)." Precedent exists for accepting float10 residuals as
   faithful. A6a likewise stays C4 with an airborne-unmeasured caveat.
2. **Demote-and-shim** (A5 → C3): by the same standard that held A6a at C2 until its
   float10 residual was shimmed, A5's airborne 1-ULP residual makes its airborne path not
   bit-identical → C3 until an airborne suspension-force x87 shim lands (mirroring the A6a
   grounded shim). A6a would then also warrant airborne verification/shim.

This is a substantive tracker-confidence question spanning two functions the current
session did not promote — surfaced for the user rather than decided. A6b's C4 (this
session's task) is independent of it and stands on its own bit-identical evidence.
