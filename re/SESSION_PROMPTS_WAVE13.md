# Session prompts — wave 13 (close coverage + finish physics C4 + playable polish). Paste one per session.

Generated 2026-06-17. STATUS: 3/5 physics chain fns bit-identical C4 (A3 24/24, A4 96/96, A5
96/96) via the .asi verbatim-LUT installed-hook lane (Vehicle/PhysicsChainHooks.cpp). A6a = 93/96
C2 (3 RED = x87 float10 per-wheel suspension-force X, disasm 0x468127..0x468337). A6b = C2 (aero
body airborne-gated, unexercised in the all-grounded Arctic demo). The standalone DRIVES + STEERS
a real-physics race. COMMON PREAMBLE: re/SESSION_PROMPTS.md. Rules: Frida/boot/standalone-runtime =
MAIN tree; analyzed Ghidra (pool11/12/6, NOT pool4); subagent=`general-purpose`, self-contained;
NO-GUESSING; commit nanofives+trailer; guard original/; only DATA faults matter; in-process
self-test; COMMIT INCREMENTALLY. **MASHED PROCESS HYGIENE (mandatory, CLAUDE.md "MASHED process
hygiene (multi-session)"): MASHED is NOT single-instance — other sessions run their own. Track the
PID you spawn and kill ONLY that PID (Stop-Process -Id <pid>); NEVER blanket-kill by name
(no Stop-Process -Name MASHED / taskkill /im MASHED.exe); NEVER auto-attach to "first MASHED by
name" — always target your explicit spawned PID.**

KEY INSIGHT: the last-2 physics C4 + the A4/A5/A6a coverage caveats share ONE blocker — the
canonical Arctic throttle-only all-grounded demo never goes airborne / brakes / hits random
surface, and the .asi harness overrides only control-4 (no steering). A richer scenario driver is
the single high-leverage unlock.

---

## WS-PHYS-COVERAGE-SCENARIO — richer scenario driver (LEAD, MAIN TREE). Pool: dev .asi + Frida
> Build a scenario that exercises the unhit branches, with the installed-hook in-process self-test
> LIVE. Extend the harness to drive a RICH input (steer + brake + accel) and reach: a JUMP/RAMP
> (airborne: A5 grounded!=4.0 suspension path, A6a airborne, A6b's whole aero body), BRAKE (A4 in1,
> A6a BrakeForceAccum), a RANDOM-SURFACE tile (A5/A6a RNG key 0xffff32ff). Options: drive an AI car
> (it steers/jumps on its own) and hook its record; or script a multi-control input block; or pick a
> track with ramps. Bit-confirm each branch. OUTCOMES: (a) **A6b -> C4** if its airborne body is now
> exercised + bit-identical; (b) remove the A4/A5/A6a coverage caveats from hooks.csv (or fix any
> divergence the new branches expose). This one investment closes the most C4 debt.

## WS-PHYS-C4-A6A-FINISH — close A6a's float10 suspension residual (MAIN TREE). Pool: dev .asi + pool11
> The 3 RED A6a calls = 1-8 ULP in per-wheel suspension-force X (piVar12[0x1c]) + dependent +0x9c0;
> the grounded-suspension block (0x468127..0x468337) keeps local_bc/local_98/f5/f4 in x87 float10
> across compute+store. Write a naked-asm shim reproducing that block's exact x87 register lifetime
> (GearCvtCompute/DriveForceAccum shims are the pattern). Re-run to 96/96 -> A6a C4. HIGH regression
> risk: isolate it + re-run A4/A5/A3 self-tests after. If intractable, document A6a as behaviorally-
> faithful-C2 at 93/96 and STOP (sound off-ramp).

## WS-PHYS-SMOKE-STEER + CONST-BACKPORT — playable polish + standalone bit-fidelity (MAIN TREE). Pool: standalone + pool11
> Run mashed_re.exe MASHED_REAL_PHYSICS=1 + steer input: heading follows input (not a circle),
> verify/flip steer sign, calibrate kYawScale to the original turn radius. Back-port the C4-found
> exact-bit consts into the STANDALONE bodies: VehicleControl.cpp kGripMul 0x392ec33e; ForceIntegrator
> 005cc990 0x3727c5ac + 005ccd08 0x453b8000; Integrate2.cpp the 15 A6a consts (A6a_FINDINGS doc);
> AeroStabilize.cpp _DAT_005ccae0=180/pi + _DAT_005ccad8=90.0. Build + commit.

## WS-AI-VERIFY — AI runtime (MAIN TREE). Pool: standalone run
> mashed_re.exe MASHED_REAL_AI=1 + race: opponents follow the AI%d.AI line? Resolve [U-C-STEER-MAG]/
> [U-C-RATE0/1]; fix ctrl->motion. (Bonus: a real-AI race naturally produces airborne/brake/steer
> events — could double as the WS-PHYS-COVERAGE-SCENARIO driver.) Screenshot/log opponent paths.

## WS-D-VISUAL / WS-J2 / WS-E-POLISH — tails
> D-VISUAL powerup visuals on the spike; J2 collision FX from live contacts; E-POLISH vehicle
> lighting (ledger #9) + HUD on the spike.

---
## Recommended wave-13 (MAIN tree, serialized)
**13a:** WS-PHYS-COVERAGE-SCENARIO (the high-leverage unlock — A6b C4 + close A4/A5/A6a caveats).
**13b:** WS-PHYS-C4-A6A-FINISH (naked-asm, or accept 93/96 + stop). **13c:** SMOKE-STEER+CONST-
BACKPORT + AI-VERIFY. Then tails. GOAL: 4-5/5 physics fns C4 with full branch coverage + a
calibrated, AI-populated, bit-faithful real-physics race.
