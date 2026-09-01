<!-- PER-AREA COVERAGE LEDGER -->
<!-- Owned by the PARENT orchestrator. One section per canonical subsystem being swept.  -->
<!-- This is the single keyed store the recurring loop reads to answer "have we come back -->
<!-- enough times, and is this area done." It is NOT hooks.csv (that is per-RVA truth);   -->
<!-- it is per-AREA state: rounds run, residue burndown, parity history, dry counter.     -->
<!--                                                                                      -->
<!-- Coverage is tracked in THREE columns because a C3 row can be pure documentation:     -->
<!--   documented  row is >=C3 in hooks.csv                                               -->
<!--   implemented row's file column points at a real mashedmod/src/mashed_re/**.cpp      -->
<!--   linked      that .cpp is in build.bat (exe target) OR asi_sources.rsp              -->
<!-- S-DoD (ROADMAP.md) needs LINKED + default-path-reached + STUBS section empty, so the -->
<!-- loop's exit gate reads the linked column, not documented.                            -->
<!--                                                                                      -->
<!-- Dry counter: a round that lands 0 new C3+ AND does not improve parity increments it. -->
<!-- 2 consecutive dry rounds => area marked MINED-OUT, scheduler moves to the next area. -->
<!-- MINED-OUT is not COVERED: it means "no cheap wins left", revisit condition recorded. -->

# Area coverage ledger

Baseline snapshot: **2026-08-31** (from `hooks.csv`, `scripts/progress.py`, `build.bat`).
Round order: **render** (pilot) -> hud -> ai -> track -> frontend.

### Fleet roster (spawned 2026-09-01, star topology — all ACTIVE)

| area | child session id | branch | pool slot | notes |
|---|---|---|---|---|
| render | cmti6ok6y8wccqj1c7z0z1ble | area/render | Mashed_pool4 | round 2; an earlier render session (cmti3pv1z8ux6qj1c3zmpd3qh) was created DEAD in a daemon hiccup and abandoned (unstoppable, ignore it) |
| hud | cmti6ml7p8vvaqj1cj2tf7s8l | area/hud | Mashed_pool14 | round 1 |
| ai | cmti6ndt38w1iqj1c27t82i0b | area/ai | Mashed_pool2 | non-visual, parity N/A; round 1 |
| track | cmti6nqjc8w4eqj1cec4ocplb | area/track | Mashed_pool15 | game-mode slice, authoring-heavy; round 1 |
| frontend | cmti6o4d98w7qqj1c9tgq2c3o | area/frontend | Mashed_pool3 | guard scr1 118/118; round 1 |

All 5 bootstrapped 2026-09-01 with distinct pool slots (no collision) and distinct branches. GAP-2 (pool acquire) did NOT bite — all five slots bound cleanly.

Parent owns [[CROSS_AREA_BUS]] + this ledger; children report cross-area findings via send_to_session, never edit the bus. Spawn lesson: spawn ONE AT A TIME (a 5-way burst 500'd the endpoint and left one dead session). Watch for pool-slot contention — 5 children each acquiring a Ghidra slot + worktree + MASHED launch on one machine; GAP-2 (ghidra_pool.ps1) is unverified and children are told to STOP+report if acquire fails.

| area | residue<C3 | implemented .cpp | rounds run | dry streak | last parity | state |
|---|---|---|---|---|---|---|
| render | 747 | 166 | 2 | 0 | none (bit-identical) | ACTIVE (pilot) |
| hud | 77 | 36 | 2 | 2* | n/a | ACTIVE — no cheap wins; ~15 rows pending Rt2d reclass-OUT (B-0001 REFUTED, band is first-party); filed 3 cross-area findings (*2nd dry is a parent-directed calibration, not a cheap-win hunt — parent to decide MINED-OUT) |
| ai | 29 | 45 | 4 | 0 | n/a (non-visual) | ACTIVE — 1 C3 VERIFIED (0x00415e20 AiSteeringAngleError, parent booted-race 3585-call 0-mism GREEN); found+fixed a real x87 ST0-leak reimpl bug en route |
| track | 60 | 3 | 1 | 1 | — | ACTIVE — no free synthetic wins; needs course-load verifier (2nd dry -> MINED-OUT, revisit=verifier built) |
| frontend | 82 | 152 | 0 | 0 | scr1 118/118 GREEN | ACTIVE (merging infra base) |

State vocab: QUEUED | ACTIVE | MINED-OUT | COVERED.

---

## render

- **Coverage target (S-DoD):** every executed render function at F-DoD, no env-gated fallbacks, TU linked into `mashed_re.exe` and reached on the default path, `render` STUBS section empty, formats round-trip.
- **Visual target:** no open render defect RED; parity scoreboard not regressed vs previous round.
- **Open visual defects (coverage gate — must close or re-file, not drop):**
  - City black-road (filed cb9acad4 lineage / `re/analysis/`).
  - Dump white-sky.
  - Terrain ambient overbrighten — `TrackRenderer.cpp:228`, coupled to Arctic sea ([[terrain-ambient-fill-overbrightens]]).
  - Water-scoped ambient fold — merged, defaulted ON ([[geomlight-darkens-arctic-sea]]).
- **Known blocker:** T-ARCTIC — no pose-matched original Arctic frame; mode-3 nav unimplemented in `race_draw_burst.py`. Per-track parity on Arctic cannot close until this is fixed. Recorded, not silently skipped.
- **Camera slice inside render:** tracked by NAME (residue rows point at `.md` docs, not `Camera/*.cpp`, so a file-prefix filter reads a false zero). Metric: `area_residue.py --subsystem render --name-match "cam|view"` -> 5 residue below C3 at baseline (1 implemented+linked: `CameraRecomputeProjCoeffs`). "Cameras covered" = that command empty.

### Rounds

<!-- Append one row per round: round | date | candidates | landed C3+ | parity delta | dry? | note -->

| round | date | candidates | landed C3+ | parity delta | dry? | note |
|---|---|---|---|---|---|---|
| 1 | 2026-08-31 | 1 (004c5890) | 1 | none (allocator, no visual output change) | no | Proved the heavy half end-to-end: build ✓ (both targets) -> run_diff.py path1 GREEN 10/10 with allocator_nonnull live -> run_verify_hook path2 JMP install verified -> re-classify 004c5890 C2->C3. The "harness extension" the round set out to author already existed (diff_template.js:4963); the blocker was a STALE same-day demotion note, not missing code. New gap found AND FIXED same round (GAP-5): run_verify_hook.py passed path2_tests as call args, so 0-arg functions FAILed call-through; fixed in verify_hook_install_template.js callFn (honor signature.args.length===0 -> fn()); re-verified path2 FULL PASS 3/3. |
| 2 | 2026-09-01 | 1 (004c4eb0) | 1 | none (bit-identical reimpl of a previously extern call-through leaf — zero pixel change by construction) | no | Landed RwMatrixInvert (004c4eb0, cofactor 3x3 inverse + inverse translation) C2->C3. Chosen over the feeder's low-RVA top-K: those are all `undefined FUN(void)` side-effecting draw/state functions with callee stubs that do NOT Frida-diff on a return value (confirms round 0's "no free wins" for the draw class); the real cheapest verifiable wins are the descriptively-named RW math leaves (RwMatrix*). Multiply/Combine dispatch through a device fn-ptr (DAT_007d4028+8) so are not self-contained; Invert is a pure leaf (sole global 0x005cc320=1.0f). Verbatim naked-x87 transcription (bit-identity needs the 80-bit x87 op order; plain-C /arch:SSE2 would round intermediates and diverge). path1 run_diff GREEN 5/5 bit-identical (4 invertible + 1 singular det==0 guard); path2 FULL PASS (JMP install + interceptor 2x + call-through GREEN). RESOLVED U-4930 (structural, exact cofactor indexing) by raw-disasm ground truth. Harness fix: added a fmt_desc_pair_compare case to verify_hook_install_template.js callFn (was falling through to fn(input) -> "bad argument count", same class as round-1 GAP-5) — SWEEP-CRITICAL, flagged in PROMOTION_QUEUE. Deploy contention noted: original\mashed_re_dev.asi is a fleet-shared single deploy target; a sibling's stale build was there, path2 required deploying mine (run_diff loads build/ directly so path1 was unaffected). Open visual defects (City black-road, Dump white-sky, terrain-ambient) UNTOUCHED this round -> re-filed OPEN, not dropped; T-ARCTIC still BLOCKED. |
| 0 (shakeout) | 2026-08-31 | 0 | 0 | none | n/a | Manual end-to-end walk. Gate ✓ (.unpatched=bdcae093 matches anchor). Decode path ✓ on account2 (analyzeHeadless + 16 pool slots). Found+fixed 4 feeder gaps in area_residue.py: GAP-1 (downgraded) DecompPC.java absent from THIS working tree but tracked in HEAD (13562b) — a fresh git worktree checkout gets it; restored the local copy, no real gap; GAP-2 (open) ghidra_pool.ps1 status raw-invocation path-mangling — verify via skill from a real worktree; GAP-3 feeder ranked a BLOCKED-ON-ENV row (004c1a70, x87 naked) #1 — now reads frida_diff col, sinks BLOCKED; GAP-4 feeder boosted DEMOTED rows via degenerate all-True artifact CSVs — now reads notes, sinks DEMOTED, trusts only literal green* verdicts. **Key finding: render has NO free C2->C3 wins.** Residue = 732 doc-only C2 (need authoring), 12 DEMOTED (need harness extension, chiefly nonnull-only pointer-comparison arg_type — unblocks the allocator class incl. 004c5890 RwTexDictionaryCreate), 1 BLOCKED, 2 implemented-but-problematic. Heavy half (build/Frida/parity) not yet proven — requires either authoring a doc plate (e.g. 00401f10, has stubs S-3120/S-3121) or the harness extension. |

---

## hud

- **Coverage target (S-DoD):** every executed hud function at F-DoD, linked into `mashed_re.exe`, reached on the default path, `hud` STUBS section empty, formats round-trip.
- **Worktree/slot:** `.worktrees/area-hud` on `area/hud`, pool slot `Mashed_pool14`, log dir `log/area-hud`.
- **Structural facts (round 1):**
  - Residue = 77 below C3; only **2 implemented+linked** and both are traps: `00552e40 FontCtx_FlushMatrix` (Frida GREEN is `crash_equal_ok` NULL-cam, body unverified — false-green) and `00552b60 FontSys_InitSeq` (deadlocks at quiescent menu). The remaining 75 are doc-only C2 needing authoring.
  - The `0x00553000-0x00557fff` font-vector band (~15 rows: `00554010/150/200/390`, `00555830`, `00556780`, `00556e40`, `005571c0/e0`, `FontSys_*`) is **suspected vendored RenderWare Rt2d** (module-vendor-doubt in the plates; FlushMatrix's callers `00552890/00552920` already reclassed `third-party-library[renderware]` Rt2d). A confirmed Rt2d calibration reclasses-OUT these -> biggest single lever on hud residue. **Cross-area (hud->render), reported to parent (F1).**
  - The real game-hud functions (`0x0041xxxx` VehicleIcons/label-trail, `0x0047xxxx` event-markers) are `void`/GPU-side-effecting/register-convention/stateful per-frame emitters — none is a clean bit-identity diff; they are asm-exact port work, not cheap wins.
- **Open findings (see `re/analysis/area_hud_round1_scoping.md`):** F1 Rt2d band reclass; F2 `004a2c48`=`__ftol` not "QPC tick"; F3 `004726f0` returns float on ST0 (plated `void`).
- **Parity:** no hud-specific recipe run in round 1 (scoping only).

### Rounds

<!-- round | date | candidates | landed C3+ | parity delta | dry? | note -->

| round | date | candidates | landed C3+ | parity delta | dry? | note |
|---|---|---|---|---|---|---|
| 1 | 2026-09-01 | 9 decoded (00552e40, 00552b60, 004128f0, 00413b80/bb0/cb0/f50, 00412cf0, 00455b50, 0047d640, 0047def0, +leaves 004a2c48/00412f30/004726f0) | 0 | n/a (scoping) | yes | No clean cheap C2->C3 win in hud (mirrors render round-0). FlushMatrix GREEN = crash_equal_ok false-green (rejected); FontSys_InitSeq deadlocks; VehicleIcons trio + 004128f0 void/GPU/register-convention; 00412cf0 gate-satisfied but Ghidra decomp is LOSSY (drops 004726f0's ST0 return + the K²·_DAT_005cd04c chain into byte+0x27 — needs asm-exact port). 3 cross-area findings reported to parent (F1 Rt2d band, F2 __ftol label, F3 004726f0 float return). Added `DisasmPC.java` (account2 asm dumper, no MCP). |
| 2 | 2026-09-01 | B-0001 Rt2d calibration (9 Group-B band rows + 8 callee classifications) | 0 | n/a | yes (parent-directed calibration, not a cheap-win hunt) | **REFUTE**: the font-vector band is FIRST-PARTY, not vendored RW. Callees that make it "look RW" are first-party render/boot (004cd070 RwRenderPrimitiveSubmit render-C2, 004cd140 render-C3, 005c4c60/4d30/4da0 boot); FGDC20.RWF is a Mashed asset (Font36.piz); the port already reimplements it (D3d9Render/MashedFont.cpp). No reclass-OUT queued — would discard first-party work + doesn't shrink residue. Also flagged the ~15-row list conflated Group A (named/C3-impl first-party — never reclass) with Group B. B-0001 -> REFUTED. Evidence: re/analysis/area_hud_round2_rt2d_calibration.md. Held before HUD-render verifier pending parent (premise flipped). |

## ai

- **Coverage target (S-DoD):** every executed AI function at F-DoD, TU linked into the `.asi`/exe and reached on the default path, `ai` STUBS section empty. Non-visual: **parity gate N/A**, exit gate is coverage + Frida diff only.
- **Area shape (round 1):** 30 residue<C3, of which **29 are doc-only C2** (need a `.cpp` authored) and **1 is implemented+linked** (`00415e20 AiSteeringAngleError`, whose sibling `004161e0 AiSplineTargetInit` is also implemented+installed in `Ai/AiTargeting.cpp` but its hooks.csv `file` col still points at the `.md`, so it reads `mapped`/`link=no` — tracker drift, not a real gap).
- **No synthetic cheap win.** Every AI residue row reads/writes live vehicle+track state, so a hook-bypassed path1 diff on unpopulated state is a degenerate green ([[scratch-field-false-green]]; the out3_idx audit demoted `VehicleVelocityWorldGet` for exactly this). The three AI functions that reached C3 each used a **live race**: `AiLineOfSight`/`AiWallAhead` via in-process A/B self-tests (`MASHED_AI_*_SELFTEST`, scenario=race, ad-hoc — no push-button runner), `VehicleVelocityWorldGet` via a contrived-state `run_diff_scenario` (`cache_setter_observe`). The spline/targeting leaves are not cleanly contrive-able: they call **unverified C2 callees** (`FUN_00443dc0`, itself a residue row) with large unenumerated global footprints.
- **Stale-blocker cleared (finding, round 1):** `AiSteeringAngleError`'s hooks.csv note reads "still wedges later — NOT fully resolved, see U-9025." **U-9025 is RESOLVED** (2026-07-28, commit f1855ad9): the wedge was a dropped implicit return in `FUN_005aef00` (AudioThreadDescInit), not AI; the hooked build completes 10/10 races. So `AiSteeringAngleError` is **no longer wedge-blocked** — it is a pure float-returning candidate one in-race self-test A/B from C3 (mirror `AiLineOfSight.cpp`'s `LosDispatch`, compare the returned float bits, passthrough orig).
- **account2 blocker for the child:** the AI C3 route requires a **booted race**; running `MASHED.exe` + Frida is keep-local execution that prompts/stalls on account2, so the child cannot complete a race diff. The race-run half belongs to the parent's scenario/sweep lane (or an account3 run). Reported to parent.

### Rounds

<!-- Append one row per round: round | date | candidates | landed C3+ | parity delta | dry? | note -->

| round | date | candidates | landed C3+ | parity delta | dry? | note |
|---|---|---|---|---|---|---|
| 1 | 2026-09-01 | 30 scoped (top: 00415e20, 004161e0) | 0 | n/a (non-visual) | yes | Characterization + gate. Gate: worktree area-ai on Mashed_pool2, anchor bdcae093 matches; preflight staleness was a shared-pool CWD path artifact (real `.rep` in main `mashed_pool`). ~~baseline build.bat exit 0~~ **CORRECTED in r2: that was a FALSE PASS** — `cmd /c "mashedmod\build.bat"` from Git Bash mangled the path and ran nothing (log was a 3-line cmd banner, `mashedmod/build/` never created); the real build was run+verified in r2. No synthetic cheap win exists — all 30 residue read live state; the C3 route is a booted-race self-test. **Finding: U-9025 stale** (resolved 2026-07-28 as an audio-thread bug), so `AiSteeringAngleError` is unblocked, not wedged. account2 child cannot boot a race (prompt/stall); race-run half reported to parent for the scenario/sweep lane. `AiSplineTargetInit` mapped->impl drift noted. 0 landed because the only acceptance (a live-race Frida diff) is not child-executable on account2, not fabricated GREEN. |
| 4 | 2026-09-01 | 1 (00415e20 promote) + fleet x87 scan | **1 (FIRST ai C3)** | n/a (non-visual) | no | **Parent VERIFIED GREEN** (booted race @6ba264a6: 3585 in-race calls, 0 mism, cars racing normally, no freeze) and authorized the promotion. Re-classified 0x00415e20 AiSteeringAngleError C2->C3 (hooks.csv C2->C3 + scenario=race + frida_diff=original/ai_steer_selftest.log; CHANGELOG prepended below the ENTRIES marker; no open uncertainties/stubs on the fn). ai area's FIRST C3, and it validated the parent's booted-race lane end-to-end. Then ran the parent's fleet-wide x87 scan of the ai reimpls (Ghidra signatures via Mashed_pool2): the only ST0-returning RW-math forwards are Vec2Length 0x004c3bf0 / Vec2Normalize 0x004c3c60 / RwV3dNormalize 0x004c39b0 — all now declared `float` (AiNavHooks fixed this class 2026-07-01; AiSteeringAngleError was the straggler, fixed r3; AiStandalone does normalize inline w/ no fn-ptr). Every other void-declared forward (0x0046d6a0/6d0 undefined4 EAX, 0x004c3df0 undefined4 EAX ptr, 0x00443dc0/443300/416230 genuinely void) does NOT return ST0 -> no other latent leak. ai x87-leak class CLOSED. |
| 3 | 2026-09-01 | 1 (00415e20 root-cause debug) | 0 (re-queued, fix pending parent re-run) | n/a (non-visual) | no | **Root-caused the parent's booted-race freeze.** Parent r2 run: only AiSteer_Entry installed, yet the sim froze (all cars grounded=0 vel=0, steering fired once then stopped). NOT a data side-effect (parent's hypothesis). Decoded the reimpl's callees (analyzeHeadless, Mashed_pool2): 0x0046d4a0 writes only its local out-param (safe); **0x004c39b0 (RwV3dNormalize) is `float10 FUN_004c39b0` — returns a float10 in ST0**, but the reimpl forwarded it through a `void` fn-ptr. A void decl never pops ST0; AiSteeringAngleError calls it TWICE, so the in-thread double-invoke overflows the 8-deep x87 stack within a frame -> NaN FP -> whole-sim freeze. The classic [[feedback_x87_st0_float10_return_fnptr]] bug AiLineOfSight.cpp warns about. FIX: declared fn_norm_t/call_004c39b0 to return float (emits FSTP pop). Also verified the one shared write (0x0046d510 velocity block) is idempotent so the double-invoke can't corrupt it. Rebuilt GREEN both targets. Re-queued area-ai-r2 as NEEDS-BOOTED-RACE-RERUN. This is a REAL correctness fix to a C2 reimpl, surfaced only because the harness invoked it in-thread. |
| 2 | 2026-09-01 | 1 (00415e20 AiSteeringAngleError) | 0 (queued NEEDS-BOOTED-RACE) | n/a (non-visual) | no | Parent chose option (b): child does the non-stalling half, no race boot here. **Authored + built the in-race A/B self-test harness for AiSteeringAngleError**, mirroring Ai/AiLineOfSight.cpp LosDispatch. Decoded the prologue via capstone off MASHED.exe.unpatched (SUB ESP,0x10 / PUSH ESI / MOV ESI,[ESP+0x18] / LEA @0x00415e28) to build the OrigSteeringAngleError trampoline; SteerDispatch does the bit-exact float A/B and returns orig (safe passthrough); naked entry AiSteer_Entry installed at 0x00415e20. **Build now genuinely verified via PowerShell**: compile+link GREEN both targets (mashed_re.exe 1.68MB + mashed_re_dev.asi 857KB), AiTargeting.cpp only the benign C4996 getenv warning (same as LOS). The lone failure was the deploy-to-`original\` step, environmental in a worktree (no original/ junction). Queued to re/PROMOTION_QUEUE.md as area-ai-r2 (NEEDS-BOOTED-RACE) + area-ai-r2-drift (AiSplineTargetInit re-classify request). NOT dry: authored+built a real C3 harness; only the race-run acceptance is parent/account3-gated. |

## track

_QUEUED — game-mode slice; only 3 implemented .cpp, expect a long area._

## frontend

_QUEUED — tail; draw-list parity already GREEN-capable (scr1 118/118)._
