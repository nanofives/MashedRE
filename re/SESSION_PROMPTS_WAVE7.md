# Session prompts — wave 7 (FIX THE CRASH, then verify the race). Paste one per session.

Generated 2026-06-17 after WS-PHYS-SMOKE caught a runtime AV. Continues the verify+runtime
pivot (renderer = Track::World spike, ratified). COMMON PREAMBLE: re/SESSION_PROMPTS.md.
Rules: **Frida/boot/standalone-runtime = MAIN tree only**; **analyzed Ghidra clones** (pool6/11).
NO-GUESSING; commit as nanofives + trailer; guard `original/`.

STATE: physics chain ported + wired behind `MASHED_REAL_PHYSICS` (substep + live suspension +
gravity-vestigial). **It AV-crashes ~2-3s into a race** (eip=0x0005ad26, READ near-null 0x2000,
eax=0 → bad/uninitialized fn-ptr or null-base deref). Scaffold (toggle OFF) is CLEAN and drives.
Steer still kinematic ([U-A8-STEER]). 0 physics fns C4. AI bridge wired but never runtime-tested.
Detail: re/analysis/PHYS_SMOKE_2026-06-17.md.

DEP ORDER: WS-PHYS-CRASH-FIX (lead, unblocks everything) → WS-PHYS-SMOKE-2 → {WS-A-VERIFY-3,
WS-AI-VERIFY} ; WS-A8-STEER (worktree, parallel-able) ; tails D-VISUAL/J2/E-POLISH.

---

## WS-PHYS-CRASH-FIX — fix the MASHED_REAL_PHYSICS AV (LEAD, MAIN TREE). Pool: standalone + poll_attach
> Repro the crash: run mashed_re.exe with MASHED_REAL_PHYSICS=1 + a race demo under
> `re/frida/poll_attach_catch_crash.py` to capture the **caller chain** of eip=0x0005ad26 (the
> WS-PHYS-SMOKE dump had no stack walk). The fault = READ near-null 0x2000 with eax=0 — a
> null-base + 0x2000 field deref or a call through a zeroed function pointer. Chase the prime
> suspects: (a) an inert dep still invoked — grep the physics chain for any fn-ptr / device
> thunk that's null in the standalone (RwMatrixRotate mode-1 device matmul in A6b is
> standalone-INERT → A6b's rotation-apply may call into a null RW device; VehicleControl/A5/A6a
> Rw_* should now bind to RwV3dTransformPointsCPU — verify they actually do, not the stub);
> (b) `g_vehicleArrayBase`/the 16×0xd04 record array binding (is it allocated + sized before
> the dispatcher runs?); (c) `Collision::g_worldTris`/`g_worldTriCount` from VehiclePhysics_SetWorld
> (null/missized soup → solver walks garbage). Fix the root cause; gate any still-unsafe path.
> Re-run: the race must survive past the throttle ramp. Commit. This unblocks all physics runtime/C4.

## WS-PHYS-SMOKE-2 — re-run the physics smoke (MAIN TREE). Pool: standalone run
> After the fix: MASHED_REAL_PHYSICS=1 race, 20-30s, screenshot + pos/vel log. Confirm the car
> accelerates / brakes / suspends / stays on track / is stable; document remaining gaps (steer
> kinematic). ON-vs-OFF comparison. This is the gate for declaring physics "runs".

## WS-A-VERIFY-3 — physics telemetry C4 lane (MAIN TREE). Pool: dev .asi + Frida
> (Only after the crash fix.) The scenario-attach canonical-race telemetry lane (run_diff can't —
> WSA_VERIFY2 doc): ABI shims for the register-based originals, bind our 10 globals to the original
> live `_DAT`, RH_ScopedInstall LIVE, per-frame 0xd04 record-field diff (vel+0x9b0/fwd+0x9d4/
> angvel+0x9bc/torque+0x1a8/+0x26c), hot-path-sampled one fn at a time. Resolve [U-A6A-FLOAT10].
> Promote GREEN fns to C4 via re-classify (no synthetic-bypass overclaim).

## WS-AI-VERIFY — AI runtime + crash-check (MAIN TREE). Pool: standalone run
> Run mashed_re.exe MASHED_REAL_AI=1 + a race. Does it crash (same class as physics?) and do
> opponents follow the AI%d.AI line (not the gate-ribbon)? Resolve [U-C-STEER-MAG]/[U-C-RATE0/1];
> fix the ctrl→motion mapping/steer-sign. Screenshot/log opponent paths vs spline.

## WS-A8-STEER — map steering into the physics chain (worktree). Pool: Mashed_pool11
> The remaining A8 gap: FUN_00470670 consumes input[0]=accel/[1]=brake/[5]=gate only; the steer
> field in the input descriptor (&DAT_007f1038 + map*0x13) → wheel steer-angle (+0x3c) path is
> unmapped (car currently steers kinematically). RE the descriptor steer field + how it drives
> the wheel steer angle / angular control, and wire it into the chain so physics-steering is real.
> Build green. (Runtime-verify in a follow-up PHYS-SMOKE.)

## WS-D-VISUAL / WS-J2 / WS-E-POLISH — tails (as wave-6 spec)
> D-VISUAL: powerup visuals on the spike. J2: collision FX from the now-live contacts. E-POLISH:
> vehicle lighting (ledger #9) + HUD on the spike. After the physics race is stable + verified.

---
## Recommended wave-7
**7a (MAIN tree, lead):** WS-PHYS-CRASH-FIX → WS-PHYS-SMOKE-2 (these gate everything).
**7b (parallel once stable):** WS-A-VERIFY-3 + WS-AI-VERIFY (main tree, serialized on the booted
process) ‖ WS-A8-STEER (worktree). Then tails. GOAL: a crash-free, on-track, real-physics +
real-AI race, with the chain's C4 debt closing fn-by-fn.
