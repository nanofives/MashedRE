# Session prompts — wave 11 (finish physics C4 + close coverage + steer/AI). Paste one per session.

Generated 2026-06-17. 2 of 5 physics fns are now C4 (A4 96/96, A5 96/96) via the .asi verbatim-
LUT installed-hook lane (Vehicle/PhysicsChainHooks.cpp + re/frida/phys_c4_telemetry.py; the
trampoline/forwarder/self-test template is reusable). COMMON PREAMBLE: re/SESSION_PROMPTS.md.
Rules: Frida/boot/standalone-runtime = MAIN tree only; analyzed Ghidra clones (pool11/12/6, NOT
pool4); subagent type = `general-purpose` (fork gone), self-contained prompts; NO-GUESSING;
commit as nanofives + trailer; guard `original/`; boot's ~37k benign EXECUTE AVs are SEH-recovered
(only DATA faults matter); kill MASHED.exe after runs; COMMIT INCREMENTALLY.

REMAINING physics C4: A6a FUN_00467650, A6b FUN_00468980, A3 FUN_0046b540 (all C2, lane-ready).
Plus coverage gaps + steering calibration + standalone const back-port + AI runtime.

---

## WS-PHYS-C4-A6A — A6a FUN_00467650 to C4 (MAIN TREE). Pool: dev .asi + pool11
> Extend the lane to A6a (the big x87 float10 velocity/angular-integration body). Reuse the A5
> pattern (naked entry trampoline + OrigA6a early-return trampoline + live-LUT/RNG-forwarder +
> shared-global snapshot if it writes any + in-process bit self-test). Resolve [U-A6A-ST0]: capture
> the live ST0 feeding FUN_004a2c48=ROUND at the call. TU is x87 (no /arch:SSE2). Bit-diff the
> fields A6a writes; fix any exact-bit const / float-reinterpret bug (cf. A5). C4 via re-classify
> (no overclaim; document unhit branches).

## WS-PHYS-C4-A6B + A3 — A6b FUN_00468980 + A3 FUN_0046b540 to C4 (MAIN TREE). Pool: dev .asi + pool11
> A6b: disassemble + forward the FUN_004a3384 (asin) + FUN_004c4d20 (device matrix, no-visible-args
> register ABI) callees to the live originals; bit-diff the rotation-apply. A3: spawn-time init (off
> hot path) — self-test the record init (mass +0x50/+0x54/+0x58, wheel geometry, handling table
> 0x00613140 walk). C4 each via re-classify. After these, ALL 5 physics chain fns are C4.

## WS-PHYS-COVERAGE — close the A4/A5 unhit branches (MAIN TREE). Pool: dev .asi + Frida
> A4 (brake in1!=0, input[5]>128 else) + A5 (airborne grounded!=4.0, random-surface RNG key
> 0xffff32ff) were transcribed-not-exercised in a forward all-grounded Arctic race. Drive a scenario
> that hits them: brake input + a jump/ramp (airborne) + a random-surface tile, with the installed-
> hook self-test live; bit-confirm those branches. Upgrade the C4 evidence (remove the coverage
> caveats from hooks.csv) or fix any divergence the new branches expose.

## WS-PHYS-SMOKE-STEER — calibrate steering + back-port exact consts (MAIN TREE). Pool: standalone + pool11
> Run mashed_re.exe MASHED_REAL_PHYSICS=1 with steer input: confirm heading follows input (not a
> circle), verify/flip the steer sign, calibrate kYawScale to the original turn radius. ALSO back-
> port the exact-bit constants the C4 A/B found into the STANDALONE bodies for bit-fidelity:
> VehicleControl.cpp kGripMul -> 0x392ec33e; ForceIntegrator.cpp _DAT_005cc990 -> 0x3727c5ac,
> _DAT_005ccd08 -> 3000.0/0x453b8000 (+ audit the other standalone consts vs the PhysicsChainHooks
> Cf(0x..) exact set). Build + commit.

## WS-AI-VERIFY — AI runtime (MAIN TREE). Pool: standalone run
> mashed_re.exe MASHED_REAL_AI=1 + a race (LUT-safe, steering real): do opponents follow the AI%d.AI
> line? Resolve [U-C-STEER-MAG]/[U-C-RATE0/1]; fix ctrl->motion. Screenshot/log opponent paths.

## WS-D-VISUAL / WS-J2 / WS-E-POLISH — tails
> D-VISUAL: powerup visuals on the spike. J2: collision FX from live contacts. E-POLISH: vehicle
> lighting (ledger #9) + HUD on the spike.

---
## Recommended wave-11 (MAIN tree, serialized on the booted process)
**11a:** WS-PHYS-C4-A6A -> WS-PHYS-C4-A6B+A3  (=> all 5 physics fns C4).
**11b:** WS-PHYS-COVERAGE (close A4/A5 branch caveats) + WS-PHYS-SMOKE-STEER (+ const back-port) +
WS-AI-VERIFY. Then tails. GOAL: the entire physics chain bit-identical-C4 + a controllable, AI-
populated real-physics race — physics provably the game, every function verified.
