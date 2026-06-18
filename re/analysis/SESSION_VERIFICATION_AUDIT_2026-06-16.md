# Verification audit — 2026-06-15/16 scaffold-to-port session (item 5)

The project's acceptance gate is **C4 = bit-identity vs the original via the
`diff-original` Frida lane** (re/CONFIDENCE.md; [[feedback-no-overclaiming-c-levels]]).
Almost everything built this session is **SCAFFOLD** (invented stand-ins that run
+ look right) — by definition NOT C4 and NOT `diff-original`-able, because there
is no 1:1 original function to diff against a scaffold. This itemizes the debt so
it is explicit + actionable (you cannot pay down debt you have not classified).

## Classification of this session's deliverables

### A. SCAFFOLD — invented; NOT faithful; cannot be C4'd as-is
Must be REPLACED by verbatim ports before they can be verified:
- Vehicle handling (TrackRenderer::UpdateCar) — kinematic; real = FUN_00470670
  cluster (RW-Physics). See vehicle_physics_cluster.md. **C4 blocked on the port.**
- AI driving (gate-ribbon lane follower) — real = **FUN_00418860 family** (per-frame AI
  tick → per-vehicle decide/steer/throttle → synthetic input block). [Corrected
  2026-06-16: "FUN_0040e480" was a mis-citation; that RVA is `CarSlotStateSet` (frontend
  leaf). Cluster mapped in `re/analysis/ai_controller.md` (WS-C C1 done).]
  **WS-C2 progress (2026-06-16):** ctrl byte→axis map RESOLVED (U-0407 closed,
  `ai_ctrl_byte_map_RESOLVED_2026-06-16.md`); the orchestration spine
  (`Ai/AiController.cpp`: 0x00418860 tick, 0x00418560 step, 0x00417640 post-step) +
  LOS/spline/steering leaves (`Ai/AiTargeting.cpp`: 0x00416060 LOS, 0x004161e0 spline-target,
  0x0046d510 velocity, 0x00415e20 steering-angle-error, 0x00415d00 wall-ahead) are
  **verbatim-ported** as RH_ScopedInstall hooks and **build+link into mashed_re_dev.asi**
  (all 8 exports verified in the .map). These are
  **NOT yet C-level-promoted** — porting is implementation, not evidence; the C3a
  per-function `run_diff` (esp. 0x00416060, isolatable) needs a scenario:'race' attach,
  and the whole-loop C3b position diff stays gated on WS-A8. Remaining C2: the float
  control step FUN_00416250 + 4/9/8 variants, pre-tick FUN_004177b0, bank switcher
  FUN_00417180, the rest of the targeting helpers (incl. FPU-arg FUN_004150e0 needing a
  naked-asm shim), the `.AI` parser, and the DAT_005ccXXX tuning-const harvest.
- Collision (ground raycast only) — real = RW-Physics contact system.
- Power-up EFFECTS (boost/shield/missile/mine/shock) — real = **FUN_0045bba0
  dispatcher + 9-entry type table @0x005f9998 family**. [Corrected 2026-06-16:
  "FUN_00430670" was a mis-citation; that RVA is a player-slot resolver. **WS-D/D1
  done** — full architecture, slot struct @0x0088fbe0, type table and per-type
  effect-fn RVAs in `re/analysis/structs/powerup_system.md`.] D2 (verbatim per-type
  port) gated on Ghidra fn-split of 0x453f60–0x45be81 + WS-A1 (vehicle struct) + WS-B
  (collision) + WS-E (RW scene-graph). Scaffold held-type name now data-faithful to
  POWERUPS_GOLD.LUA (PickupField::RealTypeName).
  - **WS-D2+D3 done (2026-06-16, this session).** Fn-split (clone-only, no master
    write) + full decomp captured in `re/analysis/structs/powerup_effects_decomp.md`.
    **D2:** `mashedmod/src/mashed_re/Powerup/{PowerupSystem,PowerupEffects}.{h,cpp}`
    — the dispatcher (FUN_0045bba0), slot lifecycle (FUN_0045baa0/bfa0/bac0),
    fire-mode decode, and each of the 9 types' ARM/FIRE/CANFIRE/DEACT/TICK decision
    logic ported VERBATIM; the leaves (RW scene-graph WS-E, contacts WS-B,
    projectile pools 0x006883xx, tuning band 0x005c*) routed through
    `IPowerupBackend` stubs (each citing its RVA). **D3:** the invented
    boost/shield/missile/mine/shock `ApplyPowerup` switch in `TrackRenderer.cpp` is
    REPLACED — `FireHeldPowerup()` now reads `PickupField::held_type()` (the real
    code) and drives `PowerupSystem`; `PowerupBackendImpl` realises each effect's
    leaf on the in-race visuals (missiles_/mines_/ai_cars_/parts_). Both targets
    build GREEN. **Still C2** (faithful structure, stubbed un-diffable leaves) — NOT
    C4; not `diff-original`-able while the leaves are stubs and the effects read live
    race state. The numeric rates/durations are marked stand-ins (DAT_005c* band
    unharvested), not bit-faithful. C4 path = WS-H2 installed-hook canonical-race
    observation of FUN_0045bba0 once WS-B/WS-E land, not synthetic run_diff.
- Particles, pickup orb visuals, lap/elim flow tuning, results screen layout,
  game-mode->rule mapping, car flat-lighting (approx, not RW RpWorld).
- Progression store (our own sidecar format, not the gamesave serialization).

### B. VERBATIM-PORTED — a specific RVA; C4-TARGETABLE (diff-original pending)
These CAN be `diff-original`-verified against the booted original:
- Race camera — Race/RaceCamera (0x00446520 / 0x00410d10). [C4 lane: open]
- Nav state machine — FUN_0043d2a0 + descriptor tables. [partially diff'd earlier]
- Elimination scoring (FUN_0040eee0 / FUN_0040b290 / 0x00410510) — partial.
- RW math primitives (Math/RwV3dTransform, RwV2d, RwSqrt) — leaf, diff'able.
**C4 QUEUE (next):** RaceCamera, the scoring trio, the RW math leaves — run
re/frida/run_diff.py per hook once the original is booted (the boot AV is fixed,
[[project-boot-crash-rw-nullderef-not-display]]).

### C. DATA-VERIFIED — format cracked + checked against the real asset bytes
Not C4 (no function), but verified that the PARSE matches the original data:
- RWS audio 0x809 + 0x80d IMA ADPCM — decoded WAVs + autocorr 0.84/0.96 vs noise.
- Course_Id<->area table — read directly from each COURSE.LUA.
- POWERUPS_GOLD.LUA placement — independent re-parse cross-check (below): MATCH.
- **WS-C .AI path data (2026-06-16)** — opponent-AI race-line splines + tile grid
  cracked from MASHED.exe (saver FUN_00423540 / loader FUN_004235b0) + the real
  AI.piz bytes: AI%d.AI = 12-byte RW chunk hdr (type 0x13269902, size 0x11884,
  ver 0x1c02000a) + 0x11884 payload (tile grid 128×128 i16, subcell grid, 4 line
  arrays race/inside/slow/cheat each 3×0x204 splines of 64 XZ points + count).
  **13/13 members parsed, exact 0x11890 byte consumption** (re/tools/ai_data.py
  validate). C++ header Ai/AiData.h (validate/load); doc
  re/analysis/formats/ai_path_data.md. AI30.AI ver=0x1803ffff (loader ignores ver).
- **WS-F track data formats (2026-06-16)** — all five cracked as standard RW core
  chunk streams (IDs vs rwplcore.h), parsed with exact byte consumption across
  **229/229 assets / 13 tracks, 0 failures** (re/tools/rw_track_data.py validate;
  C++ twin Track/TrackData.{h,cpp} runtime-checked vs Arctic — ALL PASS). Doc:
  re/analysis/formats/track_anim_data.md.
    - F1 .SPL = rwID_SPLINE (0x0C); F2 .ANM = rwID_HANIMANIMATION (0x1B);
      F3 .UVA = rwID_UVANIMDICT (0x2B); F5 .MTS = rwID_MATRIX (0x0D) list
      (AUDIT: instance-placement matrices, NOT material scripts; .MTS.TXT twins
      stale on several tracks — binary authoritative); F4 LAPDATA.LUA (text).
    - Wired (both targets build clean): F4 laps count at the LAPDATA finish gate
      (was hardcoded gate 0); F3 UV-scroll on world sea/sky materials. Deferred
      (in the doc): F2 copter prop render, F3 prop/extension binding, F4
      multi-line sequence + split times (the original lap FUN — out of no-Ghidra WS-F).

## Verification run this session (data level)

- POWERUPS_GOLD.LUA (Arctic): C++ parser = 18 spawns; independent Python re-parse
  cross-check = 18, positions/types identical (see the cross-check log). PASS.
- (Audio) cdaudio 0x80d -> IMA -> WAV: autocorr +0.96 (real music), rate 44100
  from the 0x80e header. PASS (decode validity).

## Path to paying down the debt
1. Boot-original `diff-original` lane (run_diff.py) for the category-B verbatim
   pieces -> first real C4 datapoints from this session's ports.
2. As each scaffold (A) is replaced by a verbatim port, move it A->B and C4 it.
3. Category-C data parsers stay "data-verified" (no function to diff).
This audit is the item-5 starting point; see re/analysis/C4_REVALIDATION.md for
the historical C4 debt tracker.

## H1 update — lane stood up on build 26200 (2026-06-16, later same day)

**Boot regression found + fixed first.** The boot-original lane was dead on
arrival: `run_diff.py` failed `VirtualAllocEx 0x5`, and a clean manual boot (no
Frida, no .asi) AVed within ~7 s. Root-caused via WER minidump
(`scripts/parse_minidump.py`, new this session): `0xC0000005` WRITE in
`ntdll!RtlpHeap +0x542f0`, `ECX=0x5477`, heap base 0x30000, MSVC-CRT-init
ret-chain 0x4ac660/0x4a5f49/0x4a4274/0x49644c — **byte-for-byte the
[[project-emulateheap-boot-av]] signature**. Isolation proved it independent of
the compat layer (reproduced with no layer / WIN98RTM-only / no apphelp), of our
`.asi` (`MASHED_RE_NO_AUTO_HOOK=1`), and of our d3d9 proxy (real system d3d9
substituted). Cause: an **overnight Windows feature update 26100→26200 + reboot
(08:54)** changed the native heap so MASHED's legacy CRT init corrupts it. The
EMULATEHEAP relationship **inverted** (dropped on 26100, now REQUIRED on 26200+).
Fix = `~ RUNASINVOKER WIN98RTM HIGHDPIAWARE EMULATEHEAP`; `setup_mashed_compat.ps1`
now toggles EMULATEHEAP on `OSVersion.Build >= 26200`. Lane re-verified GREEN.

**Category-B results.**
- **RW math leaves — REINFORCED (already C4).** All 8 registered leaves re-diffed
  GREEN on build 26200 via run_diff (path1 bit-identity, hook bypassed = C3-grade
  per CONFIDENCE.md, reinforcing existing C4): fast_sqrt 18/18, fast_inv_sqrt
  18/18, vec3_magnitude 18/18, rw_v3d_transform_point 14/14,
  rw_v3d_transform_vector 10/10, vec2_length 14/14, vec2_normalize 10/10,
  rw_matrix_scale 11/11 (113 vectors, 0 mismatches; CSVs in `log/diff_*.csv`).
  **NET-NEW:** the 4 pointer-arg leaves (RwV3dTransformPoint/Vector @0x004c3730/
  0x004c3880, Vec2Length/Normalize @0x004c3bf0/0x004c3c60) carried C4 whose A/B
  was "harness-limited RPC (pointer args)" — i.e. path1 bit-identity was never
  actually run. The diff_template.js transform/vec2 handlers now exist, so this
  is the **first clean path1 bit-identity** for those four. (No re-classify: level
  unchanged at C4, evidence strengthened.)
- **RaceCamera (0x00446520 / 0x00410d10) + scoring trio (0x0040eee0 / 0x0040b290 /
  0x00410510) — NOT synthetic-diffable; stay C2.** Confirmed these are *standalone
  adapted reimplementations* inside mashed_re.exe (Race/RaceCamera.cpp camera+
  elimination; D3d9Render/TrackRenderer.cpp scoring) — NOT `RH_ScopedInstall`
  hooks at those RVAs and not exported from the .asi. They are multi-KB
  state-machines reading live race globals (game-mode DAT_008a94d0, score arrays
  DAT_008a94e0…) and calling subroutines; not callable in isolation, so the
  bypassed-call run_diff lane cannot bit-diff them. Their **real C4 path is an
  installed-hook canonical-race observation** (RH_ScopedInstall the original RVA
  in the .asi + `scenario=race` telemetry diff with the inline-JMP live = WS-H2),
  not run_diff. Behavioral evidence already on file: camera live-trace
  `camera_trace.csv` (zoom law 286/286 rows; offset/pitch within hmix margin) —
  strong but not bit-identity, so the "C2 retained" in hooks.csv is correct.
  Their **leaf sub-functions are already C3** via the race scenario lane:
  RaceFloat898980Get 0x00442df0, CarStatePairGet 0x0046cbb0, CarSnapshotDwordGet
  0x00423b20, RaceModeSet 0x0040e360, Player::WriteFieldZero 0x0041ef60,
  Pred405890 0x00405890, TiebreakFlagGet 0x00431d80, EntityScoreFieldAdd 0x0046c700.

**Honest C-level note (no overclaiming).** run_diff with the hook bypassed is
C3-grade ([[feedback-no-overclaiming-c-levels]]); the math leaves stay C4 on
their prior install-observe evidence, now backed by the path1 A/B they lacked.
**No function was promoted to C4 from this run alone.** The "first C4s" goal for
RaceCamera/scoring is deferred to WS-H2 (installed-hook canonical race).

## H2 update — camera/scoring C4 blockers pinned (2026-06-16)

Findings: `re/analysis/WS_H2_C4_LANE_FINDINGS_2026-06-16.md`.
- **Part 1 (C4 newly-landed WS-A..E ports): no-op.** Every WS-commit since the
  audit baseline is an RE-map / data-verify / decision, not an installed verbatim
  function port → nothing new to bit-diff. Recorded, not skipped.
- **Camera (0x00446520 / 0x00410d10): C2 is permanent for the standalone body.**
  Evidence-pinned: the original routes magnitude/normalize through the RW
  fast-sqrt **LUT** (`call 0x004c3ac0` ×3, `call 0x004c39b0` ×4 in
  `FUN_00446520.asm`); the standalone `RaceCamera.cpp` must approximate with
  `std::sqrt` because the LUT globals (`0x007d3ff8/0x007d3ffc`) are not the
  fast-sqrt table standalone — so it is bit-distinct **by construction** (this
  is the exact "offset/pitch within margin" residual in `camera_trace.csv`). A
  camera C4 datapoint needs a SEPARATE ~7.4 KB `.asi`-only verbatim hook calling
  the real LUT primitives — large; recommend its own session.
- **Scoring trio (0x0040eee0 / 0x0040b290 / 0x00410510): C4-FEASIBLE.** Integer
  arithmetic over `DAT_008a94e0…` globals, no LUT/transcendental hazard → an
  `.asi`-only verbatim hook can be bit-exact and reach C4 via installed-hook
  canonical-race observation. This is the realistic WS-H2 win (bounded effort).
- **Decision pending user ratification** (architecture-level: authoring
  `.asi`-only second implementations distinct from the standalone bodies). See
  the findings doc's "Recommendation".

## WS-A-VERIFY-3 update — physics chain MOVES; per-fn C4 verdict (2026-06-17)

The vehicle-physics chain (A3 FUN_0046b540, A4 FUN_00470670, A5 FUN_0046ddb0,
A6a FUN_00467650, A6b FUN_00468980) now DRIVES the standalone car (speed 0->~112,
grounded reaches 4.0, clean exit — see PHYS_SMOKE_2026-06-17.md "WS-PHYS-MOTION").
Motion root cause + fix: the port passed a ZEROED +0x928 wheel-matrix block as A5's
xform (original passes A4's param_4 world xform); forward = xform*(0,0,1) collapsed
to (0,0,0) -> drive direction 0 -> no force. Fixed by synthesizing a yaw world matrix.

### [U-A6A-FLOAT10] — RESOLVED
A6a's grip (l_60) + normal-load (l_d0) accumulators store back as 32-bit `FSTP float
ptr` every iteration (asm 0x004682c0.. pool11); the x87 80-bit width is only a ST0
transient of one a*b+c before rounding to float32. `double` intermediates are a
strictly-closer approximation (<=1 ULP), and the standalone fn is bit-distinct by
construction anyway (LUT fallback, below). For an .asi verbatim hook the TU must
stay x87 (build.bat: no /arch:SSE2).

### Per-fn C4 verdict (HONEST — no synthetic-bypass overclaim)
All five remain **C2** in hooks.csv. None was promoted this session, because:
- **A5/A6a/A6b are BIT-DISTINCT BY CONSTRUCTION in the standalone** — same hazard the
  H2 camera finding pins: they route magnitude/normalize/transform through the RW
  fast-sqrt LUT (FUN_004c3ac0/004c39b0/004c3df0), which the standalone replaces with
  std::sqrt fallbacks (no RW device -> null LUT, WS-PHYS-CRASH-FIX). So the standalone
  bodies CANNOT be bit-identical to the original; a C4 datapoint needs `.asi`-only
  verbatim hooks calling the real LUT primitives in the running MASHED + installed-JMP
  canonical-race telemetry — the SAME architecture "pending user ratification" above.
- **They are hot-path** (FUN_004c3ac0 fires ~2,700/s at the menu — CLAUDE.md "Frida
  overhead on hot paths"): an Interceptor-per-frame field diff destabilises MASHED in
  ~6s, so the telemetry lane must hot-path-SAMPLE one fn at a time, not trace all five.
- **No RH_ScopedInstall + no hooks_registry.py entry exists** for any of the five; they
  are standalone ports only. The full register-ABI (.asi A4 EAX=record, A5 EDI=record)
  installed-hook lane is a large, multi-session build.

**Verdict A3/A4/A5/A6a/A6b = BLOCKED on the ratified `.asi`-second-impl C4 lane** (NOT
RED — the standalone bodies are faithful transcriptions that now produce correct
motion; they are simply not bit-diffable against the original while they use the LUT
fallback). A4 (FUN_00470670) is the one with NO transcendental hazard in its own body
(it only does the input->force arithmetic; the LUT calls are in the A5/A6a callees it
dispatches) -> **A4 is C4-FEASIBLE as a standalone-body installed hook** the same way
the scoring trio is (bounded effort), if/when the .asi-second-impl decision is ratified.

## WS-PHYS-C4-LANE — lane STOOD UP; A4 is C4-GREEN (2026-06-17, branch ws-phys-c4-lane)

User ratified the full `.asi`-only verbatim-LUT installed-hook canonical-race
telemetry lane. Built it; **A4 (FUN_00470670) is now C4 in hooks.csv** (first real
physics-chain C4). The rest of the chain remains C2 (scoped below).

### Lane architecture (`mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp`, .asi-only)
- **Register-ABI entry trampolines.** The physics fns are NOT __cdecl — the
  dispatcher passes the 0xd04 record in a register. A4's ABI (disassembled,
  Mashed_pool12): EAX=record; stack [+4]=param_1 [+8]=dt [+c]=input* [+10]=xform;
  caller-cleans (5 reg pushes ECX/EBX/EBP/ESI/EDI, `ret` no-imm). `A4_Entry`
  (`__declspec(naked)`) captures EAX + forwards the 4 cdecl args to `A4_Body`,
  preserving the same callee-saved set.
- **Live-LUT callee forwarders.** Naked shims set EDI=record (A5 FUN_0046ddb0),
  ESI=record (A6a FUN_00467650), ECX=ESI=record (A6b FUN_00468980) and forward the
  cdecl stack args to the **live originals** — so the RW fast-sqrt LUT (built by
  RwEngineOpen, live inside MASHED) executes; the input smoother FUN_004a2c48
  (round-ST0-to-i64, implicit ST0) is forwarded by a naked shim that loads ST0
  exactly as A4 does (FILD;FADD dt;CALL). This is what makes bit-identity reachable.
- **Built x87** (the .asi TU has no /arch:SSE2) so the C transcription's float
  intermediates round to float32 like the original. **EXACT-bit-pattern constants**
  (`Cf(0x........)`): the decimal literal `1.66677e-4f` (0x392ec604) for the grip
  scale was the *only* divergence — the real .rdata value is 0x392ec33e; fixed.
- **MASHED_HOOK_ONLY gate** installs only the physics hook live (HookSystem.cpp
  allowlist) → a clean "modded-vs-original" canonical scenario.

### Deterministic bit-identity self-test (the wall run_diff/Interceptor can't cross)
Frida Interceptor on A4 (>1000/s) destabilizes MASHED before a single call can be
captured (re-verified 3×). So the A/B is **in-process, ZERO Frida overhead**
(env `MASHED_PHYS_C4_SELFTEST=1`): per call, the hook runs the **ORIGINAL A4 body**
via an early-return trampoline (re-execute the saved prologue → JMP A4+5; 0x00470914
temporarily patched to a frame-restoring `RET` so ONLY the body math runs, not the
A5/A6a/A6b dispatch), snapshots the original body outputs, restores the record, runs
my `A4_BodyMath`, and bit-compares — logging `phys_c4_selftest.log`. Same exact live
record state → no race noise.

### Per-fn verdict
| Fn | RVA | Verdict |
|----|-----|---------|
| A4 VehicleControl | 0x00470670 | **C4-GREEN, 96/96 bit-identical** (88 drive-force calls in0=1..66, gm=6 Quick Battle, all 16 ring phases; 8 coast). inline-JMP LIVE (0xE9) confirmed in canonical race; chain crash-free. Evidence: `re/analysis/phys_c4_evidence/A4_selftest_GREEN.txt`. Promoted in hooks.csv; U-1408 (reg-ABI) resolved. |
| A5 ForceIntegrator | 0x0046ddb0 | **C4-GREEN, 96/96 bit-identical** (WS-PHYS-C4-A5 2026-06-17). EDI=record reg-ABI hook; inline-JMP LIVE in canonical race; in-process FULL-fn A/B vs live original GREEN over 2 races. All callees forwarded to live originals (RW LUT 004c3df0/4d20/3ac0/39b0, RNG 472650, rubber-band 442ce0/442c80, face-normal 46c5f0) → no stubs in body. Shared scratch 0x881560..0x881590 snapshot/restore; RNG-cursor detect (excludes +0xb00/+0xb08 if random branch fires). Evidence: `re/analysis/phys_c4_evidence/A5_selftest_GREEN.txt`. U-2687/U-3563 resolved. **COVERAGE-GAP:** in Arctic Quick Battle the car stayed all-4-grounded (grounded==4.0 all 96) so the airborne branch (grounded!=4.0: divide-by-dt susp path + airborne-flag) and the random-surface branch (key 0xffff32ff → +0xb00/+0xb08 via FUN_00472650) were NOT exercised — transcribed verbatim but unverified in-race (NOT overclaimed). |
| A6a Integrate2 | 0x00467650 | C2 (stays) — **RED 93/96** via the verbatim-LUT lane (WS-PHYS-C4-A6A 2026-06-17, below). [U-A6A-ST0] RESOLVED (7 ST0 shims). NOT promoted: 3/96 calls diverge 1-8 ULP in the suspension force X (`piVar12[0x1c]`) — the last un-shimmed [U-A6A-FLOAT10] chain (`local_bc`/`local_98`/`f5`/`f4` x87-register-retained). |
| A6b AeroStabilize | 0x00468980 | C2 — small but ECX=ESI=record + transcendental `FUN_004a3384` (asin/acos) + opaque `FUN_004c4d20` (no visible args, FPU/device matrix). Own session. |
| A3 VehicleInit | 0x0046b540 | C2 — spawn-time; deterministic table writes; not on the per-frame hot path. Lane applies but lower priority. |

### Remaining work (next sessions)
The A4 lane (register-ABI trampoline + live-LUT forwarders + early-return original-body
trampoline + in-process bit-A/B) is the reusable template. For A5/A6a/A6b each session
must: (1) author the register-ABI `.asi` verbatim hook forwarding LUT calls to the live
originals; (2) extend the self-test to a FULL-function A/B (snapshot+restore the shared
globals each writes; control/seed `FUN_00472650` RNG for A5, or exclude the random-
impulse +0x2c0 fields); (3) capture drive-path calls and bit-compare; (4) promote GREEN
subset via re-classify. ~~A5 next (task priority)~~, **A5 DONE (below)**; then A6a, A6b, A3.

## WS-PHYS-C4-A5 — A5 (FUN_0046ddb0) is C4-GREEN (2026-06-17, branch ws-phys-c4-a5)

A5 the per-wheel force integrator is now **C4 in hooks.csv** (second physics-chain C4
after A4). The lane is the A4 template extended for A5's two extra hazards.

### What A5 adds over A4 (and how it was handled)
- **EDI=record reg-ABI** (vs A4's EAX): prologue `MOV EAX,[ESP+8]; SUB ESP,0x70; PUSH
  EBX/EBP/ESI`, caller-cleans, `ret` no-imm (Mashed_pool11). `A5_Entry`
  (`__declspec(naked)`) captures EDI + forwards dt(`[ESP+4]`)+xform(`[ESP+8]`).
- **A5's whole function IS the body** (no callee-dispatch tail like A4), so the
  "original-body" A/B run is a forwarded call to the LIVE original via `OrigA5Trampoline`
  — it re-executes the 2 patched prologue instrs then `push 0x0046ddb7; ret` (clobber-free
  jump that PRESERVES EAX=xform, which the original pushes as the transform matrix). This
  bypasses our own inline-JMP so the self-test compares original-vs-mine on identical state.
- **SHARED globals**: A5 writes the per-wheel scratch `0x00881560..0x00881590` (48 b: the
  piVar10 loop + 4 steer scalars 0x881564/570/57c/588). The self-test snapshots+restores
  that region (with the 0xd04 record) so both impls see identical input.
- **RNG**: the random-surface branch (key 0xffff32ff) calls `FUN_00472650` twice writing
  +0xb00/+0xb08. The self-test reads the PRNG read-cursor `*(DAT_007dc578+4+DAT_007d3ff8)`
  before/after the original run; if it advanced, the random branch fired → those two fields
  are EXCLUDED from the bit-compare (not replayed) and the call is flagged `rng=1`. On Arctic
  the branch never fired (rng=0 all 96).
- All other callees forwarded to live originals (RW LUT 004c3df0/4d20/3ac0/39b0, rubber-band
  442ce0/442c80, face-normal 0046c5f0 with its EAX/ECX/EDX/ESI custom reg setup) → **no stubs
  in the body** (the C4 gate). Built x87; float10 → `double`.

### Exact-bit-pattern + transcription bugs caught by the A/B (the value of the lane)
1. Two `.rdata` constants mis-rounded from the header/decimal: `_DAT_005cc990`
   0x37278908→**0x3727c5ac**; `_DAT_005ccd08` 6000→**3000.0 (0x453b8000)**.
2. `(int)(floatexpr)` stores are **FSTP float stores**, not C++ int truncations:
   converted all such writes to float-reinterpret. The grounded-count smoking gun stored
   integer 4 vs float 4.0=0x40800000.
3. `(float)self[idx]` reads are **float REINTERPRETS** (the field is a float), not int→float
   conversions: converted all 48 record reads to `a5F()`. Grounded count was reading its own
   float bits as an int then re-converting → 4.0 ran away to ~1.3e9.
4. `OrigA5Trampoline` initially clobbered EAX(=xform) with the jump target before the
   original pushed it as the transform matrix → garbage world-forward; fixed via push-imm+ret.

### Verdict
A5 FUN_0046ddb0 = **C4-GREEN, 96/96 bit-identical** across two canonical Arctic Quick-Battle
races (deterministic; chain crash-free; clean process exit). Evidence
`re/analysis/phys_c4_evidence/A5_selftest_GREEN.txt`. **Honest coverage gap** (NOT overclaimed,
same class as A4's brake-branch note): the car was all-4-grounded for every sampled call, so
the **airborne branch** (grounded != 4.0) and the **random-surface RNG branch** (key 0xffff32ff)
were transcribed verbatim but not exercised in-race. A scenario that drives the car airborne
(jump/ramp) and onto a random-surface tile would close them. Remaining chain: A6a, A6b, A3.

## WS-PHYS-C4-A6A — A6a (FUN_00467650): RED 93/96, stays C2 (2026-06-17, branch ws-phys-c4-a6a)

A6a (the big x87 float10 velocity/ang-vel integration step) ported into the same
`.asi`-only verbatim-LUT installed-hook lane (`PhysicsChainHooks.cpp`). Full findings:
`re/analysis/phys_c4_evidence/A6a_FINDINGS_2026-06-17.md`; evidence
`re/analysis/phys_c4_evidence/A6a_selftest_93of96.txt`.

### [U-A6A-ST0] — RESOLVED
A6a calls FUN_004a2c48 (round-ST0-to-i64) at **7 sites** with implicit ST0 inputs absent
from the decomp. Disassembled (Mashed_pool12 RO) each call's exact x87 feed and reproduced
it with naked shims forwarding to the LIVE FUN_004a2c48: gear up/down timer (FILD[+0x494]
±dt), 3× boost timer (FILD[+0xbf4]−dt), the drive-projection round
(`(w·vel)/(w[-0xa]·1.74533)`), and the brake quantize (FILD input[5]+256.0). All ST0 inputs
cited to their instruction address.

### Verdict: RED — 93/96 bit-identical; NOT promoted (NO overclaim).
In-process A/B (A6a inline-JMP LIVE, canonical Arctic Quick-Battle, MASHED_HOOK_ONLY=A6a_Entry):
**93/96 calls bit-identical** over 2 races (deterministic). The structural fix
(`local_c0` drive-dir dot association: `fwd.z*(inv*ld8)` with `s2` pre-computed+reused)
took it from 58 RED → 3 RED; the gear/CVT float10 shim (`GearCvtCompute` keeping fVar5/fVar3
float10 across the 5-candidate loop) took 79 → 93. **15 of 38 .rdata constants** that the
standalone Integrate2.cpp's decimal literals mis-round were replaced with exact `Cf(0x..)`
bit patterns. Six float10-accumulator chains shimmed (GearCvtCompute, DriveForceAccum,
Accum60, AccumD0Num, FrictionUpdate, LinVelFactor).

**Why not C4:** the remaining 3 calls (24/82/84) diverge **1-8 ULP in the per-wheel
suspension force X** (`piVar12[0x1c]`) + one dependent `+0x9c0`. Root cause pinned
(disasm 0x468127..0x468337): the suspension force block keeps `local_bc`/`local_98`/`f5`/`f4`
in x87 registers as float10 across the compute+store; the C transcription rounds them to
float32 at different points (the significant X drive-dir exposes it; near-zero Y always
matched). The 4 closed float10 chains used naked shims; closing the suspension chain needs a
**full naked-asm shim of the whole grounded-suspension block** — large + high-regression-risk,
deferred. This is exactly the pre-acknowledged **[U-A6A-FLOAT10]** residual (`<=ULP in rare
cases`), so per [[feedback-no-overclaiming-c-levels]] A6a STAYS C2 in hooks.csv (NOT 96/96).

### Coverage (honest)
gm=6 all-4-grounded every call; the airborne path, helicopter type-C drive, brake branch
(BrakeForceAccum + call#7 transcribed, unhit in the throttle-only demo), spinning-powerup
vel-damp, and the boost-state machine were transcribed verbatim but NOT exercised in-race.

## WS-PHYS-C4-A6B+A3 — A3 is C4-GREEN; A6b grounded-gate GREEN but airborne body unexercised (2026-06-17, branch ws-phys-c4-a6b-a3)

Extended the proven A4/A5 `.asi` verbatim-LUT installed-hook lane to the last two
physics-chain fns. **4 of 5 physics fns are now C4** (A3, A4, A5; A6a + A6b stay C2).

### A3 FUN_0046b540 (spawn-time vehicle init) — **C4-GREEN, 24/24 bit-identical**
`__cdecl(int slot)` (ABI confirmed at caller 0x0040ed68 PUSH EDI;CALL + body
SUB ESP,8;PUSH ESI;...;MOV ESI,[ESP+0x10];CMP ESI,0x10). Runs ONCE per car at
spawn (NOT per-frame) -> a single-shot in-process A/B at the spawn call suffices.
The `.asi` body is a faithful transcription of 0x0046b540..0x0046ba97 reading the
record base `&DAT_008815a0 + slot*0xd04`: 4 handling-global writes, the
track-type table walk (`[0x613140]`/`0x613148` 5-int entries, stop @ entry[+0xc]==-1),
~80 record field writes, 4 FastSqrt radius loops (FUN_004c3b30 = RW fast-sqrt LUT,
forwarded LIVE), and a float10 mass/inertia reduction kept verbatim as a NAKED helper
(`A3_MassInertia`, 5 float10 values on the FPU stack). The 5 type-lookup fns
(FUN_00430790/0042f6a0/00431d70/0040ce80) forwarded LIVE (deterministic per spawn).
In-process A/B (A3 inline-JMP LIVE, canonical Arctic Quick-Battle, original-vs-mine on
the same slot via the OrigA3 trampoline): the FULL 0xd04 record + the 4 handling
globals (0x613108/14/30/3c) + the return value are **bit-identical across slots 0-3,
24/24 OK** (`re/analysis/phys_c4_evidence/A3_selftest_GREEN.txt`). U-7510/U-7511 are
DATA-semantic (what the table values / suspension consts MEAN) — non-blocking for C4
(behavior is bit-identical; meaning deferred). Promoted in hooks.csv.
- **OrigA3 trampoline gotcha (fixed):** the 5-byte inline-JMP at 0x0046b540 splits the
  first `MOV [0x613108],imm` (10 bytes, 0x46b544..0x46b54d). Resuming at 0x46b544 lands
  mid-instruction -> crash. Fix: the trampoline re-executes SUB ESP,8 + PUSH ESI + the
  WHOLE first MOV (EAX dead at 0x46b54e), then jumps to 0x0046b54e (first intact instr).

### A6b FUN_00468980 (aero-stabilize) — grounded-gate GREEN 4/4; airborne body UNEXERCISED -> C2
ABI resolved (the decomp's wall): at the A4 dispatch 0x00470943 — **ECX=record,
ESI=xform** (the world RwMatrix, NOT the record; `MOV ESI,[ESP+0x3c]`), [ESP+4]=dt.
(The lane's own A4_Body Call_A6b set ESI=record — a latent dispatch bug there; A4 is
C4 via its body-math self-test which stops before the dispatch tail.) Callee ABIs:
**FUN_004a3384 = CRT acos(double)->ST0**; **FUN_004c4d20 = `__cdecl RwMatrixRotate(out,
axis, angle_deg, mode)`** (internally FUN_004c3b90 inv-sqrt LUT + fsin/fcos -> routes
through the RW device, forwarded LIVE); FUN_004c39b0 = normalize. The body is a NAKED
verbatim transcription of 0x00468980..0x00468b34 with the 3 CALLs live-forwarded +
EXACT-bit .rdata consts. The exact-bit sweep CAUGHT the standalone
`Vehicle/AeroStabilize.cpp` using WRONG consts (`_DAT_005ccae0` is the double 180/pi
0x404ca5dcc0000000, NOT -2.0; the pitch bias `_DAT_005ccad8` is double 90.0, NOT 0.0).
**Verdict:** A6b's whole body is gated airborne (`+0x9e0==0.0`); in the canonical
Arctic race the car is all-4-grounded EVERY frame (`groundedCnt`==4.0, 179/179
telemetry), so only the grounded SHORT-CIRCUIT path is reachable -> A/B GREEN 4/4 but
the aero body (the real work) is UNEXERCISED. Per [[feedback-no-overclaiming-c-levels]]
A6b STAYS C2 (not C4 on "correctly does nothing"). Closing it needs an airborne
scenario; the available harness (nav_agent.js) overrides only control 4 (no steering),
so the car can't be driven off Arctic's ramps. Evidence:
`re/analysis/phys_c4_evidence/A6b_FINDINGS_2026-06-17.md` + `A6b_selftest_grounded_4of4.txt`.

### Physics-chain C4 tally: 3/5 (A3, A4, A5). A6a RED 93/96 (susp-force float10); A6b grounded-only.

## WS-PHYS-COVERAGE-SCENARIO — richer scenario closes A4's brake gap; airborne still flat-blocked (2026-06-17, branch ws-phys-coverage-scenario)

Drove a richer canonical scenario through the same `.asi` verbatim-LUT installed-hook
lane to exercise the transcribed-but-unhit branches. Full findings:
`re/analysis/phys_c4_evidence/COVERAGE_SCENARIO_FINDINGS_2026-06-17.md`.

**Driver:** AI-opponent multi-car coverage. The per-frame dispatcher (FUN_00470c70)
runs A4/A5/A6a/A6b for EVERY car record (slot 0 = player, 1-3 = AI), and the AI
opponents brake/steer/race on their own. Each self-test was re-armed with independent
per-branch sampling quotas (so the early all-grounded throttle-only frames stop flooding
the cap before a branch-novel call lands) and now logs the record's car `slot=`. Harness
`phys_c4_telemetry.py` window 12->45 s default + continuous accel (ctrl 4, confirmed by
`phys_c4_inrace_probe.py`) + optional steer; authoritative run 75 s, all 4 hooks live,
self-test on, A4 inline-JMP LIVE. Process hygiene: harness Popen-spawns its own MASHED
and kills ONLY that PID.

**Results (per-branch bit-confirm):**
- **A4 brake branch + input[5]>128 grip-else — GREEN, caveats REMOVED.** 53 brake calls
  (input[1]=76..155) + 16 input[5]=255 calls from braking AI records (slots 1-3),
  **0/104 mismatch**. A4's honest coverage gap is closed. hooks.csv updated.
  Evidence `A4_coverage_brake_GREEN.txt`.
- **A6a brake branch — exercised; A6a STAYS C2 (residual unchanged).** 32 brake calls;
  the 11/83 non-OK are ONLY the pre-known `[U-A6A-FLOAT10]` susp-force `wNfx`/`wNfz`
  residual (+ downstream vel) that runs on the grounded contact path regardless of
  throttle vs brake. BrakeForceAccum/call#7 themselves are bit-clean -> no new
  divergence class, no regression. Evidence `A6a_coverage_brake_93pct.txt`.
- **A5 airborne + random-surface, A6a airborne, A6b aero body — STILL UNREACHED.** The
  Quick-Battle Arctic arena is geometrically FLAT: probed exhaustively
  (`phys_c4_inrace_probe.py`) the player car driven full-throttle to speed ~2888 with
  every steer/ram combo NEVER lifted a wheel, and the racing AI opponents stayed
  all-4-grounded for 75 s (grounded==4.0 every sample). With zero airborne frames for
  any car, the airborne-gated branches and the Arctic-absent random-surface key can't be
  reached on this scenario. Closing them needs a circuit-race menu path to a ramp/jump
  track (the nav recipe is hardcoded Quick-Battle/Arctic) + a per-track jump map — a
  separate menu-RE effort; NO-GUESSING: not fabricated, and no synthetic +0x9e0=0.0 write
  (that is C3, not canonical). A5 stays C4 (caveat retained, now noted environmental);
  A6a/A6b stay C2.

**A6b verdict: NOT C4 (airborne body still unexercised; grounded gate GREEN 8/8).**

### Physics-chain C4 tally (unchanged): 3/5 (A3, A4, A5; A4 coverage now broader). A6a RED 93/96; A6b grounded-only.
