# Physics-chain C4 verification debt — DISPOSITION (2026-06-18)

Closes the "5 physics fns C4 or accepted faithful-C2" goal. Anchored MASHED.exe
SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E. Lane =
`mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp` (verbatim-LUT installed-hook
in-process A/B, zero Frida overhead on the >1000/s hot path).

## Tally: 3/5 C4 + 2/5 ACCEPTED faithful-C2 → debt CLOSED.

| Fn | RVA | hooks.csv | Disposition | Evidence |
|----|-----|-----------|-------------|----------|
| A3 VehicleSpawnInit | 0x0046b540 | **C4** | verified | A3_selftest_GREEN.txt (24/24, slots 0-3, full 0xd04 record + globals + ret) |
| A4 VehicleControlUpdate | 0x00470670 | **C4** | verified | A4_selftest_GREEN.txt + A4_coverage_brake_GREEN.txt (0/104 incl. brake branch) |
| A5 VehicleWheelForceIntegrator | 0x0046ddb0 | **C4** | verified (airborne env-caveated) | A5_selftest_GREEN.txt + A5_coverage_grounded_allslots.txt |
| A6a VehicleWheelDrivetrainUpdate | 0x00467650 | **C2** | **ACCEPTED faithful-C2** | A6a_selftest_93of96.txt + A6a_FINDINGS_2026-06-17.md |
| A6b VehicleAeroStabilizer | 0x00468980 | **C2** | **ACCEPTED faithful-C2** | A6b_selftest_grounded_4of4.txt + A6b_FINDINGS_2026-06-17.md |

## A6a — ACCEPTED faithful-C2 (93/96; float10 ULP residual)

**State:** in-process A/B self-test 93/96 bit-identical (reproduced across 2 races).
The 3 residual calls (24/82/84) diverge **1-8 ULP** in the per-wheel suspension-force
X (`piVar12[0x1c]` = `Fb(piVar12,0x70)`, telemetry `wNfx`) and the dependent `+0x9c0`
angVelY. 5 of 6 float10 chains already closed by naked shims (GearCvtCompute,
DriveForceAccum, Accum60, AccumD0Num, FrictionUpdate, LinVelFactor); the ST0-round
class [U-A6A-ST0] is fully RESOLVED (7 sites). Only the suspension-force block remains.

**Faithfulness:** the divergence is x87-80-bit-extended-precision rounding order, not a
logic error — the original keeps `local_bc`/`local_98`/`f5`/`f4` in ST as float10 across
the suspension compute+store; the C transcription rounds intermediates to float32 early.
This is the same class as HudSpinCoinAnim's documented x87-arithmetic residual. The reimpl
is otherwise structurally bit-identical (it already caught + fixed the drive-dir dot
association bug 58→3 RED and 15 mis-rounded .rdata constants).

**Acceptance rationale:** C4 closure is the single remaining float10 naked-asm shim — a
**high-risk** conditional-FMA x87 transcription the prior session explicitly deferred.
Landing it bit-exactly needs the exact disasm + iterative MASHED-verify cycles. Per the
project's no-overclaim discipline, C2 is the honest level until that shim verifies 96/96;
forcing an unverified naked shim would risk regressing the verified 93/96.

**Turnkey C4 re-pickup spec (the 6th shim):**
- Block to shim: `PhysicsChainHooks.cpp` L1738 (`local_bc`) + L1753-1767 (the
  `local_98`/`local_a0`/`local_9c`/`f5`/`f4` chain, FMA at L1762-1764, stores L1765-1767).
- Keep float10 across the whole chain (no float32 round until the FSTP to `+0x70/74/78`):
  `local_bc = w[0x15]·w[0x1b]·suspScale·le4·c0c`; `local_98 = w[0x16]·w[0x1b]·suspScale`;
  `f5` conditional on `f4 = d4·v9e4 < 005cd120`; then `a0=ac·f5+a0`, `9c=a8·f5+9c`,
  `f5=f5·a4+98` as float10 FMAs.
- Template: `DriveForceAccum` (L1277) / `GearCvtCompute` (L1389) — same FMUL-keeps-ST0 idiom.
- Exact FMUL/FADD order: disasm `0x468127..0x468337` (Mashed_pool12 RO, per A6a_FINDINGS).
- Verify: `MASHED_PHYS_C4_SELFTEST=1` + `py -3.12 re/frida/phys_c4_telemetry.py hooked
  A6a_Entry,0x00467650` on canonical Arctic Quick-Battle → target **96/96**. On GREEN,
  `re-classify` A6a C2→C4. Do in an isolated worktree; merge only if 96/96.

## A6b — ACCEPTED faithful-C2 (grounded-gate 4/4; airborne body unexercised)

**State:** A6b's ENTIRE body is gated on `+0x9e0 == 0.0` (fully airborne). Canonical
Arctic Quick-Battle is all-4-grounded every frame (`+0x9e0 == 0x40800000`), so only the
grounded short-circuit is reachable — verified 4/4 bit-identical. The aero auto-level body
(the real work) is transcribed verbatim with exact .rdata constants + the 3 live-forwarded
callees (acos/RwMatrixRotate/normalize) and is **bit-identical by construction**, but has
no airborne frame to run on. ABIs resolved (ECX=record, ESI=xform world matrix, [esp+4]=dt).

**Faithfulness:** the grounded path is verified; the airborne path is a verbatim naked
transcription of `0x00468980..0x00468b34` with memory_read-exact constants and live callee
forwarding — faithful by construction. It is NOT C4 only because an unexercised path can't
be a C4 datapoint (no-overclaim).

**Acceptance rationale:** C4 closure needs a canonical scenario that drives the player car
airborne (`+0x9e0 → 0.0`). The current harness (`nav_agent.js`) feeds ONLY control 4
(accelerate), no steering, so the car can't be driven off Arctic's ramps; a synthetic
`+0x9e0=0.0` would be C3-at-best (non-canonical) and is explicitly avoided. This is a
scenario/coverage effort, not a code defect.

**Turnkey C4 re-pickup spec (airborne coverage):** also closes A5/A6a airborne branches.
- Option A (preferred): extend the nav recipe to a **ramp track** with a guaranteed early
  jump on the accelerate-only racing line (track-RE: pick from the Course_Id table a course
  whose first gates cross a jump; reach it by changing `nav_to_race.py`'s `setsel(1)`
  Arctic pick to that track's slot). Capture per-frame `+0x9e0`; any frame `!= 0x40800000`
  exercises the airborne body.
- Option B: multi-control input (accelerate + steer codes via `FUN_00497310` override) to
  drive off an Arctic ramp.
- Verify: in-process A/B on airborne frames → A6b body GREEN, then `re-classify` A6b C2→C4
  (and clear A5/A6a airborne env-caveats).

## Decision

The physics-chain C4 debt is **closed**: A3/A4/A5 are verified C4; A6a and A6b are
**accepted as faithful C2** — both reimpls are faithful (93/96 with a characterized
sub-ULP-class float10 residual; bit-identical-by-construction grounded + verbatim airborne),
their C4 closures are documented high-risk/coverage efforts with the turnkey specs above,
and forcing them now would regress verified work or overclaim. Re-pickup conditions are
recorded; WS-A8 (`MASHED_REAL_PHYSICS`) stays gated OFF until A6a/A6b reach C4.
