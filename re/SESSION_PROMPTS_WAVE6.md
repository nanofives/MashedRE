# Session prompts — wave 6 (VERIFY + RUNTIME pivot). Paste one per session.

Generated 2026-06-17 after wave-5a + two RATIFIED user decisions:
- **Renderer = the `Track::World` spike** is the shipping standalone world renderer. The
  verbatim RW D3D9 driver (B-FULL) is **RETIRED for the standalone** (RW-engine-bound +
  redundant with the spike); the ported RW pipeline (RwWorldLoad/RwWorldRender/RwWorldStream)
  is reserved for the `.asi`/diff context. Record in DEFERRED.md (re-classify).
- **Wave-6 = pay down the verification/runtime debt** (5 waves of build-green, gated-OFF,
  UNVERIFIED code). No new breadth. Make `MASHED_REAL_PHYSICS`/`MASHED_REAL_AI` actually work
  + be behaviorally verified.

COMMON PREAMBLE: re/SESSION_PROMPTS.md. Rules still binding: **Frida/boot/standalone-runtime =
MAIN tree only**; **analyzed Ghidra clones** (pool6/11; not pool4). NO-GUESSING; commit as
nanofives + trailer; guard `original/`.

DEP ORDER: WS-A8-GAPS (worktree, code) → then MAIN-tree: WS-A-VERIFY-3 ‖ WS-PHYS-SMOKE,
WS-AI-VERIFY. Tails: WS-D-VISUAL, WS-J2, WS-E-POLISH.

---

## WS-A8-GAPS — close the physics runtime gaps. Pool: Mashed_pool11 (worktree, code)
> Close the four documented A8 gaps so `MASHED_REAL_PHYSICS` produces real driving:
> - **[U-A8-STEER]** the steer input byte is unmapped (A4 reads accel/brake only). RE where
>   steering enters the per-wheel sim (wheel steer-angle +0x3c via the control path / the
>   input descriptor slot the original uses) and wire it into VehicleControl/the dispatcher.
> - **[U-A8-GRAVITY]** gravity globals `_DAT_00803334/38/3c` (+ `_DAT_00803340` scale) are 0 —
>   RE the setter upstream of `FUN_00470c70` and set g_gravX/Y/Z/g_gravScale so cars fall/settle.
> - **terrain contacts** — `g_worldTris` is unset → 0 ground contacts → no suspension. Feed the
>   track world triangles (from `Track::World`) into `Collision::WheelContactSolver`.
> - **[U-A8-SUBSTEP]** replicate the original's ≤50-substep-per-frame loop (FUN_00470c70 chunks)
>   instead of 1 integrate/frame.
> Build green (worktree). The runtime smoke is WS-PHYS-SMOKE (main tree).

## WS-A-VERIFY-3 — physics telemetry C4 lane (MAIN TREE). Pool: dev .asi + Frida
> The real C4 gate (run_diff can't — WSA_VERIFY2 doc). In the dev .asi: ABI shims for the
> register-based originals (A4 EAX=record, A5 EDI=record…), bind our 10 globals to the original
> live `_DAT` each frame, scenario-attach a canonical race with RH_ScopedInstall LIVE, per-frame
> diff the 0xd04 record fields (vel +0x9b0, fwd +0x9d4, ang-vel +0x9bc, torque +0x1a8/+0x26c),
> **hot-path-sampled one fn at a time** (poll_attach_catch_crash for crashes). Resolve
> [U-A6A-FLOAT10]. Promote GREEN fns to C4 via re-classify (inline-JMP live; no overclaim).
> Model on the camera/scoring scenario-attach lane. Run AFTER A8-GAPS (chain complete-enough).

## WS-PHYS-SMOKE — physics runtime smoke (MAIN TREE). Pool: standalone run
> Run `mashed_re.exe` with `MASHED_REAL_PHYSICS=1` + a race (MASHED_RACE_DEMO/PLAY knobs);
> confirm the player car accelerates / brakes / **steers** / falls + settles on suspension /
> stays on track. Screenshot + log per-frame pos/vel. Compare to the scaffold. Report exactly
> what's realistic vs broken (feeds back into A8-GAPS + VERIFY-3). Kill the process after.

## WS-AI-VERIFY — AI runtime + fix approximations (MAIN TREE). Pool: standalone run
> Run `mashed_re.exe` with `MASHED_REAL_AI=1` + a race; do opponents follow the AI%d.AI race
> line (not the gate-ribbon)? Resolve [U-C-STEER-MAG] (exact ROUND(ST0) steer magnitude/ESP
> slots) + [U-C-RATE0/1] (the two `FUN_0046d6a0/6d0` rate floats I substituted) by observation;
> fix the ctrl→motion mapping/steer-sign. Screenshot/log opponent paths vs the spline.

## WS-D-VISUAL — power-up visuals on the spike. Pool: Mashed_pool5
> Renderer = spike (ratified), so de-stub Gun_Deact/Drum_Deact + the powerup visual spawn/
> teardown against the **spike's** scene (D3d9Render quad/billboard layer), NOT the RW graph.
> No longer blocked on WS-E. Diff vs original behavior.

## WS-J2 — collision FX. Pool: none (after WS-A8-GAPS terrain contacts live)
> Wire permdict collision FX (skids/impacts) to the now-live contact events. Verify FX per impact.

## WS-E-POLISH — spike renderer polish (LOW priority, after verify). Pool: Mashed_pool6
> On the accepted spike: vehicle lighting (ledger #9) + HUD/Im2D fidelity + a parity sweep vs
> original. The ported RW pipeline stays `.asi`-only. NOT the RW driver.

---

## Recommended wave-6a (worktree) then 6b (MAIN tree, serial)
**6a:** WS-A8-GAPS (the one code/RE session — makes the smoke meaningful).
**6b (main tree, serialized on the booted process):** WS-A-VERIFY-3 (telemetry C4) →
WS-PHYS-SMOKE → WS-AI-VERIFY. Then WS-D-VISUAL / WS-J2 / WS-E-POLISH (worktree-able).
GOAL OF WAVE 6: `MASHED_REAL_PHYSICS` + `MASHED_REAL_AI` produce a real, on-track race that is
behaviorally diffed against the original — i.e. the standalone is provably the game, not just
build-green scaffolding.
