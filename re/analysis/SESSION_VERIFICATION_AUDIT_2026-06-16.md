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
| A5 ForceIntegrator | 0x0046ddb0 | C2 — lane-ready but LARGER (~3KB) + harder self-test: writes SHARED globals (DAT_008815xx wheel scratch) and calls **RNG** `FUN_00472650` (random-impulse +0x2c0) → the full-fn A/B needs shared-global snapshot/restore + RNG-seed control (or exclude +0x2c0). EDI=record; calls 004c3df0/004c4d20/004c3ac0/004c39b0 (live LUT). Own focused session. |
| A6a Integrate2 | 0x00467650 | C2 — the big x87 float10 body; ESI=record; same full-fn self-test needs + [U-A6A-ST0] implicit-ST0 smoother. Own session. |
| A6b AeroStabilize | 0x00468980 | C2 — small but ECX=ESI=record + transcendental `FUN_004a3384` (asin/acos) + opaque `FUN_004c4d20` (no visible args, FPU/device matrix). Own session. |
| A3 VehicleInit | 0x0046b540 | C2 — spawn-time; deterministic table writes; not on the per-frame hot path. Lane applies but lower priority. |

### Remaining work (next sessions)
The A4 lane (register-ABI trampoline + live-LUT forwarders + early-return original-body
trampoline + in-process bit-A/B) is the reusable template. For A5/A6a/A6b each session
must: (1) author the register-ABI `.asi` verbatim hook forwarding LUT calls to the live
originals; (2) extend the self-test to a FULL-function A/B (snapshot+restore the shared
globals each writes; control/seed `FUN_00472650` RNG for A5, or exclude the random-
impulse +0x2c0 fields); (3) capture drive-path calls and bit-compare; (4) promote GREEN
subset via re-classify. A5 next (task priority), then A6a, A6b, A3.
