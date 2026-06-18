# Session prompts — wave 14 (playable-race polish; C4 holdouts documented). Paste one per session.

Generated 2026-06-17. The physics C4 push hit its hard tail: **3/5 chain fns bit-identical C4
(A3, A4-broad-coverage, A5)**. The last two are genuinely gated — A6a = 93/96 (x87 float10
suspension residual -> naked-asm shim), A6b + A5/A6a airborne branches = env-blocked (Quick-Battle
Arctic is flat; need a ramp/jump track via menu-nav RE). So wave-14 PROMOTES the playable-race
polish (already queued) to the lead — it's tractable + user-visible — and keeps the two C4
holdouts as documented lower-priority follow-ups.

COMMON PREAMBLE: re/SESSION_PROMPTS.md. Rules: Frida/boot/standalone-runtime = MAIN tree;
analyzed Ghidra (pool11/12/6, NOT pool4); subagent=`general-purpose`, self-contained; NO-GUESSING;
commit nanofives+trailer; guard original/; in-process self-test; COMMIT INCREMENTALLY.
**MASHED PROCESS HYGIENE (MANDATORY — CLAUDE.md): MASHED is multi-instance; track the PID you spawn
and kill ONLY that PID (Stop-Process -Id <pid>); NEVER blanket-kill by name (Stop-Process -Name /
taskkill /im); NEVER auto-attach to first-MASHED-by-name; if you find an orphan MASHED you can't
confirm you own, LEAVE IT.** (A blanket-kill destroyed another session's TTD capture 2026-06-17.)

---

## WS-PHYS-SMOKE-STEER + CONST-BACKPORT — playable real-physics race + standalone bit-fidelity (LEAD, MAIN TREE). Pool: standalone + pool11
> Run the STANDALONE mashed_re.exe with MASHED_REAL_PHYSICS=1 + steer input (track your spawned PID;
> kill only it). Confirm: heading follows steer input (not a fixed circle), verify/flip the steer
> SIGN, calibrate kYawScale to the original turn radius (capture an original turn-rate reference if
> feasible), brake works, the car suspends + stays on track, stable. Screenshot + pos/vel log; ON
> vs OFF. THEN back-port the C4-found exact-bit constants into the STANDALONE bodies (the .asi impl
> already has them via Cf(0x..)): VehicleControl.cpp kGripMul 0x392ec33e; ForceIntegrator.cpp
> _DAT_005cc990 0x3727c5ac + _DAT_005ccd08 0x453b8000; Integrate2.cpp the 15 A6a consts (full list:
> re/analysis/phys_c4_evidence/A6a_FINDINGS_2026-06-17.md); AeroStabilize.cpp _DAT_005ccae0=double
> 180/pi + _DAT_005ccad8=double 90.0. Build green; commit.

## WS-AI-VERIFY — AI runtime in the standalone (MAIN TREE). Pool: standalone run
> Run STANDALONE mashed_re.exe MASHED_REAL_AI=1 + a race (PID-scoped hygiene): do opponents follow
> the AI%d.AI race line (not the gate-ribbon)? Resolve [U-C-STEER-MAG]/[U-C-RATE0/1]; fix the
> ctrl->motion mapping/steer-sign. Screenshot/log opponent paths vs the spline. (A real-AI race is
> also the natural driver for the airborne-coverage holdout if it ever reaches a jump.)

## WS-D-VISUAL — power-up visuals on the spike (MAIN TREE/worktree). Pool: pool5
> De-stub Gun_Deact/Drum_Deact + the powerup visual spawn/teardown against the spike's D3d9 quad/
> billboard layer (renderer = spike, ratified). Diff vs original behavior. No longer blocked on RW.

## WS-J2 — collision FX (worktree/standalone). Pool: none
> Wire permdict collision FX (skids/impacts) to the now-live contact events. Verify FX per impact.

---
## C4 HOLDOUTS (documented, lower-priority — pick up when the tractable polish is done)
## WS-PHYS-C4-A6A-FINISH — naked-asm float10 suspension shim (MAIN TREE). Pool: dev .asi + pool11
> Close A6a's 3 RED (1-8 ULP, per-wheel susp-force X, x87 float10 across compute+store @
> 0x468127..0x468337) with a naked-asm shim matching the exact x87 register lifetime. HIGH
> regression risk: isolate + re-run A3/A4/A5 self-tests after. Or accept A6a faithful-C2 @ 93/96 + stop.

## WS-PHYS-AIRBORNE-COVERAGE — ramp-track scenario for the airborne branches (MAIN TREE). Pool: dev .asi + Frida + menu-RE
> Quick-Battle Arctic is flat. RE the menu-nav path to a circuit/ramp track + a per-track jump map so
> a canonical run goes airborne, then exercise + bit-confirm A5 airborne/random-surface, A6a airborne,
> and A6b's aero body -> potentially A6b C4 + remove the A5/A6a airborne caveats. Substantial menu-RE.

---
## Recommended wave-14
**14a (lead, user-visible):** WS-PHYS-SMOKE-STEER+CONST-BACKPORT -> WS-AI-VERIFY (a calibrated,
AI-populated, bit-faithful real-physics race in the standalone). **14b:** WS-D-VISUAL + WS-J2
(gameplay tails). **Holdouts (opportunistic):** A6a-FINISH, AIRBORNE-COVERAGE. GOAL: a polished,
controllable, AI-populated real-physics standalone race — the playable payoff — with the physics
chain 3/5 C4 (the 2 holdouts characterized + scheduled, not overclaimed).
