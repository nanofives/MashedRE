# Session prompts — wave 16 (clean sustained-drive harness → stabilize → calibrate). Paste one per session.

Generated 2026-06-18 after wave-15a's first inline runtime drive. FINDING (re/analysis/
STEER_CALIB_RUNTIME_2026-06-18.md): the standalone runs real physics CRASH-FREE (exit 0), but in
a RACE_DEMO+PLAY_DEMO run the car's yaw was flat (1.550) and speed erratic (0/260/0/56/0) — no
clean sustained drive, so steering/kYawScale can't be calibrated yet. Root: no "enter InRace +
hold + scripted-drive" harness (RACE_DEMO captures + Escs at 6.6s); possible drive instability.

COMMON PREAMBLE: re/SESSION_PROMPTS.md. Rules: Frida/boot/standalone-runtime = MAIN tree only;
analyzed Ghidra (pool11/12/6, NOT pool4); subagent=`general-purpose`, self-contained; NO-GUESSING;
commit nanofives+trailer; guard original/. **MASHED/standalone PROCESS HYGIENE (CLAUDE.md):
game procs are multi-instance; track + kill ONLY your spawned PID (Stop-Process -Id <pid>); NEVER
blanket-kill by name; NEVER attach/target by name; leave orphans you can't confirm you own.**

---

## WS-PHYS-DRIVE-HARNESS — clean sustained-drive demo (MAIN TREE). Pool: standalone run
> Add a clean sustained-drive demo mode (new env knob e.g. MASHED_DRIVE_HOLD=1, OR extend PLAY_DEMO):
> enter InRace once (GameFlow_RequestRace, like RunRaceDemoStep case 0->1) and HOLD it (no capture
> phases, no Esc) for ~20-30s while a scripted input drives — straight accel first, then a known
> steer ramp (e.g. di.steer steps 0 -> +0.5 -> +1 -> -1), logging car_yaw/pos/speed each 0.25s to
> mashed_re.log. Do NOT set MASHED_RACE_DEMO (it perturbs the drive). Build; run PID-scoped; commit
> the harness on a fresh branch.

## WS-PHYS-DRIVE-STABILIZE — confirm/repair the real-physics drive (MAIN TREE). Pool: standalone + pool11
> With the clean harness: confirm a STABLE speed ramp under straight accel (MASHED_REAL_PHYSICS=1).
> If speed is erratic (the wave-15a 0/260/0 pattern persists OUTSIDE the RACE_DEMO conflation),
> root-cause it: candidates = the A8-GAPS substep loop (n=clamp(round(dt/1ms),1,50) — dt units?),
> the per-frame global rebind (g_suspScale from the dispatcher), the contact-soup feed
> (VehiclePhysics_SetWorld g_worldTris), or a d753e701 const back-port regression (diff vs the
> .asi C4 values). Compare to MASHED_REAL_PHYSICS=0 (scaffold, known-smooth 0→14.9). Fix; re-confirm.

## WS-PHYS-STEER-CALIB — calibrate kYawScale + steer sign (MAIN TREE). Pool: standalone run
> Once the drive is stable: observe car_yaw vs the scripted di.steer. Confirm heading follows input
> (turns, not a circle), straight=no drift. Verify/flip steer sign (+steer -> input[0] vs [1]).
> Calibrate kYawScale (VehiclePhysicsRun.cpp:241, now 1.0) so the turn radius matches the original
> (capture an original-side turn-rate ref via the .asi if feasible). Commit the value + sign on a
> fresh branch. THEN WS-AI-VERIFY (MASHED_REAL_AI race: opponents follow the AI line?).

## (carried) C4 holdouts + integration
> WS-PHYS-C4-A6A-FINISH (naked-asm float10 suspension shim); WS-PHYS-AIRBORNE-COVERAGE (ramp-track
> menu-RE); integrate the d753e701 const back-port (ws-phys-smoke-steer) so the standalone is bit-
> faithful. tails: WS-J2 collision FX.

---
## Recommended wave-16 (MAIN tree, serialized)
**16a:** WS-PHYS-DRIVE-HARNESS -> WS-PHYS-DRIVE-STABILIZE (the drive must be smooth before anything
else is measurable). **16b:** WS-PHYS-STEER-CALIB -> WS-AI-VERIFY. GOAL: a stable, controllable
real-physics standalone drive, then steering calibrated, then AI confirmed — the playable payoff.
NOTE: this is focused single-track runtime work (one booted process at a time); not a fanout.
