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
| frontend | 81 | 152 | 1 | 0 | scr1 118/118 GREEN (unchanged) | ACTIVE |

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

_QUEUED — game-mode slice; only 3 implemented .cpp, expect a long area._

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
