# Session prompts — wave 4. Paste one per session.

Generated 2026-06-16 after wave-3a (WS-C-WIRE + WS-E2 merged to main; WS-A-VERIFY ran).
Use the COMMON PREAMBLE from `re/SESSION_PROMPTS.md`. NO-GUESSING; anchor SHA-256
BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E; `diff-original` = C4;
commit as nanofives + trailer; Ghidra read-only via ghidra-pool; guard `original/`.

### TWO HARD ORCHESTRATION RULES (learned wave-3a — bake into every prompt)
1. **Behavioral / Frida / boot-original sessions MUST run in the MAIN working tree, NOT an
   isolated worktree.** `original/` is gitignored — it exists only in the main checkout, so
   a worktree has no `MASHED.exe` / `.asi` / Frida target. (WS-A-VERIFY got BLOCKED on this.)
   RE/port/coding sessions CAN use worktrees.
2. **Use ANALYZED Ghidra pool clones.** pool6 + pool11 are analyzed; **pool4 is NOT** (no
   functions defined) — assign pool6/pool11/pool7 and verify `function_list` is non-empty.

State carried from wave-3a (all PENDING C4 / inert behind toggles):
- Physics chain ported but **NOT wirable yet**: A6a (Integrate2) contact-friction block +
  grip-clamp + boost SM + float10 still deferred; A6b (AeroStabilize) rotation-apply deferred;
  A5 needs a real C++ device-transform. `FUN_004c3df0`=(dst,src,count,matrix); `FUN_004a3384`=acos;
  `FUN_004a2c48`=ROUND(ST0). Detail: re/analysis/WSA_VERIFY_2026-06-16.md + physics_completion_track.
- AI exe-wired behind `MASHED_REAL_AI` but **INERT**: uses the original data model (spline banks
  @0x00801aa0, 0x74 ctrl records) — needs the `.AI` loader + a ctrl-block→`ai_cars_` adapter.
  Steering math fixed (U-C-STEER closed). Behaviour-tree callees still stubbed.
- RW world readers ported behind an inert seam: device submit + RW helper substrate deferred.

DEP ORDER: (A6-COMPLETE + A-DEVXFORM) → A-VERIFY-2[main tree] → A8+B-WIRE → J2 ;
AI-BRIDGE → C-AITREE ; E-DEVICE → E3/E4/E5 → D-VISUAL.

---

## WS-A6-COMPLETE — finish the deferred physics blocks. Pool: Mashed_pool11
> Complete the verbatim ports of A6a `FUN_00467650` and A6b `FUN_00468980` (re/analysis/
> physics_completion_track_2026-06-16.md lists what's deferred). A6a: the per-wheel
> contact-frame friction block, the trailing speed-limit grip-clamp, and the boost/jump
> state-machine (the parts after the integrate spine), with the `float10` (x87) evaluation
> matched faithfully (inline-`__asm` per the RwMatrixRotateInner precedent where plain
> double diverges). A6b: the rotation-apply (the two `FUN_004c4d20` calls; `FUN_004a3384`=
> `std::acos`). Cite every RVA + harvest remaining constants. Mark PENDING C4. Build green.
> Worktree OK. Depends: nothing (RE/port only).

## WS-A-DEVXFORM — real C++ device-transform for physics. Pool: Mashed_pool11
> `FUN_004c3df0`=(dst,src,count,matrix) is a pass-through to the RW *device* transform — the
> standalone needs a plain C++ 3×4-matrix × vec3 transform so A5/A6a get correct results
> without RW device init. Implement it (Math/) and bind ForceIntegrator's `Rw_TransformPoints`
> + A6a's transform calls to it (NOT the device thunk). Verify vs a hand-computed case.
> Build green. Worktree OK.

## WS-A-VERIFY-2 — behavioral C4 of the physics chain (MAIN TREE ONLY). Pool: dev .asi + Frida
> RUN FROM THE MAIN WORKING TREE (rule #1). After A6-COMPLETE + A-DEVXFORM land: boot the
> original (`setup_mashed_compat.ps1`; build 26200 ⇒ EMULATEHEAP on), RH_ScopedInstall the
> 5 chain fns at their RVAs, register them in re/frida/hooks_registry.py, and run the
> installed-hook canonical-race diff per fn (hot-path caution: sample one fn, short runs;
> poll_attach_catch_crash.py for crashes). Diff per-frame record fields original-vs-reimpl.
> Promote GREEN fns to C4 via re-classify (inline-JMP live; no synthetic-bypass overclaim).
> Output the GREEN subset for A8.

## WS-A8 + WS-B-WIRE — wire the verified physics + contacts. Pool: Mashed_pool2 (+pool3)
> Only after VERIFY-2. As in wave-3 spec: exe-link the Vehicle/*.cpp, allocate the 16×0xd04
> records, run the `FUN_00470c70` dispatcher, feed B2/B3/B4 contacts (DAT_008815a4 + per-wheel
> buffers; qhull off the per-frame path), map car↔record, gate `MASHED_REAL_PHYSICS`. Smoke-race
> ON + from-main behavioral diff. Then WS-J2 (collision FX) once contacts are live.

## WS-AI-BRIDGE — make the wired AI actually drive. Pool: Mashed_pool11
> The `MASHED_REAL_AI` tick is INERT because it reads the original data model. Build the
> bridge: (1) the `.AI` spline loader (`FUN_004235b0` / `AiData_LoadInto` into the banks at
> 0x00801aa0 — format already cracked, Ai/AiData.h) loading the track's AI%d.AI from AI.piz;
> (2) a ctrl-block (0x74 records) ↔ TrackRenderer `ai_cars_` adapter so the tick's outputs
> drive the standalone opponents. With `MASHED_REAL_AI` ON, opponents should follow the real
> race line. Worktree OK. This is the prerequisite for live AI.

## WS-C-AITREE — port the AI behaviour tree. Pool: Mashed_pool11
> Port the still-stubbed `FUN_00416250` targeting behaviour tree (modes 1..10) + its helpers
> `FUN_00414570/00415880/00414a70/00414c30/004150e0/00414f00/004148b0/00415020/00416060/
> 00415d00`, the 4/9/8 variants (`FUN_00416a30`/`00417da0`), rubber-band `FUN_004177b0`,
> powerup activation `FUN_00415220`, into Ai/AiStandalone. `FUN_00416250` is already fully
> decompiled (WS-C-WIRE). Mark PENDING C4. Worktree OK. Depends: ideally WS-AI-BRIDGE.

## WS-E-DEVICE — the RW D3D9 device submit (first real pixels). Pool: Mashed_pool6
> Port the deferred RW D3D9 driver substrate so the world readers (RwWorldLoad) + traversal
> (RwWorldRender) actually draw: the RxPipeline node bodies `FUN_004eb3d0`/`FUN_004ea6d0` +
> `DrawIndexedPrimitive` (0x004e1007) + `RpWorldInstance` device VB/IB, and the RW stream/
> error/plugin helpers currently in RwWorldLoadStubs.cpp (`FUN_004cc5e0/004cbd30/004cc790/
> 004d7ff0/004d8480/004d8000/004e1b60/004e1c90/004f3e90`). Resolve `[U-E2-HDR]`/`[U-E2-XCRIPT]`
> vs the real rwID_WORLD chunk bytes (the standalone's byte-faithful `Track::World` is a
> cross-check). With `MASHED_RW_RENDER` ON, render a track through the RW path; screenshot-
> parity. Then E3 (RpWorld + vehicle lighting, ledger #9), E4 (Im2D HUD/menu), E5 (ship).

## WS-D-VISUAL — power-up visual de-stub. Pool: Mashed_pool5 (after E-DEVICE/E3)
> De-stub Gun_Deact/Drum_Deact + the WS-B/RW path in Powerup/PowerupEffects.cpp; bind powerup
> visuals to the real RW scene graph. Diff vs original.

---

## Recommended wave-4a (parallel — all RE/port, worktrees OK)
**WS-A6-COMPLETE + WS-A-DEVXFORM** (pool11) ‖ **WS-E-DEVICE** (pool6) ‖ **WS-AI-BRIDGE**
(then C-AITREE) (pool11/pool7). Bottlenecks unchanged: one shared Ghidra server (RE phases
serialize), TrackRenderer/build.bat = merge points (worktree each + sweep). NONE of 4a boots
the game, so no `.asi`/Frida contention.
## Wave-4b (MAIN TREE, sequential, after 4a merges)
**WS-A-VERIFY-2** (boot-original physics diff) → **WS-A8 + WS-B-WIRE** → **WS-J2**; and the
**WS-E parity** pass. After 4b: real physics (verified) + live AI + RW-rendered world in the
standalone. Remaining tail: E3/E4/E5 polish, D-VISUAL, full-race parity.
