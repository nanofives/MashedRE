# Session prompts — wave 15 (runtime polish + C4 holdouts). Paste one per session.

Generated 2026-06-17 on isolated branch `ws-plan-wave15` (off main 6d5abb40), authored while
the main tree was under the user's active management (TTD-lane integration on ws-d-visual).
This is the FORMAL wave-15 plan. COMMON PREAMBLE: re/SESSION_PROMPTS.md.

STATUS GOING IN: physics chain 3/5 bit-identical C4 (A3, A4, A5) via the .asi verbatim-LUT
installed-hook lane (Vehicle/PhysicsChainHooks.cpp). The standalone DRIVES + STEERS a real-
physics race (MASHED_REAL_PHYSICS). Renderer = Track::World spike (ratified). Powerup visuals
already render on the spike (PowerupBackendImpl). Wave-14 recovered work: the exact-bit CONST
BACK-PORT is on branch `ws-phys-smoke-steer` (commit d753e701) awaiting integration.

RULES (binding): Frida/boot/standalone-runtime = MAIN tree only (original/ is gitignored ->
absent in worktrees); analyzed Ghidra clones (pool11/12/6, NOT pool4); subagent type =
`general-purpose` (fork removed), self-contained prompts; NO-GUESSING; commit nanofives +
trailer; guard original/; only DATA faults matter; in-process self-test (not per-frame
Interceptor); COMMIT INCREMENTALLY.
**MASHED/standalone PROCESS HYGIENE (MANDATORY — CLAUDE.md "MASHED process hygiene (multi-
session)"): game processes are MULTI-INSTANCE; other sessions run their own. Track the PID you
spawn and kill ONLY that PID (Stop-Process -Id <pid>); NEVER blanket-kill by name (Stop-Process
-Name / taskkill /im); NEVER auto-attach to first-by-name; if you find an orphan game process
you can't confirm you spawned, LEAVE IT.** (A blanket-kill destroyed another session's TTD
capture 2026-06-17.)

PREREQ: wave-15a needs the MAIN tree free (the user's ws-d-visual TTD integration landed + a
neutral checkout). Wave-15a items are runtime; do not start them from a worktree.

---

## WS-PHYS-SMOKE-STEER-RUNTIME — finish the steer-calibration (MAIN TREE). Pool: standalone run
> The const back-port (d753e701) is done; this is the runtime HALF that didn't complete. Run the
> standalone mashed_re.exe with MASHED_REAL_PHYSICS=1 + steer input (track + kill ONLY your spawned
> PID). Confirm: heading FOLLOWS the steer input (turns left/right, not a fixed circle), straight
> input = no yaw drift, radius tightens with steer magnitude. Verify/flip the steer SIGN
> (+steer->input[0] vs [1]). Calibrate kYawScale so the turn radius matches the original (the gain
> lives in the RW-Physics integrator [U-A8-YAWSCALE]; runtime-tune it). Screenshot + pos/vel/yaw
> log; ON vs OFF. Commit the chosen kYawScale on a fresh branch.

## WS-AI-VERIFY — AI runtime in the standalone (MAIN TREE). Pool: standalone run
> Run standalone mashed_re.exe MASHED_REAL_AI=1 + a race (PID-scoped hygiene): do opponents follow
> the AI%d.AI race line (not the gate-ribbon)? Resolve [U-C-STEER-MAG]/[U-C-RATE0/1]; fix the
> ctrl->motion mapping/steer-sign. Screenshot/log opponent paths vs the spline.

---
## C4 HOLDOUTS (documented; opportunistic; need the booted .asi on the MAIN tree)
## WS-PHYS-C4-A6A-FINISH — naked-asm float10 suspension shim (MAIN TREE). Pool: dev .asi + pool11
> Close A6a's 3 RED (1-8 ULP, per-wheel susp-force X, x87 float10 across compute+store @
> 0x468127..0x468337) with a naked-asm shim matching the exact x87 register lifetime (the
> GearCvtCompute/DriveForceAccum shims in PhysicsChainHooks.cpp are the pattern). Re-run to 96/96
> -> A6a C4. HIGH regression risk: isolate + re-run A3/A4/A5 self-tests after. Or accept A6a
> faithful-C2 @ 93/96 + stop (sound off-ramp).

## WS-PHYS-AIRBORNE-COVERAGE — ramp-track scenario for the airborne branches (MAIN TREE). Pool: dev .asi + Frida + menu-RE
> Quick-Battle Arctic is geometrically flat -> A5 airborne/random-surface, A6a airborne, A6b's aero
> body never run. RE the menu-nav path to a circuit/ramp track + a per-track jump map so a canonical
> run goes airborne; then exercise + bit-confirm those branches -> potentially A6b C4 + remove the
> A5/A6a airborne caveats. Substantial menu-RE; a real-AI race may help (opponents jump).

---
## STANDALONE BIT-FIDELITY note (from wave-14 const back-port d753e701, ws-phys-smoke-steer)
The .asi C4 lane found the standalone bodies carried mis-rounding decimal literals; d753e701
back-ports the exact-bit consts (VehicleControl kGripMul 0x392ec33e; ForceIntegrator 005cc990
0x3727c5ac / 005ccd08 0x453b8000; Integrate2 the 15 A6a consts; AeroStabilize 005ccae0=180/pi,
005ccad8=90.0). Integrate that branch so the STANDALONE physics is bit-faithful too (A4 has no
transcendental -> the exact consts make the standalone A4 bit-identical).

---
## Recommended wave-15
**15a (MAIN tree, after the user frees it):** WS-PHYS-SMOKE-STEER-RUNTIME -> WS-AI-VERIFY (=> a
calibrated, AI-populated, controllable real-physics standalone race — the playable payoff).
**15b (opportunistic, booted .asi):** WS-PHYS-C4-A6A-FINISH; WS-PHYS-AIRBORNE-COVERAGE.
GOAL: the playable real-physics + real-AI race confirmed at runtime, with the 2 C4 holdouts
either closed or formally accepted as faithful-C2.
