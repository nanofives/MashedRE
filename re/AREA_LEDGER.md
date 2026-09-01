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

### Fleet roster ROUND 1 (spawned 2026-09-01, star topology) — ALL CLOSED, swept in sweep/20260901

| area | child session id | branch | pool slot | notes |
|---|---|---|---|---|
| render | cmti6ok6y8wccqj1c7z0z1ble | area/render | Mashed_pool4 | round 2; an earlier render session (cmti3pv1z8ux6qj1c3zmpd3qh) was created DEAD in a daemon hiccup and abandoned (unstoppable, ignore it) |
| hud | cmti6ml7p8vvaqj1cj2tf7s8l | area/hud | Mashed_pool14 | round 1 |
| ai | cmti6ndt38w1iqj1c27t82i0b | area/ai | Mashed_pool2 | non-visual, parity N/A; round 1 |
| track | cmti6nqjc8w4eqj1cec4ocplb | area/track | Mashed_pool15 | game-mode slice, authoring-heavy; round 1 |
| frontend | cmti6o4d98w7qqj1c9tgq2c3o | area/frontend | Mashed_pool3 | guard scr1 118/118; round 1 |

All 5 bootstrapped 2026-09-01 with distinct pool slots (no collision) and distinct branches. GAP-2 (pool acquire) did NOT bite — all five slots bound cleanly.

### Fleet roster ROUND 2 (spawned 2026-09-01 post-sweep, star topology)

Based on the POST-SWEEP integrated tip `race/first-frame-parity` @ b5c17457 (FLEET_KICKOFF rule 1 — branching from a pre-sweep base is what caused the round-1 base divergence). Round-1 worktrees `.worktrees/area-*` were removed via `scripts/diag.py wt-remove` (all 5 branches verified merged first; NEVER `--force`) so the round-2 children can reuse the `area-{AREA}` names.

| area | child session id | branch | pool slot | notes |
|---|---|---|---|---|
| render | cmtiy2ect9qwpqj1crb903tla | area/render | (child-acquired) | 747 residue, largest area; carried round 1 |
| frontend | cmtiy3oum9r5fqj1cpyfngies | area/frontend | (child-acquired) | 80 residue; 2 C3s in round 1 (1 parked, U-9065) |
| ai | cmtiy4kef9rchqj1c8un3bo1q | area/ai | (child-acquired) | 29 residue; 1 C3 in round 1 |
| vehicle | cmtiy5ksp9rjfqj1cegp9nnpx | area/vehicle | (child-acquired) | NEVER SWEPT; 87 residue, 34 with scenarios |
| util | cmtiy6r049rq1qj1cr0xix213 | area/util | (child-acquired) | NEVER SWEPT; 334 residue, 222 with scenarios; RW math leaves are the known cheap-win shape ([[render-cheapest-wins-are-rw-math-leaves]]) |

**Not staffed this round, and why (recorded rather than silently dropped):** **hud** reached 2 consecutive dry rounds, the ledger's own MINED-OUT trigger, and its one lead B-0001 was REFUTED — revisit when the ~15 Rt2d rows are reclassified OUT. **track** is BLOCKED on U-9066: its course-load verifier FAILS at baseline with zero hooks, so a track child has no path to C3 until the PARENT recalibrates the assert. Staffing either would have been a guaranteed dry round.

Parent owns [[CROSS_AREA_BUS]] + this ledger; children report cross-area findings via send_to_session, never edit the bus. Spawn lesson: spawn ONE AT A TIME (a 5-way burst 500'd the endpoint and left one dead session). Watch for pool-slot contention — 5 children each acquiring a Ghidra slot + worktree + MASHED launch on one machine; GAP-2 (ghidra_pool.ps1) is unverified and children are told to STOP+report if acquire fails.

| area | residue<C3 | implemented .cpp | rounds run | dry streak | last parity | state |
|---|---|---|---|---|---|---|
| render | 746 | 166 | 3 | 0 | none (bit-identical) | ACTIVE (pilot) |
| hud | 77 | 36 | 2 | 2 | n/a | **MINED-OUT** (parent decision 2026-09-01, post-sweep) — 2 consecutive dry rounds; B-0001 REFUTED (band is first-party); ~15 rows pending Rt2d reclass-OUT; filed 3 cross-area findings. REVISIT WHEN: the Rt2d rows are reclassified OUT, which re-scopes the area. |
| ai | 29 | 45 | 4 | 0 | n/a (non-visual) | ACTIVE — 1 C3 VERIFIED (0x00415e20 AiSteeringAngleError, parent booted-race 3585-call 0-mism GREEN); found+fixed a real x87 ST0-leak reimpl bug en route |
| track | 60 | 3 | 1 | 1 | — | **BLOCKED** (U-9066) — the course-load verifier was authored in r1 but never run; the sweep ran it and it FAILS AT BASELINE with zero hooks (DAT_0063ba8c expected 1, got 3), so it cannot judge any hook. Not staffed in round 2. REVISIT WHEN: the parent recalibrates the assert to a PASSING zero-hook baseline. |
| frontend | 80 | 153 | 3 | 0 | scr1 118/118 GREEN (unchanged) | ACTIVE |
| vehicle | 87 | 1 | 0 | 0 | — | QUEUED (round 2, never swept) |
| util | 334 | 2 | 0 | 0 | n/a (non-visual) | QUEUED (round 2, never swept) |

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
| 3 | 2026-09-01 | 1 (004d8680) | 1 | none (string-compare leaf, not on the frame draw path — zero pixel change by construction) | no | Landed **RwStricmp** (0x004d8680) C2->C3. Continued the named-RW-math/util-leaf cheapest-win recipe. Chose it over the low-RVA feeder top-K (still all `undefined FUN(void)` side-effecting draw/state fns with no scalar observable) and over RwMatrixMultiply/Combine (0x004c4600/0x004c52f0 dispatch through the device fn-ptr DAT_007d4028+8 -> not self-contained, same reason round 2 skipped them). RwStricmp is a PURE LEAF (0 callees, no globals, no FP): canonical case-insensitive strcmp, fold 'A'..'Z' via SIGNED byte range (JL/JG), MOVSX-extended (int)a-(int)c on mismatch, NULL-guard. Plain-C transcription is bit-identical by construction (no x87/SSE rounding). Applied round-1 lesson #1: grep-confirmed RH_ScopedInstall(0x004d8680) is the SOLE installer (PromoLoop {0xf0,0x4d8680} is a data vtable-slot entry; StricmpThunk is the different disabled RVA 0x004b302f). **path1 run_diff.py rw_stricmp GREEN 12/12 NON-DEGEN** (log/diff_rw_stricmp.csv; discriminants: signed-ext 0xffffff60 for 0xc1-vs-'a' proves MOVSX, fold-boundary 6 for 'A'-vs-'[', both prefix dirs -99/99, NULL both args). NEW arg_type **stricmp_pair** authored in BOTH diff_template.js and verify_hook_install_template.js (SWEEP-CRITICAL, flagged in PROMOTION_QUEUE); ARG_TYPES.md regenerated (125 handlers). Gate: callee=pure-leaf exemption, caller=referencing fn RwEngineRegisterStringFunctions 0x004d8570 C3 (wires vtable slot +0xf0; sibling RwStreamWrite 0x004cbe80 C4). path2 (run_verify_hook) DEFERRED to parent frida-sweep (shared original\ .asi, not child-runnable on account2). Open visual defects (City black-road, Dump white-sky, terrain-ambient) UNTOUCHED -> re-filed OPEN, not dropped; T-ARCTIC still BLOCKED. |
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

- **Coverage target (S-DoD):** every executed track/course-load function at F-DoD, linked into `mashed_re.exe` and reached on the default course-load path, `track` STUBS section empty, formats round-trip.
- **Baseline (2026-08-31):** 60 residue<C3, all doc-only C2 (0 implemented .cpp among the residue; the only 3 track C3 are trivial global setters in `Util/PromoLoop_*.cpp` — already done). Game-mode name-match (`mode|champ|cup|race|challenge`) returns 0 rows: the mode/championship dispatchers are not separately named in the residue; they live inside the course-load chain (`FUN_00426e10` / `FUN_0040d270`).
- **Round-1 finding (harness gap, evidence-backed):** track has **no free synthetic C2→C3 wins**. Decoded the cheapest candidates (0047b9e0, 0040d440, 0040d020 + callees) via analyzeHeadless/pool15:
  - `0047b9e0` forwards `(p1,p2)` to `FUN_00496c10`, which is `void FUN_00496c10(void)` (render-quad emitter, gated on `DAT_00636b70`). Args are ignored by the callee and there is **no scalar observable** → synthetic path1 has nothing to diff (rejected by `run_diff.py:465` no-op guard).
  - `0040d440` (Course::LoadCurrent) calls `FUN_0040d270` (Course::Finish, ~100-line load/teardown driver: file work, printf, `**(param_1+param_2*4)` derefs). Not synthetic-safe.
  - `0040d020` derefs a track-table entry into `FUN_00426e10`, which opens the track `.piz` + runs `COURSE.LUA`/`LAPDATA.LUA` (heavy file I/O). Not synthetic-safe.
  - Lua config-handler cluster (`0047a1b0` family: Occluder/Mts/Clump/Spline/AI-Bsp filenames) reads a live `lua_State` (`FUN_004b6fc0`/`FUN_004b70d0`) populated only during `COURSE.LUA` execution → needs a Lua scenario, not synthetic re-calls.
  - **Net:** the residue is course-**load-time** code. C3 promotion needs a course-load scenario verifier (path2-install + `drive_to_race` load with a load-integrity assert — C4-shaped), NOT synthetic path1. Reported to parent as a HARNESS_BACKLOG gap. No unverifiable heavyweight loader code authored into the tree (respects the acceptance bar).
- **Revisit condition (if MINED-OUT):** a course-load scenario verifier lands in the Frida harness (install hook, drive into a race, assert the track loads identically) — then the load-dispatcher cluster becomes promotable.

### Rounds

<!-- Append one row per round: round | date | candidates | landed C3+ | parity delta | dry? | note -->

| round | date | candidates | landed C3+ | parity delta | dry? | note |
|---|---|---|---|---|---|---|
| 1 | 2026-09-01 | 8 decoded; VERIFIER + 3 thin thunks authored+linked+queued | 0 landed (verifier + 3-thunk cluster queued; parent verifies) | n/a (non-visual load path) | no | Preflight ✓ (anchor bdcae093; pool15; baseline build compiles both targets — only shared-`original\` .asi deploy fails, expected in a worktree; r1 build re-verified REAL not a GAP-7 false-pass: .asi 02:28:50 bumped, 16 CourseLoadDispatch.obj refs in map; switched to PowerShell build). Scoped 60 C2 residue: all course-load-time (no free synthetic C2→C3 wins, same as render round-0). Per parent's steer: (1) PRIORITY — authored the **course-load VERIFIER** by extending the canonical lane `re/frida/scenario_launch.py` additively (rpc `courseLoadAsserts` + flag `--assert-course-load`): drives to loaded-course via the existing phase-2 poke, asserts 3 cited post-load observables (DAT_0066d704==1 @0x00426e10 tail; DAT_0063ba8c==1 @0x0040d270 LAB_0040d3c3; DAT_0063ba78==DAT_0063ba7c @0x0040d440). Queued NEEDS-BOOTED-RACE VERIFIER. (2) Finished the **3 thin thunks** (0x0040d020, 0x0040d440, 0x0047b9e0) in `Track/CourseLoadDispatch.cpp`, linked into asi_sources.rsp, build ✓ (all 3 exported+auto-registered, .asi bumped), 3 hooks_registry.py entries (parses, 1092 hooks), queued NEEDS-BOOTED-RACE CLUSTER. PULLED heavyweight 0x0040cea0 back to DEFERRED per parent's acceptance-bar call (author after verifier green); 0x0040d110 stays DEFERRED (fragile stack-struct callees). Did NOT spawn MASHED (parent runs the verifier + cluster). NOT dry: authored the leverage verifier + advanced LINKED coverage +3. |

## frontend

- **Coverage target (S-DoD):** every executed frontend function at F-DoD, TU linked into `mashed_re.exe` and reached on the default path, `frontend` STUBS section empty, formats round-trip.
- **Visual target:** scr1 draw-list parity stays GREEN 118/118 (reference recipe — a RED here means a round broke composition).
- **Open visual defects:** none currently filed for frontend.
- **Worktree:** `.worktrees/area-frontend` (branch `area/frontend`); pool slot `Mashed_pool3`.
- **Cheapest-win note:** the 3 implemented+linked residue rows are NOT free diffs — TextSpriteScaled (done r1) was gated on U-0459; MenuMenusBC (`0x0042f8d0`) synthetic force-call TIMES OUT (calls the real quad drawer 5x, needs render context — canonical-scenario validation required, not a synthetic diff); HardwareShowIntroVideo (`0x00495350`) has Sleep+infinite-loop + canonical AV crash + 8 unported callees.

### Rounds

<!-- round | date | candidates | landed C3+ | parity delta | dry? | note -->

| round | date | candidates | landed C3+ | parity delta | dry? | note |
|---|---|---|---|---|---|---|
| 1 | 2026-09-01 | 3 examined (004739f0 TextSpriteScaled, 0042f8d0 MenuMenusBC, 00495350 HardwareShowIntroVideo) | 1 (004739f0) | none (no runtime code touched — scr1 stays 118/118 by construction) | no | Worktree off `main` was 49 commits behind the parent branch (missing all area-loop tooling + recent frontend work); reset the clean worktree onto `race/first-frame-parity` tip before scoping. Landed **TextSpriteScaled C2->C3**: resolved U-0459 via analyzeHeadless XrefRange on `0x005ceac4` — NO code writer (4 refs all read, 2 in FUN_004739f0), a static `.data` const `0x3b122549` = **1/448**, so `param_11==2` normalizes Y against a 448-unit frame (vs default Y `_005cc560`=1/480, X `_005cd5a8`=1/640); static => port's hardcoded read is bit-identical. Path1 GREEN 10/10; path2 install FULL PASS (interceptor 2/2) after fixing a real harness gap: `verify_hook_install_template.js` callFn had no `draw_quad_observe` case so the 12-arg vector fell through to `fn(input)` -> "bad argument count" (same class as GAP-5 0-arg). Fix marshals like `diff_template.js` packArgs. re-classify C2->C3 applied (hooks.csv/CHANGELOG/UNCERTAINTIES/source); queued to PROMOTION_QUEUE. MenuMenusBC + HardwareShowIntroVideo left as documented hard-blocked rows (see cheapest-win note). Evidence `re/analysis/frontend_u0459_005ceac4_resolution.md`. |
| 2 | 2026-09-01 | 8 decoded (00423b00, 00425ea0, 00425fa0, 00426460, 00426b40, 00426c50, 004277a0, 0043dfd0) | 0 | none | **yes** (dry #1) | Scoping/decode round: batch-decoded the top doc-only C2 residue to find the next cheapest win. Finding: NO free getters/standard-conv leaves remain — the rows are `(void)`-signature side-effecting functions (clump destroy, render/update ticks) calling other FUN_s. Best candidate is **`0x004277a0`** (pure leaf, 0 callees, 2 C3 callers MenuMenusBA/BB): a deterministic control-code remap over a length-prefixed short array. Its ONLY blocker is that no existing arg_type delivers its EAX=src-ptr / EBX=dst-ptr convention. Fully specced a new `eax_ptr_ebx_outbuf` arg_type (small delta from `esi_idx_ecx_outbuf4`) + naked reimpl + test vectors in `re/analysis/frontend_004277a0_c3_plan.md` — round 3 can author it with zero re-discovery. Dry by strict definition (0 C3 landed), but the frontier is now mapped and the next win is teed up. |
| 3 | 2026-09-01 | 1 (004277a0, per the r2 spec) | 1 (004277a0) | none (no `mashed_re.exe` code touched — TextCtrlCodeRemap is a dev-hook .asi TU; scr1 unaffected) | no (resets dry streak) | Executed the r2 spec. Authored **TextCtrlCodeRemap** (`mashedmod/src/mashed_re/Frontend/TextCtrlCodeRemap.cpp`) as a VERBATIM transcription of the `0x004277a0..0x00427832` disasm (pulled the exact bytes+disasm from the binary; pure integer, no FP → dst bit-identical by construction). Built the new `eax_ptr_ebx_outbuf` arg_type in BOTH `diff_template.js` and `verify_hook_install_template.js` (register-conv trampoline: `push ebx; mov eax,src; mov ebx,dst; call target; pop ebx; ret`). Linked into `asi_sources.rsp`, built via PowerShell (`.asi` timestamp advanced, TextCtrlCodeRemap.cpp compiled, no errors). **path1 GREEN 13/13** (`log/diff_text_ctrl_code_remap.csv`) — every remap arm + passthroughs + long mixed string. re-classify C2->C3 applied; queued to PROMOTION_QUEUE tagged SWEEP-CRITICAL (both JS handlers must ride the sweep). **path2 DEFERRED to the parent frida-sweep**: run_verify_hook uses the autoloaded shared `original\mashed_re_dev.asi`, which the worktree cannot update without clobbering siblings' newer hooks — the sweep's canonical rebuild resolves it. C3 gate met on path1 + structural (RH_ScopedInstall live, callers MenuMenusBA/BB C3, pure-leaf callee exemption). |

---

## vehicle

- **First swept:** round 2, 2026-09-01 (never staffed before).
- **Coverage target (S-DoD):** every executed vehicle function at F-DoD, TU linked into `mashed_re.exe` and reached on the default path, `vehicle` STUBS section empty.
- **Baseline (area_residue.py, post-sweep tip b5c17457):** 87 rows below C3 — implemented .cpp 1, linked 1, has-scenario 34, green-unpromoted 0, DEMOTED 0, BLOCKED 0, doc-only 86.
- **Why staffed:** 34 of 87 already carry a scenario, and the area has almost no implementation yet (1 .cpp), so the cheap-win queue is unexplored rather than mined out.
- **Watch:** the replay/ghost family (`Replay::LapFinish` 0x00411870, `Ghost::PlaybackTick` 0x00411ae0, `Ghost::SetupRender` 0x00411ce0, `Replay::CreateOrLoad` 0x00411d90) sits at the top of the queue and is live-state shaped — expect NEEDS-BOOTED-RACE, not synthetic path1.

### Rounds

| round | date | candidates | landed C3+ | parity delta | dry? | note |
|---|---|---|---|---|---|---|

---

## util

- **First swept:** round 2, 2026-09-01 (never staffed before).
- **Coverage target (S-DoD):** every executed util function at F-DoD, TU linked, `util` STUBS section empty. Non-visual, so parity is N/A — the gate is bit-identity, not a scoreboard.
- **Baseline (area_residue.py, post-sweep tip b5c17457):** 334 rows below C3 — implemented .cpp 2, linked 2, has-scenario 222, green-unpromoted 0, DEMOTED 2, BLOCKED 0, doc-only 332.
- **Why staffed:** 222 of 334 carry a scenario and the area is dominated by RenderWare math leaves, which is the exact shape that produced the cheapest round-1 win ([[render-cheapest-wins-are-rw-math-leaves]]: named RW math leaves, capstone raw-disasm + naked verbatim x87). `RwMatrixInvert` 0x004c4dc0 (C2, 23 callers) is in this area and is the direct caller of the 0x004c4eb0 cofactor path landed in sweep/20260901.
- **Watch:** the x87 ST0 leak class ([[x87-st0-float10-fnptr-void-leak]]) is a live hazard for exactly this kind of float-returning RW leaf — never forward a `float10`-returning function through a `void` fn-ptr.

### Rounds

| round | date | candidates | landed C3+ | parity delta | dry? | note |
|---|---|---|---|---|---|---|
