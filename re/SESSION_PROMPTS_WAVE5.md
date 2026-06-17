# Session prompts — wave 5. Paste one per session.

Generated 2026-06-17 after wave-4a (physics chain COMPLETE + device-transform; RW stream-parse
live; AI bridge linked — all merged to main, build GREEN) and WS-A-VERIFY-2. Use the COMMON
PREAMBLE from `re/SESSION_PROMPTS.md`. NO-GUESSING; anchor SHA-256
BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E; commit as nanofives + trailer;
guard `original/`.

### ORCHESTRATION RULES (still binding)
1. **Frida / boot-original / standalone-runtime sessions MUST run in the MAIN tree**, not a
   worktree (`original/` is gitignored). RE/port/coding sessions CAN use worktrees.
2. **Use ANALYZED Ghidra clones** — verify `function_list` non-empty; pool4 is unanalyzed; if
   your assigned slot is empty, `bash scripts/ghidra_pool.sh acquire` a fresh clone.

### KEY FINDING from WS-A-VERIFY-2 (drives this wave)
The generic `run_diff.py` lane (hook-BYPASSED, scalar vectors, path1=C3) **cannot** verify the
physics chain: A3/A4/A5/A6a/A6b operate on a live 0xd04 record + ~10 globals, fire >1000/s
(Interceptor destabilizes in ~6s), and call RW *device* ops. Same class as RaceCamera/scoring
(audit §H1). Their C4 needs a **scenario-attach canonical-race telemetry lane** (below), not
run_diff. The chain builds + the device-transform self-tests pass, but **0/5 are behaviorally
verified** — `MASHED_REAL_PHYSICS` stays OFF until the telemetry lane + A8 runtime smoke land.

DEP ORDER: WS-A8 (wire, makes it RUN) ‖ WS-A-VERIFY-3 (proves bit-identity) — independent
concerns. E-DEVICE-2 → first RW pixels. C-AITREE → AI-VERIFY. D-VISUAL/J2 are tails.

---

## WS-A-VERIFY-3 — physics telemetry C4 lane (MAIN TREE). Pool: dev .asi + Frida
> Build the real C4 harness for the stateful hot-path physics fns (run_diff can't — see the
> finding above). In the dev .asi: (1) ABI shims for the register-based originals (A4 EAX=record,
> A5 EDI=record, etc.); (2) bind our 10 stubbed globals to the original live `_DAT` addresses
> each frame (g_suspScale=_DAT_0088e5f0, gravity, g_torqueRingPhase=DAT_007f101c, ...);
> (3) a **scenario-attach canonical race** (statenav into a live race — see
> [[project-mass-canonical-observation]] caveats) with RH_ScopedInstall LIVE; (4) per-frame
> diff of the 0xd04 record fields (vel +0x9b0, fwd +0x9d4, ang-vel +0x9bc, torque +0x1a8/+0x26c)
> original-vs-reimpl, **hot-path-sampled ONE fn at a time, short runs** (poll_attach_catch_crash
> for crashes). Resolve [U-A6A-FLOAT10] (float10→double divergence) from the telemetry. Promote
> GREEN fns to C4 via re-classify (inline-JMP live; NO synthetic-bypass overclaim). This is the
> WS-H2-class gate; expect iteration. Model the lane on the camera/scoring scenario-attach work.

## WS-A8 + WS-B-WIRE — wire physics so it RUNS in the standalone. Pool: Mashed_pool2 (+pool3)
> Independent of VERIFY-3 (this makes it RUN; VERIFY-3 proves it's bit-identical). Add Vehicle/*.cpp
> to the exe source list (link closure: Collision::g_suspScratch, Math/, globals). At car spawn call
> VehicleInit(slot,trackType); per frame replicate the FUN_00470c70 dispatcher — IT computes the
> real globals (_DAT_0088e610=dt*_DAT_005cea80, _DAT_0088e5f0=_DAT_005ccd08/_DAT_0088e610,
> g_torqueRingPhase) so set OUR globals from those (no longer 0). Feed B2/B3/B4 contacts into the
> record (DAT_008815a4 + per-wheel buffers; qhull off the per-frame path). Map standalone car
> (AiCar pos/vel/yaw) ↔ record; drive the rendered car from the record. Gate `MASHED_REAL_PHYSICS`
> (default OFF). **Runtime smoke test from the MAIN tree** (toggle ON: does the player car
> accelerate/steer/brake plausibly, stay on the track?). Then WS-J2 (collision FX) once contacts live.

## WS-E-DEVICE-2 — RW device submit → first real pixels. Pool: Mashed_pool6
> RwWorldLoad now PARSES rwID_WORLD in memory (wave-4a stream half). Finish the draw: port the
> device submit — RpWorldInstance device VB/IB build, the RxPipeline node bodies FUN_004eb3d0/
> FUN_004ea6d0, DrawIndexedPrimitive (0x004e1007) — and stand up the minimal RW device state the
> standalone needs (RwEngineOpen path / the RW device table DAT_007d3ff8 binding, or a thin D3D9
> adapter). Wire: feed a track BSP through RwWorldLoad_FromBytes → RwWorldRender_SetEngine. With
> `MASHED_RW_RENDER` ON (MAIN tree), render a track's real geometry; close [U-E2-HDR]/[U-E2-XCRIPT]
> by diffing the parsed RpWorld vs the standalone's byte-faithful Track::World. Screenshot-parity.
> Then E3 (RpWorld + vehicle lighting, ledger #9), E4 (Im2D HUD/menu), E5 (ship; spike → dev-only).

## WS-C-AITREE + AI-VERIFY — behaviour tree + prove the AI drives. Pool: Mashed_pool8/11 (port) + Frida (verify)
> (Port, worktree:) port the still-stubbed FUN_00416250 targeting behaviour tree (modes 1..10,
> already decompiled) + helpers FUN_00414570/00415880/00414a70/00414c30/004150e0/00414f00/
> 004148b0/00415020/00416060/00415d00, variants FUN_00416a30/00417da0, rubber-band FUN_004177b0,
> powerup activation FUN_00415220, into Ai/AiStandalone — replacing the mode-0-only ControlStepMode0
> placeholder; close [U-C-BANDS] (the real ctrl steer/accel/brake bands). (Verify, MAIN tree:)
> with `MASHED_REAL_AI` ON, run a standalone race — do opponents follow the AI%d.AI race line
> (not the gate-ribbon)? Fix the ctrl→motion mapping/steer-sign approximations (turn rate 2.4 was
> a guess). Screenshot/log opponent paths.

## WS-D-VISUAL — power-up visual de-stub. Pool: Mashed_pool5 (after E pixels)
> De-stub Gun_Deact/Drum_Deact + the WS-B/RW path in Powerup/PowerupEffects.cpp; bind powerup
> visuals to the real RW scene graph (now that WS-E draws it). Diff vs original.

---

## Recommended wave-5a (parallel)
**WS-A8** (pool2, worktree — code; smoke test deferred to main) ‖ **WS-E-DEVICE-2** (pool6) ‖
**WS-C-AITREE** (pool8/11). Then **wave-5b (MAIN TREE, sequential):** **WS-A-VERIFY-3** (the
telemetry C4 lane) + the **A8 runtime smoke** + **AI-VERIFY** + **E-DEVICE-2 pixel test**; then
**WS-D-VISUAL**, **WS-J2**. After 5a+5b: physics RUNS + is being C4-verified, AI drives the real
line, the RW renderer puts real pixels on screen — the standalone is functionally the game,
with the verification lane closing the C4 debt fn-by-fn.
