# Session prompts — wave 12 (finish the physics C4 + runtime). Paste one per session.

Generated 2026-06-17. STATUS: 2/5 physics chain fns are bit-identical C4 (A4 96/96, A5 96/96)
via the .asi verbatim-LUT installed-hook lane (Vehicle/PhysicsChainHooks.cpp). A6a FUN_00467650
is 93/96 (C2 — the 3 RED are 1-8 ULP in the x87 float10 per-wheel suspension-force X block,
disasm 0x468127..0x468337). A6b/A3 unattempted. The standalone DRIVES + STEERS a real-physics
race. COMMON PREAMBLE: re/SESSION_PROMPTS.md. Rules: Frida/boot/standalone-runtime = MAIN tree
only; analyzed Ghidra clones (pool11/12/6, NOT pool4); subagent = `general-purpose`, self-
contained; NO-GUESSING; commit nanofives + trailer; guard original/; only DATA faults matter;
kill MASHED.exe after runs; in-process self-test (not per-frame Interceptor); COMMIT INCREMENTALLY.

DEP/PRIORITY: A6B+A3 first (likely easier -> 4/5 C4), then A6a-FINISH (hard naked-asm), then
COVERAGE + SMOKE-STEER + AI-VERIFY, then tails.

---

## WS-PHYS-C4-A6B + A3 — A6b FUN_00468980 + A3 FUN_0046b540 to C4 (LEAD, MAIN TREE). Pool: dev .asi + pool11
> Extend the lane (reuse the A4/A5/A6a template in PhysicsChainHooks.cpp). **A6b** (aero stabilize,
> small): register-ABI trampoline + OrigA6b early-return + forward FUN_004a3384 (asin/acos) +
> FUN_004c4d20 (device matrix, no-visible-args — disasm its register ABI) to the LIVE originals;
> apply A6a lessons (exact-bit Cf(0x..) consts, FSTP-float stores, float-reinterpret reads, float10
> kept across compute+store where the asm does). Bit-diff the rotation-apply fields. **A3** (spawn-
> time init, off hot-path — likely the easiest): self-test the record init (mass +0x50/+0x54/+0x58,
> wheel geometry, the 0x00613140 handling-table walk + the FastSqrt loops). C4 each via re-classify
> (bit-identical only; no overclaim; doc unhit branches). => 4 of 5 physics fns C4.

## WS-PHYS-C4-A6A-FINISH — close A6a's float10 suspension residual (MAIN TREE). Pool: dev .asi + pool11
> The 3 RED A6a calls diverge 1-8 ULP in the per-wheel suspension-force X (piVar12[0x1c]) + the
> dependent +0x9c0. Root: the grounded-suspension force block (disasm 0x468127..0x468337) keeps
> local_bc/local_98/f5/f4 in x87 float10 across compute+store. Write a naked-asm shim that
> reproduces that block's exact x87 register lifetime (the GearCvtCompute/DriveForceAccum shims are
> the pattern). Re-run the self-test to 96/96; promote A6a -> C4. HIGH regression risk — keep it
> isolated + re-run A4/A5 self-tests after to confirm no regression. (Per the ratified full-C4-lane
> direction; if it proves intractable, document A6a as behaviorally-faithful-C2 at 93/96 + STOP.)

## WS-PHYS-COVERAGE — close the A4/A5/A6a unhit branches (MAIN TREE). Pool: dev .asi + Frida
> Drive a scenario hitting the transcribed-not-exercised branches: brake (A4 in1, A6a BrakeForceAccum),
> a jump/ramp (A5/A6a airborne grounded!=4.0), a random-surface tile (A5/A6a RNG key 0xffff32ff),
> helicopter type-C, spinning-powerup vel-damp. Self-test live; bit-confirm + remove the coverage
> caveats from hooks.csv (or fix any divergence the new branches expose).

## WS-PHYS-SMOKE-STEER — calibrate steering + back-port exact consts (MAIN TREE). Pool: standalone + pool11
> mashed_re.exe MASHED_REAL_PHYSICS=1 + steer input: heading follows input (not a circle), verify/
> flip steer sign, calibrate kYawScale to the original turn radius. Back-port the C4-found exact-bit
> consts into the STANDALONE bodies: VehicleControl.cpp kGripMul 0x392ec33e; ForceIntegrator.cpp
> 005cc990 0x3727c5ac + 005ccd08 0x453b8000; Integrate2.cpp the 15 A6a consts (005cea2c 0x48927c00
> etc. — full list in re/analysis/phys_c4_evidence/A6a_FINDINGS_2026-06-17.md). Build + commit.

## WS-AI-VERIFY — AI runtime (MAIN TREE). Pool: standalone run
> mashed_re.exe MASHED_REAL_AI=1 + race: opponents follow the AI%d.AI line? Resolve [U-C-STEER-MAG]/
> [U-C-RATE0/1]; fix ctrl->motion. Screenshot/log opponent paths.

## WS-D-VISUAL / WS-J2 / WS-E-POLISH — tails
> D-VISUAL powerup visuals on the spike; J2 collision FX from live contacts; E-POLISH vehicle
> lighting (ledger #9) + HUD on the spike.

---
## Recommended wave-12 (MAIN tree, serialized)
**12a:** WS-PHYS-C4-A6B+A3 (=> 4/5 C4). **12b:** WS-PHYS-C4-A6A-FINISH (naked-asm; or accept 93/96
+ stop). **12c:** COVERAGE + SMOKE-STEER(+const-backport) + AI-VERIFY. Then tails. GOAL: 4-5/5
physics fns bit-identical C4 + a calibrated, AI-populated real-physics race.
