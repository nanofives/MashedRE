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

| area | residue<C3 | implemented .cpp | rounds run | dry streak | last parity | state |
|---|---|---|---|---|---|---|
| render | 748 | 165 | 1 | 0 | none (round 1) | ACTIVE (pilot) |
| hud | 77 | 36 | 0 | 0 | — | QUEUED |
| ai | 30 | 45 | 0 | 0 | n/a (non-visual) | QUEUED |
| track | 60 | 3 | 0 | 0 | — | QUEUED |
| frontend | 82 | 152 | 0 | 0 | scr1 118/118 GREEN | QUEUED |

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
| 0 (shakeout) | 2026-08-31 | 0 | 0 | none | n/a | Manual end-to-end walk. Gate ✓ (.unpatched=bdcae093 matches anchor). Decode path ✓ on account2 (analyzeHeadless + 16 pool slots). Found+fixed 4 feeder gaps in area_residue.py: GAP-1 (downgraded) DecompPC.java absent from THIS working tree but tracked in HEAD (13562b) — a fresh git worktree checkout gets it; restored the local copy, no real gap; GAP-2 (open) ghidra_pool.ps1 status raw-invocation path-mangling — verify via skill from a real worktree; GAP-3 feeder ranked a BLOCKED-ON-ENV row (004c1a70, x87 naked) #1 — now reads frida_diff col, sinks BLOCKED; GAP-4 feeder boosted DEMOTED rows via degenerate all-True artifact CSVs — now reads notes, sinks DEMOTED, trusts only literal green* verdicts. **Key finding: render has NO free C2->C3 wins.** Residue = 732 doc-only C2 (need authoring), 12 DEMOTED (need harness extension, chiefly nonnull-only pointer-comparison arg_type — unblocks the allocator class incl. 004c5890 RwTexDictionaryCreate), 1 BLOCKED, 2 implemented-but-problematic. Heavy half (build/Frida/parity) not yet proven — requires either authoring a doc plate (e.g. 00401f10, has stubs S-3120/S-3121) or the harness extension. |

---

## hud

_QUEUED — section stub, filled when scheduler activates it._

## ai

_QUEUED — non-visual; parity gate N/A, exit gate is coverage + Frida diff only._

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
| 1 | 2026-09-01 | 8 decoded; 3 authored+linked+queued | 0 landed (3 queued NEEDS-BOOTED-RACE) | n/a (non-visual load path) | no | Preflight ✓ (anchor bdcae093; pool15; baseline build compiles both targets — only shared-`original\` .asi deploy fails, expected in a worktree). Scoped 60 C2 residue: all course-load-time (no free synthetic C2→C3 wins, same as render round-0). After parent lifted the base-fix HOLD + set the NEEDS-BOOTED-RACE policy, authored the course-load dispatch cluster `Track/CourseLoadDispatch.cpp` (verbatim ports of 0x0040d020 LoadTrackByIndex, 0x0040d440 Course::LoadCurrent, 0x0040cea0 VehicleSurfaceSetup), linked into asi_sources.rsp, build ✓ (all 3 exported + auto-registered, .asi 857088B), added 3 hooks_registry.py entries (registry parses, 1092 hooks), queued to PROMOTION_QUEUE.md for the parent's booted-race verification. Did NOT spawn MASHED (shared-`original\` deploy collides with sibling children — parent's serialized sweep owns it). DEFERRED 0x0040d110 (fragile stack-struct passing to stub callees FUN_0041a8d0/004220d0 — decode those first). NOT dry: advanced LINKED coverage +3 and queued 3 promotable hooks. |

## frontend

_QUEUED — tail; draw-list parity already GREEN-capable (scr1 118/118)._
