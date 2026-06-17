# Session prompts — wave 3. Paste one per session.

Generated 2026-06-16 after wave-2a landed (WS-E1 traversal layer + WS-C standalone-AI
spine merged to main `befd2170`, build GREEN, both gated default-OFF). Use the COMMON
PREAMBLE from `re/SESSION_PROMPTS.md`. NO-GUESSING; anchor SHA-256
BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E; `diff-original` = C4;
commit as nanofives + Co-Authored-By trailer; Ghidra read-only via ghidra-pool
(pool0/1 locked); guard `original/`.

Read first: [[project-ports-done-convergence-pending]],
`re/analysis/physics_completion_track_2026-06-16.md`.

DEPENDENCY ORDER: **A-VERIFY → (A8 ↔ B-WIRE) → J2** ; **E2 → E-DRIVER → D-VISUAL** ;
**C-WIRE** independent. A-VERIFY is the gate — do it first.

---

## WS-A-VERIFY — diff-original C4 the physics chain (THE GATE). Pool: dev .asi + Frida
> Boot the original (build-26200 compat: `setup_mashed_compat.ps1` toggles EMULATEHEAP;
> [[project-emulateheap-boot-av]]). Register each ported physics fn in
> `re/frida/hooks_registry.py` (RVA, sig, test vectors) and stand up an installed-hook
> canonical-race lane: A4 `0x00470670`, A5 `0x0046ddb0`, A6a `0x00467650`, A6b
> `0x00468980`, A3 `0x0046b540`. For each, `RH_ScopedInstall` the original RVA, run a
> matched-input race, diff per-frame record fields (vel +0x9b0, fwd +0x9d4, ang-vel
> +0x9bc, torque +0x1a8/+0x26c). **RESOLVE the open [UNCERTAIN]s:** (a) `FUN_004c3df0`
> arg order (dst,matrix,count,src vs dst,src,count,mtx); (b) `FUN_004a3384` (=acos) /
> `FUN_004a2c48` ST0 input; (c) whether A6a's `float10` needs inline-`__asm`
> (RwMatrixRotateInner precedent); (d) the deferred A6a per-wheel contact-friction block
> + A6b rotation-apply. Fix the ports where they diverge. Promote each to C4 via
> `re-classify` ONLY with the inline-JMP live + canonical evidence (no synthetic-bypass
> overclaim — [[feedback-no-overclaiming-c-levels]]). Update the audit. **Output: which
> of A3/A4/A5/A6a/A6b are GREEN vs still-RED** — A8 wires only the GREEN chain.

## WS-A8 + WS-B-WIRE — wire physics + contacts into the standalone (coupled). Pool: Mashed_pool2 (+pool3)
> ONLY after A-VERIFY. Make `mashed_re.exe` run the verified chain. (1) Exe link closure:
> the EXE source list (`build.bat`) omits `Vehicle/*.cpp` — add ForceIntegrator/
> ForceIntegratorStubs/VehicleControl/Integrate2/AeroStabilize/VehicleInit + resolve deps
> (Collision::g_suspScratch @ DAT_00881560, Math/ leaves, globals). (2) Allocate the
> 16×0xd04 records; call `VehicleInit(slot,trackType)` at spawn (trackType from area
> Course_Id). (3) Replicate dispatcher `FUN_00470c70` (inits _DAT_0088e610/_DAT_0088e5f0,
> per-car input descriptor, g_torqueRingPhase); call `VehicleControlIntegrate` per car.
> (4) **B-WIRE**: wire the ported B2/B3 contact solvers + B4 wheel solver as the contact
> source filling DAT_008815a4 + per-wheel contact buffers the A5 integrator reads (qhull
> stays OFF the per-frame path, B1 gate). (5) Map standalone car (AiCar pos/vel/yaw) ↔
> record fields; drive the rendered car from the record. (6) Gate behind
> `MASHED_REAL_PHYSICS` (default OFF). Build green; smoke-race ON; diff vs original.

## WS-C-WIRE + remaining-AI — finish + wire the standalone AI. Pool: Mashed_pool4
> The WS-C spine (Ai/AiStandalone.cpp) is in the .asi (not exe-wired). (1) Reimplement the
> STUBBED callees from the in-file ledger: `FUN_00416250` (primary control-step behaviour
> tree, modes 1..10 + ctrl steer/accel/brake BANDS — closes [U-C-BANDS]), variants
> `FUN_00416a30`/`00417da0`, rubber-band `FUN_004177b0`, bank-timer/RNG `FUN_00417180`,
> powerup-brake `FUN_00417640` + activation `FUN_00415220`, targeting/LOS helpers
> (`FUN_00414570/00415880/00414a70/00414c30/004150e0/00414f00/004148b0/00415020/00416060/
> 00415d00`), `FUN_00443300` interp + `FUN_00443dc0` curvature/wall-march tail, and the
> `.AI` parser. (2) **Resolve [U-C-STEER]**: `FUN_00415e20` consts `_DAT_005cd0c8`/`d0`/
> `_DAT_005cc970` read as 0/0/2⁻⁶³ (degenerate) — confirm/correct via diff-original.
> (3) Bind `Ai::Host` to the standalone race state; add `Ai/AiStandalone.cpp` to the EXE
> source list; wire into TrackRenderer behind `MASHED_REAL_AI` (default OFF), replacing
> the gate-ribbon scaffold. Diff AI positions over N frames on a matched seed. Read
> `re/analysis/ai_controller.md`. Large — phase the callees.

## WS-E2 — bind a real world source + materials (first real geometry). Pool: Mashed_pool6
> RwWorldRender (E1) is INERT until `RwWorldRender_SetEngine()` binds a real RwGlobals +
> RpWorld. (1) Port/finish the BSP→RpWorld world loader (the deferred E1 world source) so
> a track's GRAPH*.BSP populates an RpWorld the traversal layer walks. (2) Port the RW
> material system + multi-TXD binding; resolve the E1 [UNCERTAIN]s: `RpAtomicGetWorld-
> BoundingSphere` atomic offset, `FUN_004f0900` device-cull predicate. (3) With
> `MASHED_RW_RENDER` ON, render a track's real geometry through the RW path; screenshot-
> parity vs original (textured). Depends: WS-E-DRIVER for actual pixels.

## WS-E-DRIVER — the RW D3D9 driver submit (the E1 deferred tail). Pool: Mashed_pool6
> Port the RxPipeline **node graph** → the RW D3D9 driver's `DrawIndexedPrimitive` submit
> (the part `RxPipelineExecute` 0x004d40d0 drives). This is what turns the traversal-layer
> shells into actual draw calls. B-FULL ratified. Parity a single textured atomic vs
> original. Then E3 (RpWorld + vehicle lighting, ledger #9), E4 (Im2D HUD/menu), E5 (wire
> as shipping renderer + parity sweep, spike → dev-only).

## WS-D-VISUAL — power-up visual de-stub. Pool: Mashed_pool5 (after E2/E-DRIVER)
> De-stub the RW scene-graph teardown leaves (Gun_Deact/Drum_Deact) + the WS-B/RW path in
> Powerup/PowerupEffects.cpp; bind powerup visual spawn/teardown to the real RW scene
> graph. Diff vs original.

## WS-J2 — collision FX. Pool: none (after WS-B-WIRE)
> Wire permdict collision FX (skids/impacts) to real collision events from B-WIRE. Verify
> FX per impact.

---

## Recommended wave-3a (parallel, no shared cliffs)
**WS-A-VERIFY** (Frida) ‖ **WS-C-WIRE+remaining-AI** (pool4, Ghidra+build) ‖
**WS-E2/E-DRIVER** (pool6, Ghidra+build). Same constraints as before: ONE shared Ghidra
server (RE phases of C + E serialize), ONE booted-original (A-VERIFY owns it — keep it OFF
the build agents' `.asi` redeploys), and TrackRenderer/build.bat are the merge points
(worktree each, sweep after). Then wave-3b: **WS-A8+B-WIRE** (needs A-VERIFY green) →
**WS-J2**; **WS-D-VISUAL** (needs E). After 3a+3b the standalone runs real physics + AI +
collision + powerups + RW-rendered world — the port is functionally whole.
