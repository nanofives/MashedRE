# Session prompts — wave 2 (post-convergence). Paste one per session.

Generated 2026-06-16 after the physics-port wiring pass. Wave 1 = the WS-A..J
subsystem ports (verification-grade, mostly `.asi`-hook or self-contained). This
wave finishes turning those ports into a standalone that runs its OWN engine.

Use the COMMON PREAMBLE from `re/SESSION_PROMPTS.md`. NO-GUESSING; anchor SHA-256
BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E; build from repo
root; Ghidra read-only via ghidra-pool (pool0/1 locked); `diff-original` = C4;
commit as nanofives with the Co-Authored-By trailer.

KEY STATE (read first): [[project-ports-done-convergence-pending]],
`re/analysis/physics_completion_track_2026-06-16.md`. The per-frame physics chain is
`FUN_00470c70`(disp) → `FUN_00470670`(**A4, ported**) → { `FUN_0046ddb0`(**A5, ported**),
`FUN_00467650`(**A6a**), `FUN_00468980`(**A6b**) } → parked-damp; spawn = `FUN_0046b540`(**A3**).
A4 = `Vehicle/VehicleControl.cpp`; A6a/A6b/A3 = `Vehicle/{Integrate2,AeroStabilize,VehicleInit}.cpp`
(this wave's fork). All physics ports are **PENDING diff-original C4**.

---

## WS-A8 — wire the physics chain into the standalone (the runtime integration). Pool: Mashed_pool2
> Make `mashed_re.exe` actually run the ported vehicle physics. Steps:
> 1. **Exe link closure**: the EXE source list in `mashedmod/build.bat` currently OMITS
>    all `Vehicle/*.cpp` (physics only ever compiled into the `.asi`). Add
>    Vehicle/{ForceIntegrator,ForceIntegratorStubs,VehicleControl,Integrate2,
>    AeroStabilize,VehicleInit}.cpp and resolve the dependency closure it pulls in
>    (Collision::g_suspScratch @ DAT_00881560, the Math/ leaves, the runtime globals).
>    Build the exe clean.
> 2. **Record store**: allocate the 16 × 0xd04 vehicle records (base mirrors
>    DAT_008815a0; layout = re/analysis/structs/vehicle.md). Call `VehicleInit(slot,
>    trackType)` (A3) at car spawn; feed trackType from the selected area's Course_Id.
> 3. **Per-frame**: port/!replicate the dispatcher `FUN_00470c70` (inits
>    _DAT_0088e610 = dt*_DAT_005cea80, _DAT_0088e5f0 = _DAT_005ccd08/_DAT_0088e610;
>    selects the per-car input descriptor &DAT_007f1038 + map*0x13; sets
>    g_torqueRingPhase). Call `VehicleControlIntegrate` (A4) per active car.
> 4. **State adapter**: map the standalone car (TrackRenderer AiCar: pos/vel/yaw) ↔ the
>    record fields (+0x9b0 vel, +0x9d4 fwd, +0x190 accel mag, +0x9e4 speed, transform).
>    Drive the rendered car FROM the record.
> 5. **Contacts**: feed the WS-B contact arrays (the B4 wheel solver output) into the
>    record fields A5 reads (DAT_008815a4 active scan + the per-wheel contact buffers).
> 6. **Toggle**: gate behind `MASHED_REAL_PHYSICS` (default OFF → the working scaffold
>    `TrackRenderer::UpdateCar` stays the shipping path until C4). When ON, the real
>    chain drives the cars.
> 7. Build GREEN; run a smoke race with the toggle ON; capture. Commit.
> Depends: A6a/A6b/A3 ports (this wave), WS-B contact wire. Acceptance is the WS-A-VERIFY
> diff, not this smoke test.

## WS-A-VERIFY — diff-original C4 the physics ports. Pool: dev .asi + Frida
> Boot the original (build-26200 compat: setup_mashed_compat.ps1 handles EMULATEHEAP;
> see [[project-emulateheap-boot-av]]). Stand up an installed-hook canonical-race
> `diff-original` lane for each ported physics fn: A4 0x00470670, A5 0x0046ddb0, A6a
> 0x00467650, A6b 0x00468980, A3 0x0046b540 (+ the RW-math leaves, already C4). For each:
> RH_ScopedInstall the original RVA in the .asi, run a matched-input race, diff per-frame
> vehicle-record fields (velocity +0x9b0, fwd +0x9d4, ang-vel +0x9bc, torque +0x1a8/+0x26c)
> vs the original. RESOLVE the two open [UNCERTAIN]s: (a) FUN_004c3df0 arg order
> (dst,matrix,count,src vs dst,src,count,mtx) — the A5/A6 binding depends on it; (b)
> FUN_004a3384 identity (A6b) + whether A6a's float10 needs inline-__asm (Rodrigues
> precedent). Promote each to C4 via re-classify only with the inline-JMP live + canonical
> evidence (NO synthetic-bypass overclaim — [[feedback-no-overclaiming-c-levels]]). Move
> each scaffold→verbatim in re/analysis/SESSION_VERIFICATION_AUDIT_2026-06-16.md.

## WS-C-STANDALONE — reimplement the AI to run in the exe. Pool: Mashed_pool4
> The WS-C2 port (Ai/AiController.cpp, AiTargeting.cpp) is a `.asi`-HOOK port: it calls
> the ORIGINAL functions via absolute-RVA casts (24 in AiController, 8 in AiTargeting) +
> RH_ScopedInstall — it CANNOT run in mashed_re.exe (those RVAs are zeroed image-pad).
> Produce a STANDALONE reimplementation: reverse + reimplement each of the ~32 callees as
> standalone C++ (the FUN_00418860 family + its leaves; AI path data via AiData_LoadInto
> from AI.piz, already cracked). Keep the .asi hook port as the diff reference. Then wire
> into TrackRenderer (replace the gate-ribbon scaffold AI) behind a toggle; diff AI car
> positions over N frames on a matched seed. Doc the standalone-vs-hook split. Commit.
> Read re/analysis/ai_controller.md. Larger effort — phase the 32 callees.

## WS-E1 — port the RW world render (the spike → real swap). Pool: Mashed_pool6
> The renderer is a D3D9 "spike", not RenderWare. Using the E1 map
> (re/analysis/render_world_path_E1_2026-06-16.md; B-FULL ratified — port the RW D3D9
> driver too), port the RW world render: frame loop → RpWorld sector render → atomic+0x48
> FUN_004e5f90 → RxPipelineExecute 004d40d0 → D3D9 (device table @ 0x00618220). The
> E2-prep work (RxPipeline submit + BSP→RpWorld loader) is RE'd — build on it. Replace
> TrackRenderer's spike world draw. Screenshot-parity a track vs the original. Commit.
> Then E2 (materials/multi-TXD/draw order), E3 (RpWorld + vehicle lighting, ledger #9),
> E4 (Im2D HUD/menu), E5 (wire as shipping renderer + parity sweep). Independent of A/B/C.

## WS-B-WIRE — feed real collision contacts into the standalone physics. Pool: Mashed_pool3
> B2/B3 ported the car↔world + car↔car contact solvers (faithful C2); B4 ported the wheel
> solver FUN_0046f6c0 + verified the chain. Wire them as the contact SOURCE for the
> standalone A5 integrator (fills DAT_008815a4 active scan + per-wheel contact buffers in
> the record). Per the B1 gate (RATIFIED, [[project-qhull-rwphysics-island]]): port the
> contact subset, qhull stays OFF the per-frame path (load-time hull build only). Diff
> contact points vs an original Frida trace. Commit. Depends: WS-A8 record store.

## WS-D-VISUAL — power-up visual de-stub. Pool: Mashed_pool5
> D2/D3 ported the dispatch + 9 per-type effects and wired them; the RW scene-graph
> teardown leaves (Gun_Deact/Drum_Deact in Powerup/PowerupEffects.cpp) + the WS-B/RW path
> are STUBBED pending the renderer. Once WS-E lands, de-stub: bind the powerup visual
> spawn/teardown to the real RW scene graph. Diff vs original. Commit. Depends: WS-E.

## WS-J2 — collision FX. Pool: none (mostly)
> J1 wired char voice-banks + engine + music transitions. Remaining: wire permdict
> collision FX (skids/impacts) to real collision EVENTS — gated on WS-B-WIRE (need the
> contact events). Verify FX plays per impact. Commit. Depends: WS-B-WIRE.

---

## Recommended order
1. **WS-A6a/b/A3 ports** (this wave, fork) → **WS-A-VERIFY** (diff the chain) → **WS-A8**
   (wire + run). This is the critical path to a real physics race.
2. Parallel long pole: **WS-E1..E5** (renderer) — unblocks WS-D-VISUAL + vehicle lighting.
3. **WS-B-WIRE** feeds A8; **WS-C-STANDALONE** (AI reimpl) is independent + large.
4. **WS-D-VISUAL** + **WS-J2** are tail items gated on E / B.
Then the standalone runs real physics + AI + collision + powerups + RW render — a port.
