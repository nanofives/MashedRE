# Mashed RE Roadmap — v3 (2026-08-15)

Supersedes v2 (2026-06-09), archived verbatim at
`re/analysis/archive/ROADMAP_v2_2026-06-09.md`. v2's workstream definitions (WS-A..WS-J)
are carried forward below and remain the unit of work; what changes is the **gate**.

> **Naming, 2026-09-26.** In this file, D0–D5 are **phases**. The four user decisions of
> 2026-07-31 in `RE_MASTER_PLAN_2026-07.md` §5 are **DEC-2/4/6/7**. Older text that says "Gate
> D2 (2026-07-31)" means DEC-2 (librw ships), not phase D2 (default physics).
> **State 2026-09-29 (supersedes the 2026-09-26 line below):** D0 ✓, D1 ✓ (default flip,
> residue R1-R3 open), **D2 REOPENED 2026-09-29** (user decision — the closing evidence
> compared a three-opponent port run against a ONE-car original capture; see §D2), D3 active
> but **blocked on D2 re-closing** — the modes 3/7 port does not start until then. D4-D5 not
> started.
>
> *Superseded — **State 2026-09-26:** D0 ✓, D1 ✓ (default flip, residue R1-R3 open), D2 ✓,
> **D3 active** (AI: ported tick drives the opponents, (b) 2 of 3 cars in tolerance; powerups
> (a)(b) met, (c) met on decisions; modes GREEN on 3/11 rules), D4-D5 not started.*

---

## Why v3 exists

v2 asked "how many functions are ported?" and drove work accordingly. That question
produced real results — 5,897 rows in `hooks.csv`, a race loop, a librw renderer, a
clean-room RWP-3.7 solver island. It also produced a gap nobody was measuring:

> **`mashed_re.exe`, run with no environment variables set, does not use most of what
> has been ported.**

Verified 2026-08-15 against the source, not against a doc:

| Ported subsystem | Gate | Default |
|---|---|---|
| librw renderer | `MASHED_RENDER_LIBRW` (`LibRw/RwRaceSubmit.cpp:135-136`, requires exactly `"1"`) | **OFF** |
| Ported vehicle physics | `MASHED_REAL_PHYSICS` (`Vehicle/VehiclePhysicsRun.cpp:154-156`, requires the var to merely exist) | **OFF** |

With neither set, the shipping exe runs a hand-written D3D9 renderer (which is neither
verbatim RW nor librw) and the kinematic drive model that v2 itself called "explicitly
NOT the ported physics". `LibRw/RwRaceSubmit.cpp:218` states it plainly: *"with no env
set the shipping D3D9 path still runs"*. A generated inventory finds 150 distinct `MASHED_*` tokens under `mashedmod/src/`, of which
**138 have an actual env-read site** (`getenv`/`GetEnvironmentVariableA`, plus the
`envSet(...)`/`EnvSet(...)` accessors) — the remainder are 8 non-env tokens and 4 dead
flag names. So "what the exe does" currently has no single answer, and the honest flag
number is **138**, not the 146 this document first claimed nor the 128 the first D0 pass
reported (146 counted raw tokens; 128 came from too strict a regex that missed the
`envSet` accessors; corrected to 138 by D0.2, `re/analysis/FLAG_INVENTORY_2026-08-15.md`).

**This is not a new requirement. It is a violation of the rule v2 already had.** S-DoD
criterion 1 reads: *"The standalone exe runs the subsystem's canonical scenario natively
— no original code, no fallbacks."* An env-gated opt-in path IS a fallback. The default
build has been quietly exempted from the project's own definition of done.

v3 makes the default build the deliverable and the measuring instrument.

---

## The default-build rule

> **A capability counts only if it runs in `mashed_re.exe` with no `MASHED_*` variable set.**

Corollaries, all enforceable:

1. **Env vars are for verification, bisection and debugging — never for selecting which
   implementation ships.** A flag that picks between a scaffold and a port is a migration
   in progress, and it gets a dated owner and an exit condition or it gets deleted.
2. **A port is not landed until its flag is inverted** — i.e. the ported path is the
   default and the flag (if kept at all) only turns it *off* for A/B. "Ported + wired +
   flagged on-demand" is the state v2 accepted; v3 calls that half-landed.
3. **Every phase gate below is measured on a clean environment.** If a demo needs a flag
   set, the demo does not count. This applies retroactively to the phase ledger.
4. **The flag inventory is a tracked number.** 138 real env vars today (150 raw tokens).
   It should fall. Count reproducibly, not by grepping the prefix — see D0.2.

This rule costs something and the cost is worth naming: some flags exist because the
ported path is *not yet good enough* to default. Inverting those flags will make the
default build visibly worse before it gets better. That is the point — it converts
invisible debt into visible, fixable regressions.

---

## Honest baseline (2026-08-15)

Recounted from the data, not read off a doc. Where a doc disagrees, both are shown.

### Coverage

| Slice | Rows | C3+ | C4 |
|---|---|---|---|
| All `hooks.csv` | 5,897 | 18.1% | 3.1% |
| **First-party only** | **3,682** | **28.8%** | **4.9%** |
| Third-party library | 2,215 | 0.3% | — |

Report the first-party number. The headline 18.1% is diluted by 2,215 vendored-library
rows that will never be ported and should not be in the denominator of a progress metric.

**The denominator is incomplete regardless:** ~1,788 RVAs inside the race slice's own
call closure have never been discovered into `hooks.csv`. Percentage-of-known is not
percentage-of-work.

Confidence spread: C1 795 · C2 4,005 · C3 881 · C4 185 · 31 untagged.
Strongest subsystems: save 93.8%, frontend 68.9%, ai 61.0%. Weakest: track 6.1%,
particle 10.0%, boot 12.5%.

### Debt

- **UNCERTAINTIES**: 2,980 live. 2,530 typed semantic/structural (the C3-blocking class),
  but only 512 of those carry a non-empty `Blocks` cell. **The Type column and the
  `Blocks` column disagree about what is actually blocking.** Reconciling them is D0 work.
- **STUBS**: 1,072 live, against a census header in the file itself claiming 1,109.
- **DEFERRED**: 43 live rows (128 struck). Four are milestone deferrals
  (D-11060/61/62/63); the rest are mechanical. **No open architectural deferral.**

### Phase ledger — corrected

v2 claimed "R0–R6 closed". Its own phase text contradicts that: **R4 and R5 both read
"OPENED"** and their exit criteria — original-screenshot parity, physics diffed against
original telemetry — are open. v3 records them as open. See the D-phase mapping below.

### Verification

458 files committed under `verify/`. But the scaffold-vs-verbatim inventory
(`SESSION_VERIFICATION_AUDIT_2026-06-16.md`) is **~2 months stale** — it predates B5,
librw, and every 2026-07 promotion. Refreshing it is the single highest-value input to
planning anything, which is why it is D0.

---

## Definition of Done

**F-DoD (function)** — unchanged from v1/v2. RVA pinned; confidence ≥ C3; no unfiled
`[UNCERTAIN]`; no stubs; clean `diff-original` on ≥1 canonical scenario; hook registered
via `RH_ScopedInstall` and runtime-toggleable; inline RVA comments. C4 still means
"verified by Frida diff **with the hook actually installed** on a canonical scenario" —
the anti-overclaim rule in `CLAUDE.md` stands and has been enforced against real
promotions before.

**S-DoD (subsystem)** — unchanged in text, **clarified in enforcement**: criterion 1's
"no fallbacks" explicitly includes env-gated alternatives. A subsystem whose port only
runs under a flag is not S-DONE. Criteria 2–6 (every executed function at F-DoD;
unexecuted functions explicitly dispatched; structs documented; formats round-trip;
`STUBS.md` section empty) carry over unchanged. The v1 percentage gates stay retired.

**P-DoD (project)** — every subsystem S-DONE; a clean playthrough of every track,
vehicle and mode on `mashed_re.exe` alone; trackers empty but for justified
`wontfix`/`deferred-not-needed`; the dev `.asi` dropped from the shipping matrix.

**D-Gate (new, applies to every phase below)** — the phase's demo runs on a clean
environment. No `MASHED_*` set, no manual steps, no "then flip this flag".

---

## Phases

Phases are gates, not dates. Do not advance while the current gate is unmet.

### D0 — Tell the truth again (prerequisite for all planning)

v2's R0 did this once and it paid for itself; the repo has drifted since.

1. ~~Refresh `SESSION_VERIFICATION_AUDIT`~~ **DONE 2026-08-15** —
   `re/analysis/SESSION_VERIFICATION_AUDIT_2026-08-15.md`. It surfaced two items that did
   not exist when D0 was written, both below (6 and 7), and one correction to this
   document's own premise: **env-gating is not the largest gap — non-linkage is.**
   `build.bat` linked **198 of 433** `.cpp` into `mashed_re.exe` **as audited 2026-08-15**
   (D0.1 first counted 193 because it counted plain sources on the `cl` line and omitted the
   5 isolated per-target `.obj`; it reported a plain-source count as a TU count. **204 after
   batch 1** landed six files on 2026-08-18 — see item 7); `Save/` contributes
   0 of 17 files and `Audio/` 4 of 25, so 585 audio and 32 save rows — *including 28 save
   C4s* — are absent from the deliverable and **no env var can reach them**. The default-build
   rule therefore needs a second clause: a capability counts only if its TU is linked
   into the exe *and* reached on the default path. **Why they are absent is settled by D0.7
   (item 7, answered 2026-08-18): this code is hook-shaped, not unlinked by drift** — see there.
2. ~~Publish the flag inventory.~~ **DONE 2026-08-15** —
   `re/analysis/FLAG_INVENTORY_2026-08-15.md`, generated rather than hand-listed. **150
   tokens, 138 live env vars, 8 non-env tokens, 4 dead flag names.** Note the count moved
   twice: v3 first said 146 (raw prefix grep), I corrected it to 128 (too strict a regex),
   and the true figure is **138** — `envSet(...)` and `EnvSet(...)` are real accessors that
   the stricter regex missed. Only **3 flags are migration debt** (`MASHED_RENDER_LIBRW`,
   `MASHED_REAL_PHYSICS`, `MASHED_RW_RENDER` which is inert), and 3 are already correctly
   inverted. The 4 dead names are comment-only: a comment naming a flag that does not
   exist is a false map — delete or implement. **Deleted 2026-09-09** (the four comments
   now say the name is retired and point at the real gate).
3. ~~Reconcile UNCERTAINTIES `Type` against `Blocks`.~~ **DONE 2026-08-15.** 2,547 rows are
   typed `semantic`/`structural` but only **644** carry a non-empty `Blocks` cell. The
   header rule (Type gates C3) is not what is practised and would have ~1,900 rows silently
   blocking promotions that in fact proceeded. Rule corrected in the tracker: **`Blocks`
   decides; `Type` is a descriptive taxonomy.** Also repaired 113 malformed rows (6 columns
   instead of 8, missing `Type`/`Evidence missing`/`Blocks`, an RVA sitting in the `Type`
   slot) — their empty `Blocks` was being counted as "blocks nothing", understating the
   figure. **`Blocks` since filled from evidence:** 43 rows → `nothing` (their target
   is at C3/C4 in `hooks.csv` *with the row open*, so it provably did not gate), 70 →
   `[UNPROVEN]` (target still C2, never tested — explicitly NOT "non-blocking").
   **Final: 2,999 open, 640 assert a blocker, 70 unproven.** 4 odd-shaped rows await
   manual review.
4. ~~Fix `STUBS.md`'s census and the 13 shifted rows.~~ **DONE 2026-08-15.** True count is
   **1,107 open / 149 struck** (1,256 total) — the header said 1,113/143, its own appended
   narrative ended at 1,109/147, and v3 quoted 1,072/1,109; all three were wrong. Census
   rewritten with its reproducing command. The 13 "misformatted" rows were missing their
   Subsystem *and* Type columns entirely (5 fields, not 7); subsystems recovered from
   `hooks.csv` via the called RVA, Type marked `[UNRECORDED]` rather than guessed. The
   `misformatted` pseudo-subsystem bucket is gone.
5. ~~State what `re/analysis/CHANGELOG.md` is.~~ **DONE 2026-08-15.** It is the **tracker
   audit trail** — why tracker state changed — not a commit log, and not to be measured
   against commit count. The file had **no header at all**, which is part of why its scope
   was ambiguous. Added one stating scope, newest-first ordering, an `<!-- ENTRIES -->`
   insertion marker, and never-rewrite/never-truncate. Also fixed a live inconsistency:
   five skill docs across `re-classify`, `ghidra-sweep`, `frida-sweep` and `multi-session`
   said "**append**" while the file is prepend-ordered — that undocumented mismatch is part
   of how `2dee9c67` came to overwrite it. All now name the marker.

6. ~~Decide the 14 unfalsifiable C4 rows.~~ **DONE 2026-08-15 — re-run, all 14 hold.**
   `canonical_c4_racediff.py` re-run in three batches (5/5/4), in-race and frame-synced
   over [300,1200]: **14/14 C4-CLEAN with `jmp=0xe9` installed**, off-set == on-set, zero
   demotions. Root cause fixed rather than just the symptom: the citations pointed into
   gitignored `/log/`, so all 14 `frida_diff` fields now point at the **tracked** artifact
   `re/analysis/phys_c4_evidence/c4_racediff_result_2026-08-15.json`. **Remaining
   systemic issue:** `/log/` still holds 2,217 files against 27 tracked, so other rows
   citing `log/...` have the same latent fragility — sweep them next.
7. ~~Resolve the linkage gap.~~ **ANSWERED 2026-08-18**, and the answer is neither of the
   two options this item offered. 124 of the 235 unlinked `.cpp` were triaged
   (`re/orchestrator/read_fleet/runs/w1_relink/`, two independent passes that converged).

   **`RH_ScopedInstall` is not a boot hazard and never was.** It expands to a file-scope
   object whose ctor calls `HookSystem::Register(RVA, &fn)` — the RVA is passed as an
   *integer*, never dereferenced — and in the exe `Register` is the no-op from
   `Stubs/HookSystemNoOp.cpp` (`build.bat:212`). `Util/UtilLeaves.cpp` has the identical
   shape and has been linked and booting all along. The real trigger is narrower: a
   file-scope initializer that *dereferences* an absolute address. Across 124 files there
   are exactly **two** offenders, both in `Audio/`: `AudioDSound.cpp:95-96` (the
   `static const GUID = *(const GUID*)0x005d09dc` pattern `build.bat:101` names verbatim)
   and `AudioRws.cpp:477-490` (RVA-bound globals — binds only, will not fault the loader,
   held out for its thunks to the original RW audio engine).

   **The real blocker is not linkage, it is that this code is hook-shaped.** Because
   `Register` is no-op'd, a linked reimpl is a *dead export* unless the standalone call
   graph invokes it by name; and its body still derefs MASHED addresses (`0x004xxxxx`
   code, `0x006xxxxx`–`0x008xxxxx` data) that are unmapped in an exe based at `0x10000`,
   so it AVs if it ever does run. **Bulk-adding the class-B files would grow the binary
   and the tracker without shipping one working feature** — the exact thing corollary 1
   of the default-build rule exists to prevent. This vindicates D0.1's amendment: linked
   *and reached* is the test.

   Per-directory disposition: `Save/` is **drift** but inert (16 files, 28 C4, all
   load-safe, nearly all RVA-tunnelled). `Audio/` is **mixed** — `AudioDSound` (8 rows)
   and `AudioRws` (20 rows) are genuine intent, the other 18 of 21 files (~50 C3) are the
   same drift as `Util/`. `Util/` is 72 files, uniformly class B, dominated by one
   `PromoLoop` family of 63.

   **Add-backs are therefore gated on NO MASHED ADDRESS IN ANY CODE PATH**, not merely on
   booting. Batch 1 landed 2026-08-18: `Save/FsOpen.cpp`, `Save/VfsStream.cpp`,
   `Save/ReplayTimeFormat.cpp`, `Input/MemsetInline_ag1.cpp`,
   `Particle/ParticleLeaves_ad4.cpp`, `ParticleLeaves_ad5.cpp` — six files meeting that
   bar, verified per file for zero non-comment RVA references and zero cross-TU deps.
   Everything else in the backlog needs its RVA tunnels neutralized first, which is
   porting work and belongs in a phase, not in this item.
8. ~~Stop the harness overwriting committed evidence.~~ **DONE 2026-08-15.** All 17
   capture sites in `exe_main.cpp` now route through `VOut()`/`VOut2()`, which root every
   harness write under `verify/run_<pid>/`. `MASHED_VERIFY_OUT` overrides the root, so
   regenerating a cited artifact in place is still possible but is now an explicit act.
   Verified both ways: the same race-demo run that previously overwrote 16 tracked BMPs
   across `verify/race1|r5|r6` now modifies zero, and the override lands where told.

**Gate:** every number in this roadmap is reproducible from the repo by a stated command.

### D1 — Default renderer — **CLOSED 2026-08-19 (default flip); D1-residue OPEN**

> **Reconciled 2026-09-26.** This section and the critical-path line under the workstream
> ledger disagreed (one said closed, one said "the gate is not closed"). Both were half right,
> so D1 is split, following D2's closed-with-residue form:
>
> - **D1 CLOSED 2026-08-19** (`f4815877`): librw is the default; `MASHED_RENDER_LIBRW=0` is
>   A/B revert only; A/B condition met 16/16 ≤1.01% (`verify/d1_recheck_20260818/REPORT.md`).
>   The flip happened before the original-side adjudication the gate text below asked for.
>   That clause moved into the residue, and it was not silently dropped.
> - **D1-residue, OPEN, must close before D5.** Gate: (R1) an original-side capture at a
>   matched, frame-synced pose adjudicates faithfulness (`drawlist_diff.py` GREEN or every row
>   cited); (R2) the verbatim race-camera pose from `Race/RaceCamera.cpp` drives the renderer
>   (Camera subsection below); (R3) the legacy D3D9 race renderer and the
>   `MASHED_RENDER_LIBRW=0` revert are deleted.
>
> **Render-faithfulness defect closed 2026-09-30 (counts toward R1, does not close it):**
> **car brightness, defect (d)** — `ParseLightsDffFaithful` composed the track directional
> light's world at-vector one frame too many, mis-aiming the sun on **13 of 13** shipped
> tracks. Fixed at `D3d9Render/TrackRenderer.cpp:853` and its twin `:751` (parent-start the
> frame-chain walk). Both renderers are fed from the same `sun_dir_`
> (`LibRw/RwRaceSubmit.cpp:571`), so one change covers librw and the legacy D3D9 path.
> Acceptance A1/A2/A4 pass, A3 fails as written; full evidence and the surviving open items
> (O2 original-side, O3 props/copters, O6 A3's missing matched-pose reference) in
> [`re/analysis/CAR_BRIGHTNESS_2026-09-30.md`](re/analysis/CAR_BRIGHTNESS_2026-09-30.md).
> **R1 itself is untouched** — it still wants the frame-synced original-side draw-list
> adjudication, and the A3/O6 gap is a concrete instance of what R1 is missing.
>
> **Second render-faithfulness defect closed 2026-09-30 (counts toward R1, does not close
> it): grey car chassis, defect (a)** — both car loaders handed the WHOLE vehicle clump
> downstream, so the standalone drew all 71 of `ADVANTAGE0.DFF`'s atomics. 27 of them are
> not body geometry (4 untextured car-sized collision hulls + 23 one-triangle locators,
> material `(102,102,102)`), and drawing them covered the painted body with a grey shell.
> Fixed by `CarDropNonRenderAtomics` in `TrackRenderer::LoadCar` / `::LoadCarLiveries`
> (`5ddc0384`); one filtered model feeds the wheel heuristic and both renderers'
> batch builders. Pre-registered acceptance `2179ed1a`, results in
> [`re/analysis/CAR_GRAY_FIX_ACCEPTANCE_2026-09-30.md`](re/analysis/CAR_GRAY_FIX_ACCEPTANCE_2026-09-30.md):
> G1/G2/G3 and G4b/G4c PASS (car-box grey fraction 0.8487 → 0.0782, hull tone
> `(102,102,102)` 1999 px → 0, textured batch count unchanged at 176 = 44 x 4, all nine
> A4 terrain/sea boxes and both frontend frames bit-identical); **G1d unmeasurable as
> written** and **G4a fails as written**, both from instrument defects that are pinned
> there. This is a **measured equivalent** of the original's per-part-code selection
> (`FUN_00420420` @ `0x00420420`), **not a verbatim port** — the part code is blocked on
> **U-9079** — so it carries no `hooks.csv` row and no C-level. Note items O3/O4/O5
> (duplicate LOD sets both drawn, `MASHED_RPLIGHT=0` renders the car black, props not
> swept) remain OPEN.
>
> **Third render-faithfulness defect closed 2026-09-30 (counts toward R1, does not close it):
> Arctic sea at road height, defect (b)** — the `Clump_Filename` prop loop pushed one
> **identity** instance per clump, on the assumption that a world clump's frame carries its own
> placement. `SEA.DFF` carries none (`frame[0].pos == (0,0,0)`), so Arctic's sea rendered as a
> single 60×60 m patch at road height, covering **77.96%** of the start-grid view with water
> where the original has road. The original places it from a **per-track hook**, not from the
> asset and not from `COURSE.LUA`: table `0x005f33f8` (stride `0x48`), record `"arctic"` at
> `0x005f3488` with `Course_Id 0` at `0x005f3498`, slot 0 at `0x005f349c` = `0x00448940`, which
> lays **25 tiles, 5×5 at 60 m spacing, anchored at `(−150, −4.1, −150)`**
> (`0x004489bb` the −4.1, `0x00448a65`/`0x00448a80` the −150.0 starts, `0x00448a6f`/`0x00448a88`
> the 5×5, `0x00448aa2` `RwFrameTranslate`, `0x00448aab`/`0x00448ac1` the +60.0 step).
> Reproduced at `D3d9Render/TrackRenderer.cpp:1715-1790` (`a0f3004c`), scoped to
> `course_id_ == 0 && c.idx == 2` — the original's own `Course_Id`-keyed scope, so no other track
> can regress by construction. **RETROFITTED INTO A REAL PORT 2026-10-01** (see the note below). Pre-registered acceptance `548ac7ce`, results in
> [`verify/sea_fix_20260930/RESULT.md`](verify/sea_fix_20260930/RESULT.md): rules 1/2a/2b/3
> **PASS** (25 instances at exactly the original's grid; relative gradient over six fixed road
> boxes 0.22–0.49× → 0.59–0.88× the original's at the committed s8/s14 poses; tile 0 equal to the
> original's live-read `translate(−150, −4.1, −150)`; all 12 other tracks bit-identical at
> `0/307 200` with a passing determinism control). **Rule 4 fails as written** on three s8 car
> boxes and the **edit is kept**: all 1 401 changed pixels lie inside a geometry-scoped pre-fix
> sea mask (100.0%, 0 outside), so the boxes held 13–53% sea and were never "outside the sea
> region"; zero car-body pixels changed. Like (a) and (d) this is a **measured equivalent** of the
> original's placement, **not a verbatim port** of `FUN_00448940` — no hook, no Frida diff — so it
> carries no `hooks.csv` row and no C-level. **U1b** (the dedicated Arctic sea pass `0x00449030`
> and its eight `RwGlobals+0x20` state pairs, hence draw order / blend mode) and **U2** (the
> original's per-clone world registration at `0x004e45b0`) remain OPEN.
>
> > **SUPERSEDED 2026-10-01 — it IS a verbatim port now, and it carries a row.** Retrofit lane,
> > first use of the standing workflow "a fix that is a real port must also produce promotion
> > evidence". The whole 880-byte body (`0x00448940..0x00448caf`) is transcribed in
> > `Render/ArcticTrackNodeSlot0.cpp`, in **both** `.rsp` lists, with
> > `RH_ScopedInstall(ArcticTrackNodeSlot0, 0x00448940)`. `TrackRenderer`'s inline 5×5 loop is
> > gone: it and the node's own translate loop now call the **same** `ArcticSeaTileGrid`.
> > `hooks.csv` **`0x00448940 ArcticTrackNodeSlot0 render C3`** (C0 → C3; there was no row at all
> > before, and the function was not even defined in Ghidra). Caller gate:
> > `TrackNodeDispatch14` `0x0041e8b0` **C3**, read from the bytes —
> > `mov ecx,[0x0063d7e4] / jmp dword ptr [ecx+0x14]`, and `+0x14` **is** slot 0.
> > **path1 is BLOCKED and was pre-registered as such** (`verify/retrofit_20261001/PREREG.md`
> > B-B1): an A/B calls the function twice and it clones 24 `RpClump`s, so no save/restore exists.
> > Run instead: Arctic hook-ON vs hook-OFF with the install witness read **in process**
> > (`[0x00448940] = 0xa5e05be9` ON vs `0x5324ec83` OFF) — the 25 live tiles' frame modelling and
> > LTM translations are element-wise identical and equal to the predicted grid in order. Two
> > Ghidra arities were wrong and are corrected in the port (`0x004671a0` takes one argument,
> > `0x004c1b10` takes two). Commits `e570a7df` (pre-registration), `60af8ace` (the ports), `37cb0d11` (the evidence), `46bf1453` (the promotions).
> >
> > **U2 is now partly answered**: the per-clone registration at `0x004e45b0` is a single
> > `RwFrameRemoveChild(course+0x105d4, tiles[0])` at `0x00448a1f` — a **detach of the base clump
> > before cloning**, not a per-clone registration. U1b is untouched.
>
> **Fourth render-faithfulness defect, STAGE 1 OF 2 closed 2026-10-01 (counts toward R1, does
> not close it): power-up pickup PLACEMENT, defect (c)** — the port parsed
> `POWERUPS_GOLD.LUA`. The original never opens it on shipping data: `FUN_004264d0`
> @`0x004264d0` pushes `"powerups_gold.dff"` (`0x005cd4e4`) at `0x004265bd`, loads the clump at
> `0x004265c2` and calls the live placement path `FUN_00426460` @`0x00426460` at `0x004265d2`;
> the Lua arm behind `JE 0x004265df` needs a null clump and all 13 track pizzes carry the DFF.
> The two files genuinely disagree — on Forest they share **zero** positions. Reproduced in new
> `Track/PowerupMarkers.{h,cpp}` (frame modelling-matrix translation `frame+0x40`; RW USERDATA
> `0x011f` **array 0, element 0** as `FUN_004b5190(atomic,0,0)` reads it; `type = v & 0xff`,
> `respawn = v >> 8`; enumeration order measured to be the reverse of the file's atomic order),
> plus `FUN_00458e00` @`0x00458e00`'s normal-race filter in `PickupField::InitReal` (cap 25,
> dedupe `< _DAT_005cc558` = 0.00100000005, reject type `0x15`, position stored **verbatim** —
> the invented `worldR_*0.012` Y lift and the "every 8th AI gate" fallback are gone). Commit
> `c9615225`; pre-registered acceptance `b2649833`, results in
> [`verify/pickups_fix_20261001/RESULT_STAGE1.md`](verify/pickups_fix_20261001/RESULT_STAGE1.md):
> **P1 PASS** bit-identical against the **live-read original pool** on TRAINING (5/5) and ARCTIC
> (7/7), n = 6 agreeing samples per track; **P2 MATCH** on all 13 tracks; **P3 PASS** 0 differing
> pixels outside the projected pickup regions on 3 Arctic frames (888/854/59 inside), with the
> instrumentation control at 0 px whole-frame, a 0.002 px projector cross-check and two-boot
> identity; **P4 PASS** (collection code carries no diff hunk, `PickRadius` still
> `worldR_ * 0.04f`). Unlike (a), (b) and (d) this one **is** a verbatim transcription of a
> named function's predicates, but it is still a standalone-side reimplementation with no hook
> and no Frida A/B, so it carries no `hooks.csv` row and no C-level.
>
> > **SUPERSEDED 2026-10-01 — it has a hook, a Frida A/B and a row.** Retrofit lane. The inline
> > copy inside `PickupField::InitReal` is gone; the body lives at
> > `Gameplay/PickupPoolSpawn.cpp`, in **both** `.rsp` lists, with
> > `RH_ScopedInstall(PickupPoolSpawn, 0x00458e00)`, and `InitReal` now derives every
> > accept/reject from that function's **return value** and reads each placed orb back out of the
> > pool it wrote. `hooks.csv` **`0x00458e00 PickupPoolSpawn gameplay C2 → C3`**. path1 **GREEN
> > 6/6 NON-DEGEN** (`log/diff_pickup_pool_spawn.csv`), path2 **PASS 4/4**, plus a canonical
> > Arctic run with the hook live. P1/P2 re-run on the retrofitted build and unchanged.
> > `U-8325` RESOLVED (`FUN_0042fe30` is `RaceEndFlagIfEndMode`, C4, not a "game-mode getter").
> > Commits `e570a7df` (pre-registration), `60af8ace` (the ports), `37cb0d11` (the evidence), `46bf1453` (the promotions). The rank-2 arm stays [UNCERTAIN U-9168] and the collection
> > radius is still open, exactly as the paragraph above says.
>
> **STAGE 2 (the LOOK) NOT STARTED, and the blocker is the instrument, not the code.** The real
> `ICONCUBE.DFF` (14 verts, 12 tris, one material), the 11 per-type icons (already resident,
> reachable via `g_quad_renderer.slot_texture`) and `PUGLOW.PNG` all load through paths the port
> already has, and the blend is known (`FUN_004770c0` writes 5/6 = SRCALPHA/INVSRCALPHA, so the
> port's additive glow is wrong). But stage 2 rule (i) wants the original's per-pickup batches by
> **count, texture name and blend state**, and the only original-side draw instrument — the d3d9
> shim — records **aggregate counters only** (`d3d9_shim.cpp:612-638`). Measuring it needs a
> per-draw ring buffer in the shim, a texture-pointer→name resolution that does not exist yet
> (RwTexture/RwRaster layout + the D3D9 raster-extension offset, then a read-only `Memory` walk —
> not an `Interceptor`, that is the hot path), and draw-to-pass attribution. Rules (ii) and (iii)
> are measurable today. The rule was not amended. Also still OPEN from stage 1: the collection
> radius is `worldR_ * 0.04f` where the original uses a **0.5** sphere (`0x00459228`), and the
> rank-0 / rank-2 arms of `FUN_00458e00` are unwired with `DAT_0067ea74`'s driver [UNCERTAIN].

Invert `MASHED_RENDER_LIBRW`. librw becomes the shipping path; the hand-written D3D9
renderer becomes the fallback, then goes away.

> **STATUS 2026-09-09 (drift correction). The flag IS inverted and librw IS the default.**
> Landed 2026-08-19 in `f4815877` ("D1: invert render default to librw; prove the gate"):
> `RaceSubmit_Requested()` (`LibRw/RwRaceSubmit.cpp`) returns true unless
> `MASHED_RENDER_LIBRW=0`, which is the A/B revert to the legacy D3D9 path. The
> "BLOCKED, divergence accumulates" paragraphs below are the 2026-08-15 measurement and
> are HISTORY: the accumulation was root-caused on 2026-08-18 as a scaffold FX particle
> defect on the D3D9 side, cut from the default build, and the re-measure is 16/16 shots
> at or under 1.01% (`verify/d1_recheck_20260818/REPORT.md`, control pair 16/16
> byte-identical `verify/d1_control_20260818/REPORT.md`; CHANGELOG 2026-08-30 "librw is
> the DEFAULT renderer"). This section was not updated at the time, and a 2026-09-09
> worker survey repeated "librw gated OFF" as fact from it.
>
> **What D1 still owes** (now the D1-residue block above, R1-R3): (1) faithfulness adjudication against the
> ORIGINAL, not D3D9-vs-librw (`RE_MASTER_PLAN_2026-07.md` §7 item 1); (2) the verbatim
> race-camera pose wired into the renderer (Camera subsection below, pose still discarded);
> (3) the D3D9 fallback has not gone away. `FLAG_INVENTORY_2026-08-15.md` class A still
> lists the flag as OFF; corrected there by a dated note, not by regenerating.

**HISTORY — measured 2026-08-15, when the inversion still read as BLOCKED
(`verify/d1_measure/MEASUREMENT.md`).** With the R10b-fixed gate, a like-for-like run
differing only in that flag gave: 12 of 16 shots at or near parity (≤0.92%), and four
that diverged — `01_inrace_track` 71.61%, `round3_result` 69.15%, `round2_result` 68.94%,
`01_action` 21.69%.

**The A/B divergence that blocked this is CLOSED** — re-measured on a clean rebuild
2026-08-18 (`verify/d1_recheck_20260818/REPORT.md`). The clean-env D3D9-vs-librw A/B is
**16 of 16 shots ≤1.01%**, 14 of 16 ≤0.4%; the worst, `r5/car_3_weave` at 1.01%, is the
pre-existing indexed-vs-unindexed fill-rule delta (the D-S3-BANK shot below), not a residue
of this work. The paired control run — identical env on both arms — is **16/16
byte-identical (0.00%)** (`verify/d1_control_20260818/REPORT.md`), so every delta above is
signal, not harness noise.

**The accumulation this section was originally written around is gone.** The 2026-08-15
figures — `01_inrace_track` 71.61%, `round3_result` 69.15%, `round2_result` 68.94%,
`01_action` 21.69%, read as "leaked or unreset state" that would make the default renderer
drift as you play — now measure **0.48% / 0.10% / 0.06% / 0.01%** on the same shots. There
was never leaked state.

**The 2026-08-15 diagnosis was wrong five times over, and is recorded because the failure
mode is instructive.** The divergence was read in turn as (1) an accumulating "leaked or
unreset state"; (2) a per-channel R/G gain on the D3D9 side; (3) a D3D9 world-coverage
failure; (4) — after that was refuted — a claim that the result screen never re-renders the
world at all; and (5) a second, independent "orange sky" colour divergence. Every one was
retracted by the next measurement (chain in `re/analysis/CHANGELOG.md`, 2026-08-15→16). The
single actual cause is a **scaffold FX particle defect on the D3D9 side, present in both
runs**: `ParticleSystem` kind==2 spawns 36 fully-opaque spin-out billboards that each
subtend the whole viewport (`verify/d1_nopart/RESULT.md`, `verify/d1_fxbloom/RESULT.md`). It
is now **cut from the default build** (draw-time kind mask; `MASHED_PARTS_KINDS=7` restores
it; re-pickup condition: the ported `Particle/` system lands). The "accumulation" was a
capture-timing artefact — spin-outs are eliminations, so the diverging frames were the ones
captured just after one.

**A clean A/B is a precondition for inverting, not proof the port is faithful**
(`verify/d1_fxcut/RESULT.md`). The two paths now agree with each other; neither has been
fully adjudicated against MASHED.exe. That lane advanced materially the same day and is no
longer blind:

- **The standalone was rendering the world MIRRORED relative to the original**
  (`verify/d1_basis/RESULT.md`) — self-consistent, so gameplay looked normal and it survived
  a clean A/B for months. Fixed by negating the camera right axis on both paths, and the
  compensating librw negation `[D-S3-4]` was reverted (librw's built-in X negation is the
  original's convention; it had been tuned to match a mirrored reference). The A/B is
  unchanged by this — a shared reflection cancels inside a D3D9-vs-librw comparison
  (`verify/d1_mirrorfix/RESULT.md`).
- **Lens is measured, not invented.** `fovy = 2·atan(0.45) = 48.46°`, `near = 0.1`, and the
  far plane = `COURSE.LUA Setup_Fog`'s far argument, all read live from
  `RwCamera::viewWindow` and adopted (`verify/d1_lens/RESULT.md`).
- Against the original at a transplanted pose the figure is 89.68% → **33.79%** after the
  mirror/lens work; the residual is a different sim moment plus lighting and texture, not a
  structural transform (`verify/d1_mirrorfix/RESULT.md`). Whole-frame imgdiff cannot validate
  a pose transplant, so it is not treated as a gate number either way.

**Still open, and NOT the old blocker:** (a) a frame-accurate original-vs-standalone parity
number, which needs the pose read synchronised to the capturing Present; (b) the sky
cloud-layer / UV-scroll animation item (`re/analysis/DIVERGENCE_LEDGER_3D.md`) — note librw
draws no sky, so this is a D3D9-vs-original question in both runs.

Accepted delta on record: D-S3-BANK closed at floor 2026-08-04 — transform exact to
4.6e-4 px, residual is a 1–2 px grazing-silhouette fill-rule difference from indexed
sector-major (librw) vs unindexed material-major (D3D9) submission of identical
vertices. Evidence committed at `verify/s3bank_iso/`. Blocks nothing; it is the
`car_3_weave` 1.01% shot above.

~~Blocked by R10b.~~ **R10b CLOSED 2026-08-15** — the gate has a zero noise floor on every
shot (16/16 byte-identical across runs, confirmed again 2026-08-18 by the control pair),
root-caused as ambient DirectInput (`DISCL_BACKGROUND | DISCL_NONEXCLUSIVE`, so typing in
another window flew the camera mid-capture) plus a `MASHED_DETERMINISTIC` backdrop that
pinned the frame index but not wall-clock; both fixed, so every delta above is signal.

**Gate:** clean-env `mashed_re.exe` renders a race through librw; `drawlist_diff.py` GREEN
or every remaining row cited (the A/B condition is met — 16/16 ≤1.01%); and an original-side
capture at a matched, frame-synced pose adjudicates faithfulness before the flag default is
flipped. R10b closed so the result is reproducible.

Closes v2's **R4**.

#### Camera — measured 2026-08-27, and it changes how D1's in-race shots should be read

`verify/d1_camera_20260826/RESULT.md`. The verbatim race camera
(`Race/RaceCamera.cpp`, RVAs 0x00446520 / 0x00441820 / 0x0040e180 / 0x00410d10) had
run every frame since June **with its pose discarded** and had never been compared
against the original. It now has been, by an offline unit diff against 826 frames of
live original telemetry, and it agrees to float precision:

| | before | after |
|---|---:|---:|
| eye position, median | 4.4343 | **0.0001** |
| aim angle, median | 37.27° | **0.0007°** |
| most-separated pair, exact | 0.0% | **92.2%** |

Two defects fixed, both ASM-cited: `0x004a2c48` implemented as `std::nearbyint` when
it is `__ftol` (truncation), which indexed one past a 30-node ribbon on 591 of 766
frames; and `MostSeparatedPair`'s out-params swapped.

**The consequence for D1 is the important part.** The pose is *still* discarded —
`race_cam_.pos()` and `.target()` have zero call sites in the tree, and the in-race
view comes from an invented chase rig at `TrackRenderer.cpp:4097-4123`. So **every
in-race shot D1 has ever compared was framed by our rig, not the game's camera.** That
does not invalidate the D3D9-vs-librw A/B (both sides used the same wrong camera), but
it does mean no in-race shot can currently answer "is this what Mashed looks like".

Correction to `verify/d1_carproj/RESULT.md`: its Candidate A fed `ctrl+0x4c` as a
look-at **point**. That field is an aim **direction** — deriving elev/azim from it
reproduces the recorded `+0x34`/`+0x38` to 0.0000°, while `(tgt - pos)` is off by up to
60°. So Candidate A was mis-specified and its rejection does not establish the
conclusion drawn from it. Candidate B's positive result is unaffected.

#### When human visual review becomes worth the user's time

Recorded because it keeps being asked and the honest answer is "it depends which
surface". Three tiers:

**Valuable NOW — frontend and text.** Menu parity is closed (468/468 draw-list rows
identical with matched saves), so any visible regression there is real signal. More
importantly the harness is **structurally blind to text**: the original's RtCharset
glyph draws never reach the hooked draw vtbl, so original captures contain no text at
all (`re/analysis/parity_tooling.md:83-87`) and font raster has *no* automated coverage.
Human eyes are the only instrument that exists for it today.

**Valuable NOW — gestalt and orientation, on stills, side by side with the original.**
This is the class the harness is worst at and a person is best at. Precedent:
`verify/d1_basis/RESULT.md` found the standalone was rendering the world **mirrored**,
and it took three analysis passes plus a false refutation to get there. A person
looking at the track would likely have said "that is backwards" in seconds. Every
`verify/` still from before 2026-08-16 is horizontally flipped for this reason.

**NOT yet worth it — in-race framing, HUD, effects.** Default in-race today is a
hand-written D3D9 scaffold, an invented chase camera, a scaffold HUD, scaffold
particles, and no audio at all (`SESSION_VERIFICATION_AUDIT_2026-08-15.md` §2).
Feedback on these would restate tracker rows that are already open, and would cost the
reviewer real time for no new information.

**The gate for in-race review** is therefore: the verbatim camera pose wired into the
renderer, AND D1's default-renderer question settled. Until both hold, an in-race
opinion is an opinion about the scaffold, not about the port.

### D2 — Default physics — **REOPENED 2026-09-29** (user decision)

> #### Re-close attempt 20 — 2026-10-02. **U-9179 RESOLVED, its producer FIXED, and the sink is GONE. `wcs_drift` fires 0 times (was 47), `0x0046f6c0`'s share of `T_post` is -0.00 % (was 84.33 %), `T_post` is INSIDE BOTH of attempt 18's bars for the first time in the re-open, recovery H1 PASSES BOTH LEGS (was FAIL), and 2 OF THE 3 D2 METRICS are inside their unchanged `d81a8df6` bounds — the 2000-2600 slip band scored at all for the first time. The transcribed integer substep loop was also ported, so the port runs 2 substeps per frame like the original. `driving-median` is still 1.3 % below its lower bound, so D2 does NOT close. New rows U-9180, U-9181. NO C-level moved.**
>
> **THE CHAIN, measured end to end.** `ProduceTerrainBatch`'s admission test
> (`ContactProducer.cpp:77-81`) was a **plane-distance** test, and a plane is unbounded — so
> it was never a locality test. On a largely coplanar track it admitted ground triangles from
> anywhere on the surface, the 256-entry store **saturated on 3565 of 3565 solver calls**, the
> loop stopped scanning, and the triangles under wheels 0 and 1 were never offered to the
> classifier `0x0046cc40`. Their `key` (`+0x1ec`) therefore stayed at the init loop's `-1`
> (`fv == 10.0` exactly on all 99 rows), `0x0046f6c0`'s state machine demoted them to state 0
> at `0x0046f91a`/`0x0046f91f` — **arm `A-demote-key`, 99 of 356 wheel-rows at `d = 222..250`**
> — `bVar16` became 2, the byte-faithful `0 < count <= 2.0` gate at `0x004701e8` opened, and
> the airborne lateral drift fired 47 times in 29 frames. **The `:163` latch arm fires 0 times
> and `bVar4 == 0` on every call, so U-9179's second candidate arm is eliminated by
> measurement.**
>
> **STEP 1, no transcription defect.** `WheelContactSolver.cpp:140-207` diffed line by line
> against `0x0046f827..0x0046fae6`; every condition, constant and offset matches, with
> `[0x005d757c] = 0x00000000 = 0.0f` confirmed by byte read and the two "assumed zero" values
> cited as writes (`0x0046f6d3`, `0x0046f70e`). Confirmed **behaviourally** by gate KA-O: the
> transcribed rule — including the 4-wheel drop and the `bVar16 == 3` promotion — reproduces
> the **running original's** own four wheel states on **4686 of 4687** consecutive entry pairs.
>
> **THE ORIGINAL HAS NO CAP.** `LAB_00468b80` increments `DAT_0088e60c` unconditionally at
> `0x00468d6c..0x00468d73`, with no bound test anywhere in `0x00468b80..0x00468d7c`; its
> locality comes entirely from the BSP walk `FUN_00538c80` this function stands in for.
>
> **THE FIX.** The admission test is now **spatial** (the triangle's AABB grown by the same
> `radius` must contain the same query centre, a conservative superset of
> sphere-vs-triangle), and it is the **default**. **No new numeric constant**, no knob on the
> shipping path, no clamp, no fitted value. `MASHED_D2_BATCHMODE=plane` is retained as the
> **diagnostic pre-fix arm**, and it reproduces attempt 19 **bit-for-bit** (0.1983 n=19 /
> UNSCORABLE n=0 / 1019.77 n=76), so the A/B is a verified control.
>
> **THREE-ARM A/B** at `d = 222..250`, n = 89 / 89 / 87 solver calls over 29 frames:
> `plane` 256 entries, K = 2, drift **47**; `planefull` (full scan) 256/379, K = 2, drift
> **47**; `local` (spatial) **17** entries, **K = 4**, **S1 = 4**, all 348 wheel-rows FILLED,
> drift **0**. So the cap alone is not demonstrated to be sufficient — the admission test is
> what fixes it.
>
> **STEP 5, separately pre-registered and KEPT by its own rule.** The integer loop
> `chunk = min(remaining, 0x32)` (`0x00470f50..0x00470f61`) + `sub = min(rem, 0x19)`
> (`0x00471110`/`0x00471117`) + `fild` (`0x00471126`) + `rem -= sub` / `jne`
> (`0x0047113f`/`0x00471141`) is ported: `nsub` went from `{3: 1626}` to **`{2: 1626}`** with
> chunks `25.000000 / 25.000000` and **no residue step**, against the ORIGINAL's own measured
> **2.012 solver calls/frame** (4672 over 2321 A6a frames, histogram `{2: 2308, 3: 24}`).
> Gates G5-1 / G5-2 / G5-3 all PASSED. The driving-median was **excluded from its own gate**
> by pre-registration, so improving one metric could not justify keeping the change.
> **Stated residual:** the outer chunk loop `0x00471143..0x00471151` is NOT ported; at
> `dt = 1/60` `remI == 50` so it iterates once and cannot be distinguished.
>
> **THE SCOREBOARD, 3 runs, against the UNCHANGED `d81a8df6` bounds:**
>
> | metric | bound | PORT | n | median `d` | PORT pre-fix | ORIG | verdict |
> |---|---|---:|---:|---:|---:|---:|---|
> | slip 1500-2000 | 0.18855..0.19635 | **0.1943** | 353 | 605 | 0.1983 (n=19) | 0.1937 (n=314, `d` 720) | **PASS** |
> | slip 2000-2600 | 0.24488..0.25487 | **0.2524** | 557-562 | 718 | UNSCORABLE (n=0) | 0.2498 (n=540, `d` 909) | **PASS** |
> | driving-median | 1904.70..1982.44 | **1852.66 / 1861.43 / 1854.65** | 1367/1340/1364 | 637 | 1019.77 (n=76) | 1937.89 (n=1154, `d` 824) | **FAIL ~1.3 %** |
>
> `T_post` at `d = 222..250`: ORIG **-0.00464** vs PORT **-0.61485**, `|delta| 0.61021`,
> **inside both bars 5.5560 / 8.6373** (n = 29, median speed 737.3, median frame 237,
> `L = 0`, `participants=1` from the game's own `MATCH-SEED` line). launch `L = 0` at
> **0.19 %**, peak 1835.50 at `d` = 95 vs ORIG 1832.40 at `d` = 95, `+0xb14` at `d` = 15 both
> — **no regression**. recovery H1 **398/400 = 99.5 %** and median **1362.7** against the
> original's 398/400 and 1333.9, from a pre-fix 60.8 % / 132.8. §26.10's median-frame guard:
> the port's scored populations moved from median `d` 79-85 to **605 / 718 / 637** against the
> original's 720 / 909 / 824, so the guard no longer disqualifies the readings.
>
> **GATES THAT FAILED, reported as failures and not re-thresholded.** KA-P's entry-pair leg
> **67.2957 %** — cause measured, and it **corrects attempt 19 `RESULT.md` §1.3**:
> `ReassertContacts` is only **partly** the original's tail `0x0047044b..0x004704b0`. The
> counting half is faithful; the **promotion of state-0 wheels back to 1**
> (`VehiclePhysicsRun.cpp:347`) and the normal writes (`:350-352`) are **port-only**, and that
> promotion is why the port's entry-pair replay disagrees while KA-O reproduces the original's
> own states. KA3 **52.7060 %** — the `%g` six-digit channel limit attempt 19 already
> diagnosed; it forms no producer delta. **No decision rule was replaced.** One rule was
> **added** and is named in `RESULT.md` §8: a port boot producing no `motion_diag.log` is
> retried up to 3 times before being called a failure — two boots failed transiently this
> session and the identical control succeeded on the next boot, so without it two diagnostic
> arms would have been misrecorded as knob-induced crashes.
>
> **PROMOTION: none requested, none granted.** `ProduceTerrainBatch` is **port-only
> scaffolding with no RVA**, so `run_diff` path1 and `run_verify_hook` path2 have nothing to
> install and nothing to call. `0x0046f6c0` C2 -> C2, `0x0046cc40` C2 -> C2, `0x00468d80`
> C2 -> C2, `0x00538c80` C1 -> C1, `0x00470c70` C2 -> C2, `0x004709a0` C2 -> C2.
> `rva-lint allowlisted=122 NEW=0`; `[asi] all 422 objects up to date`; all six edited TUs
> exe-only. **AI slots 1+ are mechanically in the blast radius** (shared producer, called per
> car at `VehiclePhysicsRun.cpp:1007`) but the magnitude is **[UNCERTAIN]** — every run was
> `participants=1`. Nothing was tuned; D3 must re-baseline against the post-fix build.
>
> **ONE OUTSIDE-SCOPE ROW, inherited not introduced:** `msd+0x4a4` reads **0.67804** on the
> original and **692.302** on the port in every band, identical pre- and post-fix — and it is
> where this fix's own `radius` comes from. Filed as **U-9181**.
>
> **Open:** **U-9180** (which term carries the remaining 1.3 %; grip-clamp #6's -0.612549 is
> the only non-zero `T_post` producer left and is too small on its own, so the carrier is
> **not** assumed), U-9181, and the unported outer chunk loop. Full result:
> `verify/d2_wheelstate_20261002/RESULT.md`.
>
> #### Re-close attempt 19 — 2026-10-02. **`T_post` is SPLIT, and the carrier is `0x0046f6c0`'s airborne lateral-drift velocity write — 84.33 % of the port's sink. It is NOT grip-clamp #6 (9.70 %), NOT A6b, NOT the parked damp, NOT the contact fixup (all exactly 0.000000), and NOT the substep budget (26.03 %, a 1.5x multiplier). The site and its gate are BYTE-FAITHFUL; the defect is the gate's INPUT. New row U-9179. No fix authored, by the attempt's own registered rule; D2 does NOT close.**
>
> **STEP 1 — the split, per producer and per substep.** New default-OFF probe `MASHED_D2SINK` (`mashedmod/src/mashed_re/Vehicle/D2SinkProbe.cpp`, exe-only — all five instrumented TUs are absent from `asi_sources.rsp`, so the `.asi` relinked with **all 422 objects up to date**) samples `|v|` at every producer's own phase inside `[the +0x9e4 store 0x004686cc, the render tick]`, which is exactly `a18_budget.py`'s `T_post` interval. Reducer `re/tools/statediff/a19_split.py`. `d = 222..250`, **n = 29**, median speed **633.2**, median frame index **237**, R = 1, `L = 0`:
>
> | site | RVA | median | share of `T_post` |
> |---|---|---:|---:|
> | **`SolveWheelContacts`** | **`0x0046f6c0`** | **-23.194031** | **84.33 %** |
> | grip-clamp #6 | `0x004687f0..0x0046897b` | -2.668457 | 9.70 % |
> | A6b | `0x00468980` | +0.000000 | 0.00 % |
> | A4 parked damp (`+0x9f0 == 2`) | `0x00470948` | +0.000000 | 0.00 % |
> | A9 position+orientation | `0x0046e9e0` | +0.000000 | 0.00 % |
> | `VehicleContactFixup` | `0x0046ef70` | +0.000000 | 0.00 % |
> | gap + tail | — | +0.000000 | 0.00 % |
>
> **Three of U-9178's four candidates are ELIMINATED by measurement.** The parked damp never fires (`+0x9f0 == 0` — the field attempt 18 could not read); the fixup runs on a median of **0** frames (2 over the whole window) with a net of exactly zero; A6b writes no velocity on the port either. And **`D_clamp6` = 9.70 % reproduces attempt 18's independent `l_60` route** (7.2038e-03 of 7.4031e-02, *"one tenth"*) — two instruments, one answer, so **§23.2's refutation is confirmed a second time**. Per substep: `D_orient` and `D_fixup` are **+0.000000 on every substep**; the whole of each substep is `D_wheel`.
>
> **STEP 1B — which write, and it is not the friction pair.** Of `0x0046f6c0`'s three velocity sites, `wcs_fric` (`0x00470072..0x0047009c`, the kFricVel 1000.0 arm) and `wcs_imp` (`0x004700eb..0x0047010d`, the kFricImp 10.0 arm) fire **0** times; **`wcs_drift`** (`WheelContactSolver.cpp:337`, `vel += normalize(d) * -20.0`) fires **47** times over 29 frames and carries **100.00 %** of that site's share. The registered prediction *"`wcs_drift` fires 0 times because `gnd == 4.0`"* was **NOT MET**, and the same log says why: the gate's own `gc` has median **2**.
>
> **STEP 1C — the gate is BYTE-FAITHFUL, so the defect is its INPUT.** `0x004701e8 fcomp [0x5cc574]` / `test ah,0x41` / `jp` skips on **greater** and on **unordered** and falls through on **equal** and on **less**, i.e. the original's condition is **`0 < count <= 2.0`** — exactly what `WheelContactSolver.cpp:323`'s `(gc != 0) && ((gc < 2) != (gc == 2))` evaluates. On **47 of 87** solver calls in the window the port's own `bVar16` is **2** because **two wheels sit at state 0** (`wcs_cnt` state words `0011` ×31 and `0110` ×16, against `1111` ×37 and `0111` ×5), and **`bVar4 == 0` on all 87 calls**, which **eliminates** the `(kState2Lo < fv) && bVar4` arm of the demotion at `:170`. **[U-9179]** The original's own witness is already on disk: the velocity is **bitwise unchanged across `0x0046f6c0` on 2945 of 2945** samples of the same §16.7 arm, so none of its three sites fires there.
>
> **WHY NO INSTRUMENT COULD SEE IT.** `+0x9e0` cannot witness the gate's input: the original's **own** tail `0x0047044b..0x004704b0` RECOMPUTES it as *the count of wheels with `state != 0`* (`mov [edi+0x9e0],0` then `+= 1.0` per non-zero wheel state) — which is exactly what `VehiclePhysicsRun.cpp:343-357` `ReassertContacts` computes, **so `ReassertContacts` is NOT a port-only construct, it is that tail**. Both sides publish `4.0` while the gate consumed `2`, so `a18_budget.py`'s `gnd == 4.0 on 29/29 both` was reading the masked value. **Gate WS FAILED as registered** (asked for exactly one writer of `+0x9e0`; `re/tools/dispsweep.py` finds **NINE**) — and that failure is what exposed the masking tail. The instrument the PREREG named was also the wrong one: `fold_sweep.py` matches **folded absolutes only**, so it cannot see `fst [edi+0x9e0]` and returned *7 reads, 0 writes* (its own `+0xbf8` known answer PASSED, so it was working and simply does not answer this question). **New standing tool `re/tools/dispsweep.py`** sweeps register-relative displacements with x87 stores classified by mnemonic, and passes its own known answer (`+0x9e4`'s x87 writers `0x00467673` and `0x004686cc`).
>
> **U-9160 RE-SCOPED, and the RVA citation CORRECTED.** The port runs **exactly 3** substeps on **1629 of 1629** frames (chunks **25.000000 / 25.000000 / 0.0000038147** ms), so the "3-or-4" is **3**, flat. The residue step is **not** cheap — at 3.81e-06 ms it still costs **-7.157898**, **86 %** of a full 25 ms step, because the cut inside `0x0046f6c0` is **per-CALL, not dt-proportional** — but it carries only **26.03 %**, so **the budget is a 1.5x MULTIPLIER, not the carrier**, and the pre-registered 50 % rule correctly did not fire. **`0x00469ad4 mov ebx,2` is NOT the substep count**: it sits `0x34` bytes inside `FUN_00469aa0`'s contact-history shift loop, as §19.2 already read it, and §20.9 re-used the same RVA for the substep count. `FUN_004709a0` has no time loop either (`0x00470ab0 cmp ebp,2` is the **retry** cap, already ported at `VehiclePhysicsRun.cpp:932`; the function is a **16-vehicle loop**, `0x004709f0..0x00470c53`). **The original's substep loop is `0x00471106..0x00471141` inside the dispatcher `FUN_00470c70`:** `rem = min(remaining, 0x32)` as an **INTEGER** (`0x00470f50..0x00470f61`), then `while (rem) { sub = min(rem, 0x19); FUN_004709a0((float)sub); rem -= sub; }` — **`50 -> 25, 25` is exactly 2**, and the `fild` at `0x00471126` is what makes it residue-free. The port's float `while (remMs > 0.0f)` over `50.0000038` is the whole of the difference. Transcribed in full in `RESULT.md` §2. Also corrected: `VehiclePhysicsRun.cpp:119`'s `kMaxSubstep = 25; // 0x19 (FUN_004709a0 inner chunk)` misattributes the constant's home — the constant is right, its RVAs are `0x00471110`/`0x00471117`. Attempt 18's live `4666/2333 = 2.0000` is unaffected (it counts hits on `0x004709a0`).
>
> **STEP 2 — NO FIX AUTHORED, under `PREREG_STEP1C.md` §4's own rule** (*author a fix only for a transcription defect at a cited RVA*). The site and the gate are both faithful; the defect is which state the wheel state machine leaves two wheels in, which needs per-wheel branch inputs (`fv`, `piVar9[0x15]`) on **both** sides and neither is instrumented. **The substep budget is also deliberately not ported**, for a measured reason: at 26.03 % against the carrier's 84.33 % it would change the default build's integration cadence and move the launch/recovery statistics while leaving **~58 %** of the sink standing, confounding the next measurement. The loop is transcribed and is one edit away once U-9179 closes. **No knob, no clamp, no fitted constant. No C-level moved and no demotion** — `0x0046f6c0` is C2/mapped, which makes no behavioural claim.
>
> **GATES.** **PASS:** CH, CV (1629 frames, `miss` 0 / `dup` 0 on all five once-per-frame tags, orphans 0, `nsub` `{3: 1629}`), **KA1 100.0000 %**, **KA2 100.0000 % worst 2.030e-15**, EV (n 29, gnd4 100.00 %, ctrl>0 100.00 %), KB3. **FAILED and reported as failures, not amended:** **KA3** 70.3499 % — cause measured, `motion_diag` prints `vel=` at `%g` six significant digits and re-rounding the probe's own vector through `%g` scores **99.9386 %**; it cannot touch the reading because KA2 is 100 % inside the probe's own channel and the two independently-sourced `T_post` medians agree to **8.0e-06** (**-27.50393** against **-27.50415**, the former bit-for-bit attempt 18's). **KB1/KB2** 97.7011 % (85/87) — the 2 misses are the two retry frames, where `wcs_in` and `sub_orient` each appear twice and the reducer pairs first-to-last (attempt 18's KA-B class). **WS** — above. **ONE DECISION RULE REPLACED, stated:** `PREREG_STEP1.md` §3's candidate list indexed **one code site by substep ordinal**, so no row could clear 50 % however many times the site ran; rule 1' sums each site over the frame's substeps before the median — a regrouping of quantities **the same PREREG §2.1 already defined**, same 50 % bar, no new channel or window. As registered, rules 1 and 2 both failed to fire and the tool printed rule 4's *"NO CARRIER DOMINATES -> STOP"*; that is reported as the registered outcome, and **rule 2's non-firing is itself the finding** about the substep budget.
>
> **STEP 3, three runs on the unchanged build** (`git diff 025e1642..HEAD -- mashedmod/src` touches only `if (armed)` sites and the probe TU), participants=1 from the game's own `MATCH-SEED rule=0 participants=1 teams=0 seed=6 engine=1` line: **a** FAIL (no fix applied; `T_post` **-0.00464** vs **-27.50393**, n=29 each, median speed 790.9 / 633.2, identical to attempt 18 to five decimals; substeps 3 because the loop was not ported, which is the expected non-blocking reading); **b** PASS (**L = 0 at 0.19 %**, `+0xb14` at `d` = 15 on both arms, peak **1835.50 at `d` = 95** against **1832.40 at `d` = 95** — no regression, and nothing behavioural changed, which is the point); **c** FAIL on the median leg (**243/400 = 60.8 %** passes, median **132.8** fails, against 99.5 % / 1333.9); **d** FAIL 3/3 against the **unchanged** `d81a8df6` bounds, identical on all three runs — slip 1500-2000 **0.1983** (n=19, median speed 1665.05, median `d` **85**), slip 2000-2600 **UNSCORABLE** (n=0), driving-median **1019.77** (n=76, median `d` **79**), with §26.10's guard firing on all three (ORIG median `d` **718 / 915 / 835**, n = 312 / 553 / 1176).
>
> **COUPLING, reported not tuned.** `ReassertContacts` and the solver's grounded count are two counters of the same thing (`state != 0` vs `state == 1`) and the original has both, so the shape is faithful — but on the port they read **4 vs 2** and the masked one is what every reducer sees. `+0x9e0` is also read by clamp #6's gate `0x00468761`, zeroed by A5 at `0x0046ddd1`, and used as `a8_slip_axis.py:35`'s `>= 3.5` regime filter, so a grounded-count change moves all three. The substep count couples to the fixup cadence **and** to `BodyOrient_OmegaFromSteer`'s per-substep accumulator (`VehiclePhysicsRun.cpp:968-977`), so porting the 2-substep loop will move the yaw channel too; rendering is **not** coupled (the render tick is once per `VehiclePhysics_StepCar`, outside the loop). **AI slots 1+ untouched** — if the loop is ported later it will move them, and that is D3's to report.
>
> **Collateral.** Leg 1, paired same-side with floors (attempt-18 `s1` vs this attempt's `s1`, floors `s2`/`s2`): **0 of 75 paired fields divergent, all 75 within the measured noise floor on every one of 1627 aligned frames**, A-only 0, B-only 0. The loop's write set is **empty** this attempt, so any divergent row would have been outside scope — **there are none, no outside-scope rows**. Leg 2, cross-side banded (floor-A `orig_lb18.msd`, floor-B `s2`, scope `scope_a6a.txt`, 19 maps): **all SIX bands `!!` OFF-REGIME** (median frame indices 1022-1556 against 66-379), so per §26.10 **not one row was read**; three fields exact in every band (`msd+0x498` 40000, `msd+0x49c` 4000, `msd+0x9e0` 4).
>
> **The diagnostic is KEPT**, justified: default-OFF, CH-verified inert, exe-only, and it is the instrument U-9179's resolution path extends rather than rebuilds.
>
> **Still open:** **U-9179** (the new blocker — which branch of `WheelContactSolver.cpp:160-175` leaves two wheels at state 0, and the original's per-wheel `fv` / `piVar9[0x15]` at the same `d`); **U-9178** (re-shaped, carrier named, decomposition no longer missing — what remains under it is only U-9179); **U-9160** (re-scoped: original 2, port 3, a multiplier, loop transcribed and ready to port); U-9177; U-9176; U-9156; U-9171; §20.14's `-0.1` duty cycle; D1-residue R1. Evidence `verify/d2_sink_20261002/`.

> #### Re-close attempt 18 — 2026-10-02. **The per-frame velocity budget NAMES `T_post`, and then the instrument that measures `T_post` is shown to be PHASE-BIASED in opposite directions on the two sides — 51x understated on the original, 10.3x overstated on the port. Grip-clamp #6 is measured CONSISTENT with its own transcription at its own phase on the running original, so §23.2's attribution of the port's loss to it is REFUTED. U-9173 RESOLVED. No fix authored; D2 does NOT close.**
>
> **STEP 1 — the budget, terms in CALL ORDER by RVA, matched `d`, attempt 10's thresholds reused verbatim** (`re/tools/statediff/a18_budget.py`; `linTerm` and the (a)/(b) bars imported from `a10_gain.py` rather than restated). Over `d = 222..250`, **n = 29 each arm**, `+0x9e0 == 4.0` on 29/29 both, `|ctrl_xz| > 0` on 29/29 both, median speed O **790.9** / P **633.2**:
>
> | term | RVA span | ORIG | PORT | OUT? |
> |---|---|---:|---:|---|
> | `T_drive` | `0x0046862d..0x004686a2` | +33.20455 | +27.08079 | (a) only → no |
> | `T_rest` | `0x00470670` + `0x0046ddb0` + `0x0046833a..0x00468625` | -3.95911 | -2.50577 | neither → no |
> | **`T_post`** | `0x004687f0..0x0046897b` + `0x00468980` + `0x004709a0`×N | **-0.00464** | **-27.50393** | **(a) AND (b)** |
> | `dS` | | +27.74798 | -1.04312 | residual 28.79110 |
>
> **`T_post` is a STANDING divergence, not an onset.** It is OUT at `d = 200..222` too (-0.00401 vs -6.73326); the port's sink **triples** (-6.73 / -27.50 / -41.47) while its `T_W1` stays flat (+24.75 / +25.07 / +18.27) and is within **9.7 %** of the original's in the defect window. `d = 222` is where `|T_post|` crosses `T_W1` and the net gain turns negative — **U-9177's "switches on at `d = 222`" is corrected.** `T_rest` being inside both bars means **U-9171's A5 Phase-4 drag is not the carrier here.** Identity residual 0.000e+00, reported as a **construction** identity, not evidence. **Gate KA-R FAILED (97.22 % against a 99 % bar) and was RETIRED, not loosened** — its own published prior (§23.2's 23/1447 = **1.59 %** above 1.02) makes a 99 % bar unsatisfiable; **KA-R2**, a different question on the complement of a counted fixup population, **PASSES on the original** (11/396 = 2.78 %; 385/385 in range; median **1.000006097**, p25 0.999956816, p75 1.000044055 — §23.2's priors reproduce to three more digits than needed).
>
> **STEP 1B — §23.2 REFUTED, and this step's own inversion WITHDRAWN.** The snapshot inversion of `(k, s)` is **withdrawn** for three measured reasons: `0x0046e9e0` integrates the body orientation **inside** the substep loop, after A6a, so the snapshot's forward axis is not the clamp's (measured **1.3986 deg**/frame ORIG, **1.1130 deg** PORT, against a 2.55 deg misalignment); the original's `k` band is **172x** wide at p10..p90 with only **15 of 29** frames solvable; and the reported medians do not satisfy the relation they were inverted from. What survives is `R` (magnitudes only) and `s'`. Then, fed with the **PORT's own measured `l_60` = 676.82** (from its own `wle4`/`wld4`, `Integrate2.cpp:434`/`:449`, accumulated `:473`) through clamp #6's transcribed arms, the port is on the **HIGH arm on 29/29** frames with `G = l_60*speed = 458 880` and **`k = 0.190822`**, so the clamp costs **7.2038e-03** of a measured **7.4031e-02** — **one tenth**; closing that by a forward-axis phase shift would need **14.6 deg** of post-clamp body rotation against **1.11 deg** measured, a factor 13. **So §23.2's "only code between the two reads is grip-clamp #6" elimination is contradicted by the clamp's own arithmetic, both sides' `k` are within 1.5x (0.190822 against a structural `>= 0.1249`), and `k`/`l_60` is NOT the carrier** — §26.3's LOW-arm `k = 0.540954` does not describe the port at matched `d` either.
>
> **STEP 2 — one live capture on the running original, the EXISTING `--lat-bracket`, no new probe.** Gate **CV PASS**: `{"armed":true,"a6a":2333,"a6b":2333,"sub":4666,"skipped":0,"err":null}`, orphans **0**, per-frame pattern `0,2,1,1` on **2333/2333 = 100.00 %** — which gives **U-9160 its ORIGINAL side live: `4666/2333` = exactly `2.0000` substeps per frame**, the first live witness for `0x00469ad4 mov ebx,2`. Gate **KA-B FAILED** (37.76 % within 4 ulps, worst 1.42e7) and is **reported as a failure, not amended**: it paired the probe's A6a-call ordinal with the `.msd`'s render-tick frame index as one counter. It does not touch the registered reading, because **`+0x9e0` at A6a entry has exactly ONE distinct value, `4`, across all 2333 frames** (A6a reads it twice and writes it **zero** times over all 1243 instructions). **BRANCH OPEN** (`G4` true 29/29, 23/23, 11/11), so `RESULT_STEP1B.md` §2.2's "the clamp's stores appear not to execute" is **WITHDRAWN** — A5's zero at `0x0046ddd1` is real but its per-wheel rebuild restores 4.0 before A6a every frame.
>
> **THE MEASUREMENT THAT CHANGES THE MAP (post-hoc, labelled).** A6b entry is the first sample after A6a returns and `+0x9e4` is written **only pre-clamp** (two literal writers `0x004686cc`/`0x0046bc36` plus A6a's entry `fstp 0x00467673`; the folded-base sweep over 622 511 instructions gives **4 reads / 0 writes**, with its `+0xbf8` known answer **PASSING**), so `R_c = |v|@A6b / (+0x9e4)@A6b` **is** clamp #6's own ratio with the clamp's own axis:
>
> | window | n | med speed @A6b | `1-R_c^2` | floor at `k >= 0.1249` | **measured / demanded** |
> |---|---:|---:|---:|---:|---:|
> | `d` 200..222 | 23 | 362.3 | 3.4453e-02 | 1.5075e-02 | **2.29** |
> | **`d` 222..250** | **29** | **879.9** | **6.2431e-04** | **3.8398e-04** | **1.63** |
> | `d` 250..260 | 11 | 1380.5 | 2.4712e-03 | 1.3794e-03 | **1.79** |
>
> **The original's grip-clamp #6 costs MORE than the structural minimum its own arithmetic demands, in every window. It behaves exactly as transcribed.** And on the **same capture** the render-tick split reports **1.2194e-05** — **51x smaller** (`R > 1` on **14 of 29** at the snapshot against **2 of 29** at the clamp's phase). **[U-9178]** So the `median(+0x9e4 / |+0x9b0..b8|)` instrument §23.2/§25/§26 all read "the clamp's cost" from is phase-biased **51x one way on the original and 10.3x the other on the port**: §23.2's published "the port loses 14.7 %/frame, the original 0.0002 %" and STEP 1B's **6 072x** factor are **instrument artefacts in unknown part**. `T_post`'s **naming** survives (the budget uses the snapshot symmetrically); its **decomposition and magnitude** do not.
>
> **U-9173 RESOLVED — a BANDING ARTEFACT, by a pre-registered prediction.** `PREREG_STEP1B.md` §5 registered before the run that the original's `s'` at matched `d = 222..250` would be **< 0.05**; measured **0.044451** (n=29, median speed 820.5) — **MET**. The same tool gives **0.727146** at band 100-150 (n=45, median speed 126.2, **median `d` = 133**), reproducing §26.4's **0.729** to three digits. Two different populations; they never had to agree. Also recorded: **U-9176 gains a SECOND RVA instance** — clamp #6's lateral construction, `0x00468771..0x00468793` against `Integrate2.cpp:666`, outside §21.5's audit scope because it precedes the arm split.
>
> **Determinism control, reported:** `orig_lb18.msd` (release **883**, `--lat-bracket`) reproduces `orig_sl1.msd` (release **890**, `--slide-probe`) **to every printed digit at matched `d`** — two boots, two probes, two release frames, identical numbers.
>
> **STEP 3, build byte-identical to attempt 17** (`git diff 6e512717..HEAD -- mashedmod/src` empty; **`NEW = 0` by construction**, no source change at all this attempt), participants=1 from the game's own `MATCH-SEED rule=0 participants=1` line, 3 runs (s2/s3 byte-identical; s1 differs only in tail capture length): **a** FAIL (no fix applied; `dS` +27.74798 vs -1.04312); **b** PASS (**L = 0 at 0.19 %**, `+0xb14` at `d` = 15 on both, peak **1835.50 at `d` = 95** against **1832.40 at `d` = 95**); **c** INCONCLUSIVE (**243/400 = 60.8 %** passes, median **132.8** fails, against 99.5 % / 1333.9); **d** FAIL 3/3 against the **unchanged** `d81a8df6` bounds — slip 1500-2000 **0.1983** (n=19, median speed 1665.05, median `d` 85), slip 2000-2600 **UNSCORABLE** (n=0), driving-median **1019.77** (n=76, median `d` 79), with §26.10's median-frame guard firing on all three (the ORIGINAL populates them at median `d` **722 / 916 / 844**, n = 318 / 554 / 1194).
>
> **Collateral.** Leg 1, paired same-side (attempt-17 `p1` vs this attempt's `s1`, floors `p2`/`s2`): **0 of 75 paired fields divergent, all 75 within the measured noise floor on every one of 1627 aligned frames**, A-only 0, B-only 0 — **no outside-scope rows**. Leg 2, cross-side banded: **all SIX bands `!!` OFF-REGIME** (median frame indices 1016-1675 against 68-381), so per §26.10 **not one row was read**; three fields exact in every band (`msd+0x498` 40000, `msd+0x49c` 4000, `msd+0x9e0` 4).
>
> **Still open:** **U-9178** (the phase-biased instrument and the port's unidentified ~90 % `T_post` sink — D2's blocker's new shape); U-9177; U-9176; U-9160 (port half); U-9156; U-9171; §20.14's `-0.1` duty cycle; D1-residue R1. **U-9173 is RESOLVED.** Evidence `verify/d2_budget_20261002/`.

> #### Re-close attempt 17 — 2026-10-02. **`+0xb0c` is RESOLVED as a SYMPTOM, not a defect, and D2's recovery gap is relocated to a per-frame speed-gain deficit that switches on at `d = 222`. No fix authored; D2 does NOT close.**
>
> `+0xb0c`'s writer is A4 `FUN_00470670` (`0x00470724` formula / `0x0047072c` zero branch) and it has **exactly two readers** in `.text` — A6a `0x004676de` (`fVar5 = max(1500.0 - b0c, 500.0)`, with `_DAT_005cd0ac = 1500.0` and `_DAT_005ccd04 = 500.0` read from the anchored binary) and the AI accessor `FUN_0046d6a0` `0x0046d6b6` (folded `0x008820ac`). Found with `findoffset.py` plus a new folded-base capstone sweep `re/tools/fold_sweep.py`, whose **first version was discarded** because its known-answer check on `+0xbf8` also returned 0.
>
> **Gate KA FAILED on BOTH routes and was retired, not amended** — snapshot `0.9488 / 0.9401 / 0.9453`, live `0.971698`, against a 0.99 threshold. Measured cause: `+0xb0c` is algebraically `speed - |dot|`, so a result-relative tolerance scores the subtraction's cancellation; KA's own worst miss is **0.24 of ONE float32 ulp**. The separately pre-registered **KA2** (4 ulps of `max(speed,1)`) **PASSES 2332/2332, max 2.034 ulps**, on the RUNNING original via the new `--slide-probe` A4-entry hook (coverage `calls 2333 / mine 2333 / skipped 0 / capped false / err null`). So the transcription is exact, the port computes the same expression, and **`+0xb0c` can only diverge through its seven inputs**.
>
> Gate **DR** names `speed` `+0x9e4` at `d = 0` (ORIG `0` exact on 890 consecutive rows, PORT `0.65`); `fwdlen` **never** exceeds 2 % anywhere. Gate **CB** is CB-LARGE over `d = 0..400` (66.65 % at `d = 372`) but **3.9892 % restricted to `d = 0..101`** (n=102), the window where the arms are still the same manoeuvre — and across that window their **median speed differs by 0.06 %** (609.07 vs 609.43).
>
> **THE DEFECT, relocated.** First durable divergence **`d = 222`**. Over **`d = 222..250`** (n=28, median speed O **805.7** / P **660.9**) the drive force agrees to **6.2 %** (median `|b14_xz|` 2.381e6 vs 2.233e6), **all four wheels are grounded on both sides** (`+0x9e0 = 4`, exact, every frame), both cars are aligned and in the same gear — and the median **per-frame speed gain is +27.87 against +4.86, a 5.7x deficit**. At `d = 200..222` immediately before, the PORT is **faster** (+19.06 vs +17.59). The gearbox collapse (port in gear 0 for 283 of 321 frames over `d = 200..520`) **follows** this; gear and its timer track exactly through `d = 0..250`. Named term, **reported UNFIXED** pending live confirmation: the consumer of `+0xb14`/`+0xb1c`, the velocity integration and its clamp chain. **[U-9177]**
>
> Also recorded **[U-9176]**: both port copies evaluate the `+0xb0c` dot product in a different x87 association order than `0x004706db..0x00470701` — float-rounding sized, named, located, deliberately left for an attempt that can carry the full promotion leg on A4.
>
> The release frame had to be **re-derived**: `a8_launch.py`'s `886` is capture-specific. `orig_sl1.msd` carries its own `+0xbf8` release marker at **890**; at `R = 890` both arms reproduce attempts 15/16 to every digit.
>
> **STEP 4** (only `mashedmod/src` change is a diagnostic `fprintf`; `NEW = 0` by construction): **a** FAIL (no fix applied; `+0xb0c` first exceeds 2 % at `d`=1); **b** PASS (**L=0 at 0.19 %**, `+0xb14` at `d`=15, peak **1835.50 at `d`=95** vs **1832.40 at `d`=95**); **c** INCONCLUSIVE (**243/400 = 60.8 %**, median **132.8** vs 99.5 % / 1333.9); **d** FAIL 3/3 — slip 1500-2000 **0.1983** (n=19, `d`=85), slip 2000-2600 **UNSCORABLE** (n=0), driving-median **1019.77** (n=76, `d`=79); `p1`/`p2`/`p3` identical to every digit, participants=1 confirmed from the game's own `MATCH-SEED` line.
>
> **Collateral:** paired same-side (attempt-15 `r1` vs this build) **0 of 69 divergent, all within the measured floor**, B-only exactly the 6 new diagnostic fields — **no outside-scope rows**. Cross-side banded: **all six bands `!!` off-regime**, so per §26.10 **not one row was read**. `verify/d2_b0c_20261002/RESULT_STEP1.md` + `RESULT_STEP2.md`.
>
> **Still open:** U-9177 (the `d`=222 velocity-integration deficit — D2's blocker); U-9176; U-9173; U-9156; U-9160; U-9171; D1-residue R1. **`+0xb0c` is closed.**

> #### Re-close attempt 16 — 2026-10-02. **U-9175 RESOLVED and CLOSED as a red herring. The drive-force Y `+0xb18` is a STRUCTURAL zero on the original (forward-Y `+0x9d8` is bit-exact `0.0` on every frame, live-tested) and a physically-negligible float-epsilon on the port (~`3.05e-08`). It is NOT the recovery defect and has no faithful single-producer fix, so STEP 2 was not entered, no value changed, and D2 does NOT close.**
>
> Live test on the running original (`--axis-probe`, two entry hooks A6a `0x00467650`-PRE ESI-filtered + A6b `0x00468980`-POST; known-answer self-check `DAT_00614708 == [0,0,1]`; 2179 A6a frames, 1275 active-drive, `err=null`; `PREREG_STEP1.md` committed `7e9cbf7c` before any run, no gate amended): the original's body forward-Y `+0x9d8` has **exactly one distinct value `0`** on every frame, all four wheel axis-Ys `+0x224/2e8/3ac/470` are `0.0`, and A6a leaves `+0xb18 == 0` at A6b entry. Since `+0xb18 = Σ axisY*force`, a zero axis-Y zeroes it at any force — **STRUCTURAL**, H-later refuted. Writers: `+0xb18` by A6a `0x00467cc5`/`0x00467d97`; axis-Y by A5 `FUN_0046ddb0` `0x0046de74` from body forward written `0x0046ddc9` (`xform*(0,0,1)`); A4 zeroes b14/18/1c each frame at `0x0047072c`.
>
> The PORT's forward-Y is a `~3.05e-08` median epsilon because `omega.x/z` (`+0x9bc/+0x9c4`) carry `~5e-10` median FP noise (max 0.013) from the contact/suspension torque sum, which `BodyOrientationIntegrate FUN_0046e9e0` (`at.y += omega.z*at.x - omega.x*at.z`) drifts into the matrix at-row Y; the original's omega.x/z are exactly 0 on flat ground. The snapshot `+0xb18` median `0.0282` is that epsilon amplified by the large drive/boost multipliers (`ff = 5e6`) — confirming the logic. Velocity-Y effect `~5e-7`/frame (`Integrate2.cpp:640`) — it cannot be the ~10x X/Z-plane recovery deficit. There is **no single faithfully-portable producer** (diffuse FP noise across the torque chain); forcing `at.y = 0` would be a forbidden clamp. STEP 2 NOT entered per `PREREG_STEP1.md` §5.
>
> **Build byte-identical to attempt 15** (`git diff 29bd7619..HEAD -- mashedmod/src` empty; only the read-only `--axis-probe` + docs). STEP 3 reproduces attempt 15 to every digit: **a** FAIL (`+0xb18` nonzero 1618/1633, median 0.0282); **b** PASS (L=0 at 0.19%, `+0xb14` at `d`=15, peak **1835.50 at `d`=95** vs 1832.40); **c** INCONCLUSIVE (**243/400 = 60.8%**, median **132.8** vs 99.5% / 1333.9); **d** FAIL 3/3 (slip 1500-2000 **0.1983** n=19 `d`=85, slip 2000-2600 UNSCORABLE n=0, driving-median **1019.77** n=76 `d`=79). The recovery gap lives in the **X/Z plane**, not the Y channel; `+0xb14`/`+0xb1c` first diverge at the engagement frame `d`=15 by only 1.57%/1.03%, so the launch force is faithful and the collapse is downstream in the velocity/clamp integration. `RESULT_STEP1.md`.
>
> **Still open:** `+0xb0c` (next first-diverging term, errs both directions, no single-constant fix); the recovery gap (X/Z plane); U-9173; U-9156; U-9160; U-9171; D1-residue R1.

> #### REOPENED 2026-09-29 (user decision) — the closing evidence was not like-for-like
>
> **Status: REOPENED.** The **CLOSED 2026-09-14** block below is retained as history, not
> deleted. Its amendments remain accurate about what they measured; what changed is the
> verdict that D2 was closeable on that evidence.
>
> **Why.** The closing evidence compared a **three-opponent port run** against a **ONE-car
> original capture**. `verify/a8_steer_20260824/orig_steerR.msd.provenance.json` carries no
> `--cars`, and `re/frida/scenario_launch.py:1739` defaults it to **1**, while the port arm
> hard-spawns three (`TrackRenderer.cpp:2441-2473` / `StartRound`). On the **matched solo
> scenario** (`MASHED_MEASURE_SOLO=1`) the port has **never** reproduced the original:
>
> | metric | ORIGINAL (solo) | port (HEAD solo) | delta |
> |---|---:|---:|---:|
> | slip 1500-2000 | **0.1913** | 0.1332 | **-30%** |
> | slip 2000-2600 | **0.2498** | 0.2179 | **-13%** |
> | driving-median | **1941** | 1818 | **-6%** |
>
> Evidence: [`re/analysis/PLAYER_REGRESSION_2026-09-29.md`](re/analysis/PLAYER_REGRESSION_2026-09-29.md)
> §5-§6. Note what this does **not** overturn: that note's finding that there is no player
> physics *regression between commits* stands (HEAD vs `56ad3806` is `-3.1% / -0.1% / +3.3%`
> on the matched arm). The port is short against the **original** at both commits, which is a
> D2 question that the three-vs-one asymmetry hid.
>
> **The gate is now:** the D2 metrics on the **SOLO arm** (`MASHED_MEASURE_SOLO=1`) measured
> against the **original's solo capture**, within bounds **pre-registered before the fix**.
> Pre-registering the bounds first is not ceremony here — `a-band-scored-off-regime-is-not-a-measurement`
> and `pre-register-the-decision-not-the-diagnosis` are both live precedents on this exact lane.
>
> **Open items blocking re-closure:**
> - **U-9149** — A6b `0x00468980`'s context pointer comes from the stack slot at
>   `0x0047093b` in the original. It is **dead in the exe** (`VehicleControl.cpp:195` passes
>   `nullptr`) and **contradicted in the `.asi` forwarder** (`PhysicsChainHooks.cpp:220`
>   `mov esi, ecx`), so neither copy is established. Both `0x00468980` and `0x00470670` were
>   demoted C4→C2 on 2026-09-29 (`re/analysis/DUAL_COPY_FIX_2026-09-29.md`).
> - **U-9147** — the standing slip gap above.
>
> **Order: D2 must close again BEFORE the D3 modes 3/7 port starts.** §D3's closure path
> (`FUN_00414c30` + `FUN_00484c70`) is on hold until then.

> #### Re-close attempt 15 — 2026-10-01. **U-9174 RESOLVED and D2's LAUNCH CLOSED IN THE DEFAULT BUILD. The `+0xbf8 = 2` writer is `0x0046d7a2` inside `FUN_0046d780`, the 15-frame hold is the LOSING branch of a start-line rev-charge mini-game, and with it ported at its own RVA — no knob anywhere — the port's lag goes to 0 and its peak lands on the original's own frame. The three scored metrics STILL FAIL 3 of 3, so D2 does NOT close.**
> 
> **The writer, and why three attempts missed it.** The vehicle array is `0x008815a0` stride `0xd04`, so MSVC folds `&veh[i].+0xbf8` into `i*0xd04 + 0x00882198` and the instruction contains **no `0xbf8` at all** (stride witness `imul eax,eax,0xd04` at `0x0046d78e`). A capstone sweep for the folded absolute operands found it; a Ghidra decompile of **all 6239 defined functions**, 0 failures, grepped for `0xbf[048c]` returns `FUN_00467650` alone and would **not** have found it. Both instruments were needed — the second is why no other writer is claimed. CONFIRMED on the RUNNING original, **two boots, entry hooks only**: entered exactly once for car 0, `bf8 0 -> 2` with `bf4` 3000 unchanged, 112 charge ticks before it rising `+50`/tick, and the same run's `.msd` carrying `bf8 == 2` on exactly 14 frames at `-200`/frame. `verify/d2_writer_20261001/PREREG_STEP1.md` + `RESULT_STEP1.md`.
> 
> **The mechanic.** `FUN_0046d7f0` (`0x0046d7f0`) adds **50** per pre-race state-tick to `veh+0xbf4` while the accel byte exceeds `160.0`, clamped `[0,3000]`; the 50 is the `push 0x32` at `0x0042c980`/`0x00492d83`. At the 1.86 s mark (`_DAT_005ccdf4`, test `0x00410460`) `FUN_0046d780` converts it: charge **> 1000** gives state **2**, an OVER-REV BOG whose countdown is the measured **3000 / 200 = 15** frames; charge <= 1000 gives state 1. The D2 arm holds full accel through the countdown, so the original **always** takes the losing branch.
> 
> **The port.** ONE body per RVA in the new `Vehicle/LaunchRevCharge.cpp`, in **both** `exe_sources.rsp` and `asi_sources.rsp`; `0x0046d7f0`'s body MOVED there out of asi-only `PromoLoop_sessionB.cpp`. `0x0046d780` **C2 -> C3** (path1 GREEN 8/8 non-degenerate + path2 install PASS), `0x0046d7f0` C3 re-affirmed — and a LATENT GAP closed: its `arg_type` had no handler in `diff_template.js`, so `run_diff.py` refused the row outright and it could not be re-verified at all. **`MASHED_D2_BOOSTHOLD` is REMOVED from the code.** Build OK, `rva-lint NEW=0`, no duplicate body by hand grep.
> 
> | gate | result |
> |---|---|
> | **a** launch | **PASS 3/3** — lag **L = 0** (0.19 %, against 49-53 % at L=14/15/16), `+0xb14` engages at **`d` = 15**, peak **1835.50 at `d` = 95** against **1832.40 at `d` = 95** |
> | **b** recovery | **INCONCLUSIVE** as registered — **243/400 = 60.8 %**, median **132.8**, against **398/400 = 99.5 %** and **1333.9** |
> | **c** metrics | **FAIL 3/3** — slip 1500-2000 **0.1983** (n=19, med spd 1665.05, med frame `d` 85), slip 2000-2600 **UNSCORABLE** (n=0), driving-median **1019.77** (n=76, med frame `d` 79) |
> 
> The port computes the original's own saturated charge of **3000** for itself. **These are attempt 14's knob-ON numbers to every digit**, so the fitted trigger was a faithful stand-in and the residual gap **survives the real fix**. §26.10's median-frame guard fires on every scored row (ORIG median `d` **720 / 909 / 824**), so the magnitudes are not readable as physics errors — the bounds are still scored and not met. D-0: the control arm is bit-identical to attempt 14's `s1` on all 1625 shared lines. One VOID run disclosed (null record array during the countdown; fixed by hoisting the physics init above it, which is the original's own ordering).
> 
> **The downstream term, NAMED and WITNESSED but NOT fixed.** With `L = 0` the arms can be compared at the same `d` for the first time in four attempts. **[U-9175]** the drive-force **Y component `+0xb18` is exactly `0.0` on 9336 of 9336 frames across FOUR original captures** while the port writes it non-zero on **385 of 400** post-release frames — **pre-existing**, binary rather than a magnitude. `+0xb0c` is confirmed as the first diverging term (`d` = 1) and is **NOT CLEAN**: too high early, **64x too low** later. No fix authored: `PREREG_STEP3.md` §3 requires a live original-side test first. `RESULT_STEP3.md` + `RESULT_STEP4.md`.
> 
> **Still open:** U-9175; `+0xb0c`; the recovery gap; U-9173; U-9156; U-9160; U-9171; D1-residue R1.

> #### Re-close attempt 14 — 2026-10-01. **STILL REOPENED. The two arms DO share their input schedule — the ORIGINAL begins steering from EXACTLY 0.000000 speed — and release-aligned the port's LAUNCH is faithful to 1.44 % fifteen frames early. Both sides then peak within 1.3 % and crash to the same trough; the ORIGINAL recovers to a median of 1828.5 and the PORT's ceiling is 91.4. D2's defect is the RECOVERY. NO fix authored: the mechanism behind the 15 frames is proven but its trigger is unlocated [U-9174].**
>
> **Step 1 refuted the input-schedule hypothesis.** Over frames 0-885 the ORIGINAL's horizontal
> speed is **exactly `0.000000` on every frame**; its release is frame **886**, witnessed three
> ways in the record (`+0xbf4` holds 3000 then counts down -200/frame; `+0xb24` first ticks 50;
> `+0x1a8` first reads 17.07471). `+0x190` is `34.0000` on all 2333 frames, so
> `255/256 * 34 * 0.5 * (6050/6000) = 17.074707` identifies the consumed steer byte as **255**.
> Release-aligned **every** input channel agrees to **0.000 %** — byte, throttle, sign, ramp
> start, step `+0.141113`, saturation on the 120th steering frame — and the port holds
> `in=(255,0,255,0)` on **4880 of 4880** frames. **The arm needs no change and there is no
> proposal for the user.** Steps 1 and 2 each had a gate FAIL AS WRITTEN (G2/G3 on a 5-vs-3
> frame proxy lag; GA on phase), neither was amended, and GA's own registered remedy measured
> `phi = 0`.
>
> **The named first diverging term: T2 `+0xb14`/`+0xb18`/`+0xb1c` at `d = 0`** — an **engagement
> latency**, not a magnitude error (`port(d=0)/orig(d=15)` = x 0.8985, z **1.0014**). The lag fit
> has a single sharp minimum at **L = 15** (median 1.44 %, p90 1.65 %, n=80), flat across the
> whole launch, and it explains **nothing** after the crash (97.4 %, n=391). Mechanism, proven on
> `orig_solo3`/`solo4`/`fp1` identically: `+0xbf8 == 2` for exactly 14 frames from release,
> `+0xbf4` counts 3000 down by 200/frame to 0 at `d = 14`, `b14` engages at `d = 15`, and
> `(+0xbf8 == 2) <=> (b14 == 0)` on **1446 of 1447** frames. `3000/200 = 15`. The arm is
> `0x00467def..0x00467e44`.
>
> **THE REFRAMING, release-aligned and frame-indexed:**
>
> | | peak | at `d` | trough | at `d` | frames after | `>= 100` | median | max |
> |---|---:|---:|---:|---:|---:|---|---:|---:|
> | **ORIGINAL** | **1832.40** | 95 | 85.45 | 101 | 1346 | **1313 (97.5 %)** | **1828.5** | 2562.8 |
> | **PORT** | **1856.57** | 80 | 85.81 | 139 | 1488 | **0 (0.0 %)** | **24.7** | **91.4** |
>
> Both arms accelerate the same, reach the same speed and hit the wall (ORIG `d = 94`, PORT
> `d = 80`). **The original drives away; the port does not.** This is §20.14's loop with a hard
> frame count attached — it is **quantified**, not superseded. Everything three attempts measured
> inside A6a on this arm was measured at or after the crash, where the port holds a median of
> 24.7; the only common-regime stretch is the **80-frame launch**, and there the port is faithful
> to 1.44 %.
>
> **NO fix authored.** All six literal-displacement writers of `+0xbf8` in the image write
> **ZERO** (`xor eax,eax` at `0x00467dd5`/`0x00467e34`); the 7th byte hit decodes as
> `mov [ebp-8], 0xb` and is discarded. **[U-9174]** the trigger uses a computed base and is not
> located, and fitting one would violate NO-GUESSING. Ghidra MCP was **down for the whole
> session**.
>
> **Scored control 3 of 3, identical to attempts 11/12/13** (`participants=1`, muted, own PIDs,
> no source edited so no build): slip 1500-2000 **0.2033** (n=20, median speed 1683.5, **median
> frame 71 = `d` 70**) FAIL; slip 2000-2600 **UNSCORABLE** (n=0) FAIL; driving-median **1355.66**
> (n=54, **median frame 54 = `d` 53**) FAIL −30.2 %. **The scored metrics themselves fail
> §26.10's median-frame guard** — the ORIGINAL populates them at `d` 719/908/797. The §3 bounds
> are untouched.
>
> **Collateral: the cross-side banded review has ZERO readable rows** — all 7 bands of all 45
> paired fields `!!`-flagged. Detail: `verify/d2_sched_20261001/RESULT_STEP{1,2}.md`,
> `D2_REOPEN_2026-09-29.md` §27.

> ##### Attempt 14, steps 2B / 2C — 2026-10-01. **STILL REOPENED, and the map changed. Supplying ONLY the missing `+0xbf8 = 2` trigger makes the port's launch the ORIGINAL's to 0.15 % with the peak on the SAME frame, and moves it from NEVER recovering to PARTIALLY recovering. The registered discriminator returned INCONCLUSIVE. U-9174 is now ON D2's critical path. NO fix authored — the trigger is still unlocated and a fitted one may not ship.**
>
> **Step 2B.** GE/GH/GG **passed**; **GF FAILED** with **zero** readable buckets, so the
> registered first-divergence rule did not execute and was not amended. The occupancy table is
> the result: in the matched-steer window `d`[119,300], **182 grounded frames each**, the
> ORIGINAL spends **1 of 182** at `cos(fwd,vel) < -0.1` and the PORT **86 of 182**; the ORIGINAL
> is at speed `>= 150` on **150 of 182** and the PORT on **0 of 182**. **Not one bucket is
> shared.** That is the **third** instrument this attempt to return "no overlap", after §26.9's
> 5 common frames and step 2A's zero readable bands.
>
> **Step 2C — the experiment.** `MASHED_D2_BOOSTHOLD=1`, **default OFF**
> (`Integrate2.cpp:316-352`): one write per car per race, `+0xbf8 = 2` and `+0xbf4 = 3000`, on
> the first frame `input[0] != 0`. The hold itself comes from the original's **own transcribed**
> `+0xbf8 == 2` arm (`0x00467def..0x00467e44`). **The trigger is FITTED** — labelled, no
> C-level, absent from every scored arm, and barred from shipping while U-9174 is open.
> `=== Build OK ===`, `allowlisted=122 **NEW=0**`; **D-0** the knob-OFF run is **bit-identical
> to `s1` on all 1629 shared lines**; **D-1** `b14` is exactly 0 on `d`=0..14 and engages at
> `d`=15 at `(-270030, 0.0218758, -761363)` against the ORIGINAL's `(-265866, 0, -762420)` —
> **x 1.57 %, z 0.14 %**; **D-2** three runs identical; `participants=1` throughout.
>
> | best-fit lag over `d`=16..95, n=80 | L=0 | L=15 | **BEST** |
> |---|---:|---:|---|
> | PORT knob OFF | 89.07 % | **1.44 %** | **L = 15** |
> | PORT knob ON | **0.15 %** | 49.21 % | **L = 0** |
>
> **The lag goes to zero and the launch error improves tenfold.** Peak **1835.50 at `d = 95`**
> against the original's **1832.40 at `d = 95`** — same frame, 0.17 % apart. Fourth independent
> confirmation of `3000 / 200 = 15`, and the strongest.
>
> **The discriminator**, 400 frames after the trough: ORIGINAL **397/400 (99.2 %)**, median
> **1333.9**; PORT knob ON **243/400 (60.8 %)**, median **132.8**; PORT knob OFF **0/400
> (0.0 %)**, median 31.5. Registered rule H1 iff `>= 50 %` **and** median `>= 900`, H2 iff
> `< 10 %`. **60.8 % passes, median 132.8 fails -> INCONCLUSIVE**, no third branch added.
> **Both surviving hypotheses have support and neither is complete.**
>
> **Scored DIAGNOSTIC arm, 3 of 3 identical, all three still FAIL** (labelled diagnostic; a
> fitted trigger can never re-close D2): slip 1500-2000 **0.1983** (n=19, median speed 1665.1,
> **median frame 86 = `d` 85**) — error halves from **+5.6 % to +3.0 %**, missing the bound by
> **0.00195**; slip 2000-2600 **UNSCORABLE** (n=0); driving-median **1019.77** (n=76, **median
> frame 80 = `d` 79**), which moves the **wrong** way while its `n` rises **54 -> 76**, because a
> partially-recovering car adds frames just above the 500 floor.
>
> **Collateral** (same-side paired, `s1` vs `bh1`, floors on both sides): 47 of 69 divergent,
> **22 within floor including all four input bytes and `io.steer`** — **no input channel moved**.
> First divergence `b14` at frame 2, alone; everything else at frame 3. **No outside-scope rows.**
>
> **What changed in the map.** U-9174 is **amended onto D2's critical path**. The knob gives the
> next attempt the **overlapping state** every instrument this session lacked. The **residual
> recovery gap is a real second defect** and is the remaining D2 lane. Detail:
> `verify/d2_sched_20261001/RESULT_STEP2BC.md`, `D2_REOPEN_2026-09-29.md` §28.

> #### Re-close attempt 13 — 2026-10-01. **STILL REOPENED. The `l_60` call-site attribution is VERIFIED and attempt 12's `l_60 >= 79 240` is WITHDRAWN (the ORIGINAL is on clamp #6's LOW arm, where `k` is pinned at its 0.1 floor) — and then step 2 found that EVERY cross-side band above 100 speed in this lane is OFF-REGIME. The two sides share FIVE frames of common regime. That withdraws §21.9's `ld4` 2.71x, §21.10's front-axis defect and this attempt's own `l_60` ratio. NO fix authored.**
>
> **STOP MEASURING INSIDE A6a.** Regime = steer saturated (`+0x1a8 >= 33.8`) and speed `>= 100`
> and grounded: the ORIGINAL has **1329 of 1329** frames after saturation (100.0%, median speed
> **1847.0**, peak 2562.5); the PORT has **5 of 1507** (0.3%, median speed after saturation
> **22.2**, max 118.6). The port peaks at **1831.5** during its steer ramp — the original at
> 1815.4 — then **collapses inside its first ~121 frames and never recovers**. That collapse is
> D2's defect; `l_60`, `ld4`, `grip*speed`, the clamp arms and the wheel axes are downstream
> scenery. Detail: `verify/d2_l60_20261001/RESULT_STEP2.md`, `D2_REOPEN_2026-09-29.md` §26.9.
>
> **Three withdrawals from step 2.** (1) Step 0's `+0x1a8` row — both sides' steer ramps are
> identical (start 17.07471, +0.141113/frame, saturate 33.86719 on the 120th steering frame;
> `ramp = (min(+0xb24,6000)+6000)/6000` at A4 `0x0047080c..0x0047082f`, so it ramps in **time**).
> (2) §21.10's "front-axis 10.0/15.4/20.8% short" and its 10.9% of the `ld4` gap — scored
> against the port's OWN `+0x1a8`, `(front − rear) + steer` has median residual **+0.000002** on
> both sides (n=1626 / n=1448), so the port's wheel axes are **exact** and U-9156's "its writer
> is not located" is moot. (3) Every cross-side band above 100 in §21.9 / §21.10 / §25.3 and in
> this attempt's own step 1.
>
> **Instrument fix:** `collateral.py --mode banded` now prints the median **frame index** per arm
> per band, flags bands whose two medians differ by >50%, and prints an OFF-REGIME block. It
> flags every band from 100-150 to 1500-2000 on step 0's own table.
>
> Full record: `verify/d2_l60_20261001/` — `PREREG_STEP1.md` (committed before any reduction
> run, **not amended**), `RESULT_STEP0.md` (the retroactive collateral review), `RESULT_STEP1.md`.
> **No game was launched for the measurement**: `--mag-probe` and `--fixup-probe` had already
> captured every channel it needs (memory `grep-the-harness-for-the-rva-before-writing-a-probe`).
>
> - **Step 0 — a standing collateral instrument.** `re/tools/statediff/collateral.py` (new):
>   `msd:`/`a6a:`/`kv:`/`csv:` channels, a noise floor measured from a same-arm repeat pair,
>   `--anchor` bounce alignment, speed banding, and two modes — `paired` (same side) and
>   `banded` (cross-side; frame-pairing two separated trajectories reports the alignment, not
>   the field). Checked first that `statediff.py` / `field_trace.py` / `msd_fields.py` do not
>   already do it. Measured: the **PORT's noise floor is EXACTLY ZERO** (205 of 205 fields
>   bit-identical on 1627 of 1627 frames); the ORIGINAL's is 533 of 833 record dwords
>   bit-identical over 2332 frames. A6a's write set (20 slots, 22 CALL sites, 7 callees) is
>   committed as `re/tools/statediff/scope_a6a.txt`. One outside-scope row, **exploratory**:
>   record `+0x1a8` (the steer angle A4 `0x00470670` writes, which A6a never touches) is
>   **33.867 flat on the original in every band** and short on the port in all six, worst
>   **41.2% at 260-500**. §21.10 never compared this — its port column was the input command.
> - **Step 1 — the attribution.** `l_60`'s slot is frame **−96** with **exactly four**
>   accesses in all 1243 instructions of A6a; its one accumulate is
>   `l_60 += mag@0x0046820f * [frame −228]` written by `0x004680fb`, i.e. **exactly the two of
>   the nine `RwV3dLength` return sites `a8_l60.py` used**. Known-answer re-run, not inherited:
>   **1424 of 1424** exact. Value reproduces at **268.587** (n=45, median speed 126.2) against
>   §21.9's 285.243 (n=35).
> - **The withdrawal.** The ORIGINAL's `grip × |vel|` at 100-150 is **30 785.1**, below the
>   `_DAT_005ce9fc = 32768` knee, so it takes the LOW arm where
>   `k = max((32768 − G) · 2^-15, 0.1) = 0.1` and can never be 0. §25.3's chain
>   "no-op ⇒ `k = 0` ⇒ only the HIGH arm ⇒ `l_60 ≥ 79 240` ⇒ 278x" **does not start**.
>   **U-9172 is withdrawn to the resolved audit trail.**
> - **The cross-side number U-9172 asked for**, verified on both sides, n and median speed on
>   every row: `l_60` **2.15x** short on the port at 100-150 (268.587 n=45 spd 126.2 against
>   the directly-logged 124.737 n=19 spd 115.3), then 1.59x / 1.79x / 1.47x / 1.18x / **1.11x**
>   at 1500-2000. At 100-150 **both sides are on the LOW arm and the port's `k` is 5.41x the
>   original's** (0.540954 against the 0.1 floor).
> - **What replaces U-9172: [U-9173].** Clamp #6 is a **LATERAL damper**, not a speed clamp
>   (`0x00468771..0x004687d7` builds `vel − dot(fwd,vel)·fwd`; both arms write `vel −= k·lat`),
>   so `|v'|/|v| = sqrt(1 − (2k − k²)s²)`. Three measurements then collide: LOW arm ⇒ `k = 0.1`;
>   the original's measured `s` = **0.729** (n=45); §25.3's `+0x9e4/|v'|` = **1.000000**
>   (2331/2331). The first two predict **1.0547**. A6b `0x00468980..0x00468b34` is ruled out
>   statically as the explanation (132 instructions, no velocity / forward-axis / `+0x9e4`
>   write). Next: ONE entry-only probe at **A6b's entry `0x00468980`**.
> - **Scored 3 of 3, no physics source changed**, identical to every digit and to attempts
>   11/12: slip 1500-2000 **0.2033** (n=20, median speed 1676.5) FAIL; slip 2000-2600
>   **UNSCORABLE** (n=0); driving-median **1355.66** (n=54) FAIL −30.2%. `participants=1`,
>   `allowlisted=122 NEW=0` (re-run fresh — the lint log on disk was stale from 29/09).
>   Collateral on this run's arms: **0 of 69** `motion_diag` fields divergent on 1598 frames.

> #### Re-close attempt 12 — 2026-10-01 (SUPERSEDED by attempt 13 on its `l_60 >= 79 240` conclusion). **STILL REOPENED. The ORIGINAL has NO sub-500 sink — attempt 11's was a back-out artefact, 198x wrong — and the NAMED sink is the PORT's own clamp-#6 velocity write inside A6a `0x00467650`, whose sole remaining lever is `l_60`. NO fix authored: the lever's value on the original is two conflicting numbers.**
>
> Full record: `re/analysis/D2_REOPEN_2026-09-29.md` **§25** (§25.5 first, then §25.2/§25.3).
> Pre-registered three times, each before the thing it governs ran, **none amended**:
> `verify/d2_sink_20261001/PREREG.md` (`26b859fd`), `PREREG_2.md` (`0d5ff8b7`), `PREREG_3.md`
> (`a17cf0c0`). Results: `RESULT_STEP1.md`, `RESULT_STEP2.md`, `RESULT_STEP3.md` in the same
> directory.
>
> - **Step 1 (R4).** One default-OFF `MASHED_A5GDIAG` line in A5 Phase 4 logs the port's `G` and
>   `local_70` directly. Channel control passed all three gates (unarmed run: **no** log; armed:
>   **1627** lines, matching `a6a_dump.log` exactly) and the pairing is proven at median rel
>   **7.6e-09** against **11.0** one frame off. KA-1 **1.814e-05** (bar 1e-3), KA-2 **0.0533**
>   (bar 0.10). **The port's `G` is `0.290250033` — the original's to every digit** — so §24.4's
>   0.85 `l70G` ratio is **all** `local_70` (0.850 vs 1.000, **U-9171**, and its sign makes the
>   port *faster*, so it is not the trap).
> - **Step 2 (R3).** The entry hook §24.6 asked for **already existed** (`--fixup-probe` site 2
>   **is** `0x00467650`) and the attempt-10 capture already carried **2331** of its rows from the
>   same run as the reference `.msd`. No hook added, no game run. Join **bit-exact** (median rel
>   `0.000e+00` on 1443 frames, runner-up 9.9e-03). The original's pre-A6a change is a **pure
>   scalar** (spread 3.813e-08, n=1432). **M3 failed in 5 of 6 bands, worst 0.995 against a 0.25
>   bar: the direct `resid` at 100-150 is −0.0655 where §24.3's back-out said −12.9566.**
>   §24.3's ORIGINAL `resid` column, §24.4's `local_70` of 182.5, and **U-9170** are withdrawn.
>   The pre-A6a scalar **agrees** across sides, 0.92x..1.16x over 14x in speed.
> - **Step 3. SINK NAMED**, all three parts of the registered naming bar met. The frame-to-frame
>   net at 100-150 is **ORIGINAL +6.148 per frame against PORT −14.940** — opposite sign — while
>   at and above 260 it agrees 0.82x..1.05x. On the running original the clamp is a **no-op at
>   every speed** (`+0x9e4 / |vel|` = 0.999991..1.000020 at the substep entry, n=12..678,
>   coverage **2331 of 2331**), and the port's clamp loss of 18.58 accounts for **0.881** of the
>   21.09 divergence. RVAs: multiplicand `0x004687db`, arms `0x00468833` / `0x004688ca`, gates
>   `0x0046874c` + `0x00468761`, full stop `0x00468939`..`0x00468954`.
> - **An ESP-walk sign error was caught and corrected before the conclusion landed** (Ghidra:
>   the multiplicand is `fVar5`, the post-W1 `|vel|`, so the port's binding is **faithful**).
>   With every other input measured identical, **`l_60` is the sole lever**, and the no-op forces
>   the original's `l_60 >= 79 240` at speed 126 against §21.9's **285.2** — **278x**.
>   **[U-9172]**, and **§21.10's "the clamp-#6 / `l_60` lane is MEASURED OUT" is WITHDRAWN.**
> - **Scored 3 of 3** at `a17cf0c0`, identical to every digit and to attempt 11, `participants=1`,
>   `allowlisted=122 NEW=0`: slip 1500-2000 **0.2033** (n=20, median speed 1676.53) **FAIL**;
>   slip 2000-2600 **UNSCORABLE** (n=0); driving-median **1355.66** (n=54) **FAIL** −30.2%.
> - **NEXT LANE, one measurement:** resolve **U-9172** — measure the ORIGINAL's `l_60` with a
>   **verified** call-site attribution (identify the accumulator divided at
>   `0x004686b3`..`0x004686be` in Ghidra first, then hook it on the running original and band it).

> #### Re-close attempt 11 — 2026-10-01 (SUPERSEDED by attempt 12 on its headline; §25.5 lists exactly what is withdrawn). **STILL REOPENED. `accum` is MEASURED and is NOT the carrier; §23's `T_rest` is 88-96% a velocity change OUTSIDE A6a; the port's whole share of it is A5's Phase-4 drag at `0x0046ddb0`; and a further sub-500 sink on the ORIGINAL is the new target. NO fix authored — two registered thresholds failed and both refusal branches fired.**
>
> Full record: `re/analysis/D2_REOPEN_2026-09-29.md` **§24** (§24.6 first, then §24.3/§24.4).
> Pre-registered twice before any run and **neither amended**:
> `verify/d2_accum_20261001/PREREG.md` (`51531edf`) and `PREREG_C.md` (`5667dbc2`). Results sheet
> with every number: `verify/d2_accum_20261001/RESULT.md`. Commits `51531edf`, `37220f4d`,
> `5667dbc2`.
>
> **Scored 3 of 3 at `37220f4d`, a no-change control — this lane changed no source:**
>
> | metric | port | n | median speed | PASS interval | verdict |
> |---|---:|---:|---:|---|---|
> | slip 1500-2000 | **0.2033** | 20 | 1676.53 (in band) | 0.18855 .. 0.19635 | **FAIL** +3.5% |
> | slip 2000-2600 | **—** | 0 | — | 0.24488 .. 0.25487 | **UNSCORABLE** |
> | driving-median | **1355.66** | 54 | 1355.66 | 1904.70 .. 1982.44 | **FAIL** −30.2% |
>
> Identical to every printed digit 3/3; bounds unchanged; `participants=1` confirmed;
> dual-copy guard `allowlisted=122 NEW=0`.
>
> **What moved.** `a11_accum.py` replays A6a's block #5 (`0x0046833a..0x00468544`), the blend
> (`0x004685b2..0x00468625`) and W1 (`0x0046862d..0x004686a2`) **from the render-tick snapshot
> alone** — no hook, no probe, no source change — and reproduces `friction_diag.log`'s verbatim
> `accum` with **cosine 1.000000000 on 1628/1628** frames (S1 passed by three orders). So §23.4's
> one unmeasured term is measured on both sides, **and it agrees**: `T_accum` is ORIG **−1.3468**
> vs PORT **−1.3396** at 1500-2000 (n=339/20), i.e. **−0.59..−1.35** where `T_rest` is
> **−4.8..−20.4**. S3 failed on the original (1.281e-02 vs 1e-3) and S5 failed on **both** sides
> in **every** band, so the registered rule did not execute: **no input named, no fix authored,
> no threshold amended.**
>
> With `accum` known, `T_rest` splits exactly (`T_accum + curv + resid`, identity residual
> **1.776e-14**, `curv < 0` on **0 of 3074**), and **`resid` is 88-96% of `T_rest`** on the
> original. **`resid` is outside A6a, proven**: rebasing on the port's `act.vel` entry channel
> gives `residA` = **−0.0000/−0.0001 in every band**. It is a **pure scalar multiply** of the
> velocity (per-component spread **3.8e-08** median on 1628/1628, `dvPerp` exactly **0.0000**),
> **not** clamp #6's `kVel`, and **quadratic in speed** (**4.1e-6..4.8e-6** over a 20x range).
> Cross-side ORIG/PORT: **240x** at 70-100, **50x** at 150-260, **1.18x** at 1000-1500.
>
> **Sub-lane C.** C1 = A5 `VehicleWheelForceIntegrate` **`0x0046ddb0`** Phase 4
> (`ForceIntegrator.cpp:86-90` + `:164-168`). **T1 PASSED at 1.053** — the port's
> `l70G = (1−σ)/(linTerm·s_mid_prev)` is constant to 5.3% over 8 bands, so **C1 is the port's
> entire pre-A6a velocity sink**. **T2/T3 FAILED** — the original's backed-out `local_70` runs
> **1.000..182.5** against the legal `[0,2]` its own construction permits (no speed term
> anywhere), with `G` **exactly constant** at `0.29025` on 1446/1446 steps. Above ~1000 the
> original obeys C1 with `local_70` = **1.000 exactly** (n=200) and the cross-side ratio is
> **0.85 / 0.66**, inside the registered [0.5,2.0]; **below ~500 there is a further sink C1 cannot
> produce and the port lacks entirely.** D4 fired, and D3 had refused a fix in advance because the
> port's `G` is in no existing log. **U-9170 filed** — speed-coupled vs slip-coupled is undecided
> because every original sample below 500 is its single post-bounce pass.
>
> **Routes closed, added to §23.4's ten:** (11) `accum` as the diverging input; (12) `frac`,
> `l_d0`, `m78`, `cMag`, `Sf − Sc`; (13) the whole of A6a as the home of `T_rest`; (14) C1 as a
> cross-side divergence **at speed**. **Correction:** §22.2's "A6a is the only writer of `+0x9b0`
> on a non-contact frame" is true of the **function** but was used for three attempts as a claim
> about the **snapshot phase gap**, which it is not.
>
> **Next, in order:** (1) log the port's `G` (bytes `0x150/0x154/0x158`) + A5's actual `fVar4` and
> `local_70`, one default-OFF line, one port run, no original run; (2) an **entry hook** on
> `0x00467650` so the original's `σ` becomes a measurement; (3) resolve U-9170, then name the
> sub-500 sink by RVA.

> #### Re-close attempt 10 — 2026-10-01 (SUPERSEDED by attempt 11 on its NEXT COMMAND; its numbers stand). **STILL REOPENED. The between-contact budget is decomposed completely, FOUR more routes close, and NO fix was authored because two registered safety thresholds failed and the surviving term is one §21.5 already proved byte-faithful.**
>
> Full record: `re/analysis/D2_REOPEN_2026-09-29.md` **§23.1** (the rule, committed before any
> run, `bccaf40b`) and **§23.2-§23.4** (the result). Results sheet with every number:
> `verify/d2_gain_20261001/RESULT.md`. Commits `bccaf40b`, `6d664146`.
>
> **Scored 3 of 3, a no-change control — this lane changed no source:**
>
> | metric | port | n | median speed | PASS interval | verdict |
> |---|---:|---:|---:|---|---|
> | slip 1500-2000 | **0.2033** | 20 | 1683.53 (in band) | 0.18855 .. 0.19635 | **FAIL** +3.4% |
> | slip 2000-2600 | **—** | 0 | — | 0.24488 .. 0.25487 | **UNSCORABLE** |
> | driving-median | **1355.66** | 54 | 1355.66 | 1904.70 .. 1982.44 | **FAIL** −30.2% |
>
> Whole-window median horizontal speed **26.33**, 1080/1080 grounded. `allowlisted=122 NEW=0`.
> Bounds unchanged and not renegotiable.
>
> **What was done.** The per-frame velocity budget on a free-flight frame was decomposed
> **completely rather than by enumerating physics**: A6a `0x00467650` is the only writer of
> `+0x9b0` there, and inside it there are exactly three velocity writes — W1
> (`Integrate2.cpp:630-632`) and grip-clamp #6's two arms (`0x004687f0..0x0046897b`) — with
> A6a's own `+0x9e4` store (`Integrate2.cpp:636`, original `0x004686cc`) sitting between them as
> an in-frame probe of the post-W1 pre-clamp speed. `+0x9e4` is a record field, so the budget
> closes with no unknown and needed **no new instrument and no source change on either side**.
>
> **FOUR routes now CLOSED (added to the six from attempts 7-9):**
> 1. **the drive / control force `+0xb14/+0xb18/+0xb1c`** — `T_drive = linTerm*(ctrl·u)` agrees
>    cross-side at matched speed, ratio **0.899..1.055 on six of seven comparable bands** (the
>    one outlier is 260-500 at 0.757). This is the matched-band measurement §20.10's withdrawn
>    reading did **not** cover, and it comes back agreeing.
> 2. **a hidden vertical control force** — `+0xb18` is exactly `0.0` on **2332 of 2332** original
>    frames; the port's `|ctrl.y|` median is **0.009** against `|ctrl.z|` **786534**.
> 3. **`linTerm`** — `+0x54` constant on 2332/2332 and 1628/1628, `kDt` exactly 1/3000, port
>    prints `linTerm=1.66667e-05`.
> 4. **the "gain between contacts" FRAMING as an independent lever** — the budget's third term is
>    grip-clamp #6, so the framing restates the same loop, exactly as §22.4's own
>    `[UNCERTAIN U-9156]` tag said. **The U-9156 row itself stays OPEN**: it is the trap, not the
>    framing.
>
> **Two registered safety thresholds FAILED on the port, and the failure's cause is the sharpest
> cross-side number in the whole re-open.** S1 **91.3%** against the registered >= 99%; S6
> **1324** detected contact frames against **212** actual fixups. Both come from one fact, which
> is detector-free and contamination-proof because the fixup touches at most 13% of port frames
> and 1.6% of original frames, so the MEDIAN is immune:
>
> > **`median(+0x9e4 / |+0x9b0..0x9b8|)` is `0.999998` on the ORIGINAL (n=1447) against
> > `1.172734` on the PORT (n=1628)** — the port loses a median **14.7%** of its linear speed
> > per frame to grip-clamp #6 where the original loses **0.0002%**. By band, 70-100:
> > **+0.003% against −15.782%**.
>
> That lands on the W2/W3 row, i.e. grip-clamp #6 — **branch 5 of the pre-registered rule, which
> is a refusal branch**, because §21.5 proved that code byte-faithful. So **no fix was authored**
> and the gap-aligned cross-side table is reported as **void**, not as a verdict.
>
> **Target invariant this lane produces** (derived, falsifiable, robust to <= 13% contamination
> by construction): `median(+0x9e4 / |+0x9b0..0x9b8|)` over race frames **== 1.000 to 1e-3**.
>
> **NEXT LANE, and it is not inside A6a, not inside `0x0046ef70`, and not the clamp.** Every term
> in the budget is now measured except the ORIGINAL's `T_rest` split. The original's `T_rest`
> reaches **−42.6/frame** right after the bounce and **decays 6x AT CONSTANT SPEED** as the car
> straightens (−34.5 at `s_from` 190.6 on the way down, −5.5 at 193.5 on the way up) while the
> port's is **−0.43/frame** in the same band. No closed route explains a drag that large or that
> decay, and `T_drive` is now excluded, so `T_rest` is the only remaining home for the
> difference. Get the ORIGINAL's `accum` (`l_b8/l_b4/lin_b0`) and its blend
> `frac = (l_d0 - m78)/l_d0` as a measurement: fit it from the per-wheel forces at record
> `wheelbase+0x70..0x78` on the PORT first, where `friction_diag.log` gives the true answer as a
> known-answer self-check, then apply the validated estimator to `orig_fp2.msd`.
>
> New tooling, both read-only and neither executing the game:
> `re/tools/statediff/a10_gain.py` (the budget) and `re/tools/statediff/a10_clampcost.py` (the
> detector-free clamp cost). Artefacts `verify/d2_gain_20261001/`.
> #### Re-close attempt 9 — 2026-09-30. **STILL REOPENED. The cadence lane is REFUTED by its own pre-registered rule, and two of attempt 8's claims are corrected — one withdrawn, one strengthened.**
>
> Full record: `re/analysis/D2_REOPEN_2026-09-29.md` **§22.3** (the rule + the correction,
> committed before any run) and **§22.4** (the result). Commits `cc5d376b`, `d76bb90f`.
> **No physics change** — the only non-comment source edit is a default-OFF probe restructure
> plus one local counter.
>
> **This attempt audited attempt 8 instead of extending it, and that was the right call twice.**
>
> **1. A blind spot in attempt 8's own instrument, found by re-reading the log it committed.**
> `MASHED_SUBSTEP_VELPROBE`'s emit sat after the world-contact block's
> `if (contacted != 0) continue;`, so it was skipped on exactly the substeps that contact. The
> signature was already in the committed data: **`c9ec == 0` on all 4000 logged substeps while
> `world_contact.log` from the same run held 211 fixups.** Corrected to one emitter called from
> both exits of the retry loop; it now sees **160 contact substeps** where the old one saw 0 of
> 4000, and the two paths cover 4000/4000.
>
> **That STRENGTHENED attempt 8 rather than weakening it.** §22.3 had to narrow §22.2's
> hypothesis-B refutation to non-contact substeps. Measured on the 160 contact substeps the
> corrected log exposes: `kFricVel` **0**, `kFricImp` **0**, drift **0**, and
> `vTop == vPostWheel` on **160 of 160**. So the claim now stands **unqualified** at 4000/4000
> including every contacting substep, and — with the original's bitwise 2945/2945, 2945/2945 and
> 2932/2932 — **both sides write `+0x9b0` in exactly two places per frame**, A6a `0x00467650`
> and `VehicleContactFixup` `0x0046ef70`.
>
> **2. §22.2's cadence claim is WITHDRAWN and U-9159 is REFUTED.** "19 frames on the original
> against 1-2 on the port" was an **inference** from fixup ordinals matched against per-frame
> speeds, never a frame index. Measured with the corrected probe:
>
> | inter-contact interval, the ten contacts after each side's own first bounce | values | n | median |
> |---|---|---:|---:|
> | ORIGINAL | 19, 13, 12, 12, 12, 13, 12, 12, 13 | 9 | **12** |
> | PORT | 21, 12, 12, 12, 10, 9, 8, 8, 8, 8 | 10 | **10** |
>
> `I_port / I_orig = 0.833`, clearing the pre-registered `0.5`, so §22.3's branch 1 fired:
> **the cadence is not the mechanism**, U-9159 is struck as resolved-by-refutation, and no fix
> was authored on it. The port's fixups do not double either — **1 per contacting frame on 160
> of 160**. The 13x whole-race ratio (23 of 2332 against 211 of 1625) is real but is the
> **consequence** of not escaping: the original stops contacting after its tenth (158-frame gap,
> next contact at speed 2290) while the port continues at a median interval of 7 (n=159).
>
> **3. U-9160 opened — a real fidelity defect, measured NOT to be on the trap's path.** The
> port runs **3** substeps per frame and **4** on a contacting frame, against the original's
> fixed **2**: `dt` takes exactly two values over 4000 substeps, **2720 at `25.000000` and 1280
> at `0.000004`**, and the residue is `3.8146973e-06` — exactly what
> `frameMs = (1.0f/60.0f)*3000.0f = 50.000004f` minus two 25s leaves at
> `VehiclePhysicsRun.cpp:891`. The original's count is the fixed `0x00469ad4 mov ebx,2`
> (`4662/2331` and `2932/1466` measured). **Inert here:** the residue pass contacts **0 of
> 1280** and writes velocity **0 of 1280**, and all 160 fixups land on `dt = 25` substeps.
> Banked with a clean target invariant rather than fixed opportunistically.
>
> **4. The sharpest matched statement of the residual, recorded and NOT offered as a lever.**
> Per contact gap, horizontal speed at the contact frame against the frame before the next:
> the **ORIGINAL gains on 7 of 9** (median `+17.84`, net `+338.74`, 219.89 -> 405.82) and the
> **PORT loses on 7 of 9** (median `-17.84`, net `-105.00`, 251.48 -> 42.13). **Gap 0 is
> like-for-like** — matched speed 219.89 against 251.48, matched free flight 19 frames against
> 21, full throttle both — and the original nets **`+11.63`** where the port nets **`-14.54`**.
> That is the `driving-median` failure restated in the post-bounce regime; its mechanism is the
> bleed §21.10 already measured out, so it names no term. `[UNCERTAIN U-9156]`
>
> **Scored 3 of 3 as a no-change control:**
>
> | metric | port | n | median speed | PASS interval | verdict |
> |---|---:|---:|---:|---|---|
> | slip 1500-2000 | **0.2033** | 20 | 1683.53 (in band) | 0.18855 .. 0.19635 | **FAIL** (+3.4%) |
> | slip 2000-2600 | **—** | 0 | — | 0.24488 .. 0.25487 | **UNSCORABLE** |
> | driving-median | **1355.66** | 54 | 1355.66 | 1904.70 .. 1982.44 | **FAIL** (-30.2%) |
>
> Identical to §21.6 and §22.2 to every printed digit. `allowlisted=122 NEW=0`.
>
> **CLOSED by measurement — six routes, do not re-open:** the fixup impulse (§22.2,
> 0.11-0.28%); the substep velocity chain on **both** contact and non-contact substeps (§22.2 +
> §22.4); `+0x9e4`'s write order (§22.2, `0x004686cc` precedes `0x004687f0`); grip-clamp #6
> (§21.5); the `l_60` / `ld4` lane (§21.10); the contact cadence (§22.4).
>
> **OPEN — the same loop from a third angle.** The original's post-bounce contact train
> accelerates and sits on the `0.9` damp cap for 15 of 23 fixups; the port's decelerates and is
> on the cap 0 of 211; and the cadence agrees. Every route into that from inside A6a and from
> inside `0x0046ef70` is now measured and faithful, so **the next attempt should not start in
> either.**


> #### Re-close attempt 8 — 2026-09-30. **STILL REOPENED. The first diverging term is NAMED and traced to an RVA; both code-level hypotheses it generated are REFUTED, so no fix was authored.**
>
> Full record: `re/analysis/D2_REOPEN_2026-09-29.md` **§22** (§22.1 is the rule, pre-registered
> and committed before any run; §22.2 is the result). Commits `74287459` (pre-registration),
> `a7311790` (tooling), `e0177628` (§22.2), `a3bd4ce6` (artefacts).
> **No physics change was authored** — this attempt was measurement plus default-OFF
> instrumentation. `MASHED_A6_FORCE_HIGHARM` was **not** added (user decision: no deliberately
> unfaithful knob).
>
> **FIRST DIVERGING TERM: `d = +0`, channel C4 — the post-bounce horizontal speed.**
> ORIGINAL `219.89064`, PORT `251.47744`, `|delta| 31.5868` against `tol 25.1477`, with every
> channel earlier in the registered scan order inside tolerance. **RVA: the last-contact damp
> at `0x0046f5ba` / `0x0046f5c0` inside `VehicleContactFixup` `0x0046ef70`.** Frames are aligned
> on each side's own first-bounce frame (`B_orig = 981`, `B_port = 81`, both inside the
> registered windows).
>
> **The decomposition, measured against the RUNNING original** (new entry-only
> `scenario_launch.py --fixup-probe`, which reads the live 18-slot contact set at the one point
> in the frame where it exists — the `.msd` cannot, its slots are `-1` on 2332 of 2333 frames):
> - **`0x0046ef70`'s impulse is FAITHFUL.** `local_54` = `(2417.77, -324.51, -141.34)` original
>   against `(2424.47, -325.4, -141.19)` port = **0.28% / 0.27% / 0.11%**. The slot producer is
>   faithful too: arm, scale, normal and magnitude agree to 4-5 digits.
> - **The whole divergence is the damp**, `0.2475405` against `0.2756187` = **+11.34%**, and the
>   damp **amplifies its `+0x9e4` input 11.1x**.
> - contact counts: **23 in 2332 frames** (original) against **211 in 1625** (port) — 13x.
>
> **Both hypotheses refused, by measurement, before any code was written:**
> - **`+0x9e4` is not `|velocity|`** on the port (median **1.017098**, n=211) where the original
>   holds it at **1.000000** (n=23 fixup entries, 0 samples off by >1e-3; also 1.0000022 at 1157
>   `0x0046f6c0` and 1157 `0x00469aa0` entries). **Circular, not independent:** a capstone sweep
>   of `0x00467650..0x00468990` (1243 instructions, reached `0x00468989`) finds the only two
>   `+0x9e4` stores at `0x00467673` and `0x004686cc`, and **`0x004686cc` precedes grip-clamp #6
>   at `0x004687f0`** — the original writes it before the clamp too, so the gap is the port
>   clamp's own excess bleed read out downstream.
> - **the port's substep writes velocity where the original's does not** — **REFUTED bitwise on
>   both sides.** The original's velocity is bitwise unchanged across `0x0046f6c0` on
>   **2945/2945**, `0x00469aa0` on **2945/2945** and the substep entry `0x004709a0` on
>   **2932/2932**; the port's three `WheelContactSolver` velocity-write sites fire **0 times in
>   4000 substeps** with the velocity bitwise unchanged **4000/4000**.
>
> **THE BASIN MECHANISM — this is what attempt 8 was sent to find, and it is a threshold.** The
> damp has a knee: `|m|/+0x9e4 <= 0.7` saturates it at the `0.9` cap (keep 90%), above the knee
> retention falls as `3*(1 - |m|/+0x9e4)`. `|m|` is the slot impulse along the wall normal, so
> the quantity is `|cos(velocity, wall normal)|`.
>
> > **The ORIGINAL is on the 0.9 cap on 15 of its 23 fixups (65.2%). The PORT is on it 0 of 211
> > (0.0%).** The original's `|cos|` falls monotonically across its 10-contact post-bounce train
> > (frames 978, 997, 1010, 1022, 1034, 1046, 1059, 1071, 1083, 1096: `0.918, 0.700, 0.596,
> > 0.557, 0.556, 0.542, 0.513, 0.465, 0.406, 0.328`), crosses under the knee at contact 1 and
> > escapes — a 158-frame gap to the next contact, at speed 2290. The port's **rises** (`0.908,
> > 0.766, 0.788, 0.781, 0.788, 0.809, 0.815, 0.815, 0.817`) and locks on a **fixed point**:
> > `|cos| ~ 0.827`, damp `~0.55`, `pre_h ~ 55`, `post_h/pre_h ~ 1.11`. **`pre_h ~ 55` is
> > exactly §21.5's 40-70 residency band** (238 of 1352 frames).
>
> **Scored 3 of 3 as a NO-CHANGE control on the instrumentation added, HEAD:**
>
> | metric | port | n | median speed | PASS interval | verdict |
> |---|---:|---:|---:|---|---|
> | slip 1500-2000 | **0.2033** | 20 | 1683.53 (in band) | 0.18855 .. 0.19635 | **FAIL** (+3.4% past the bound) |
> | slip 2000-2600 | **—** | 0 | — | 0.24488 .. 0.25487 | **UNSCORABLE** |
> | driving-median | **1355.66** | 54 | 1355.66 | 1904.70 .. 1982.44 | **FAIL** (-30.2%) |
>
> Identical to §21.6 to every printed digit on 3 of 3 runs; whole-window median horizontal
> speed **26.36**, 1080/1080 grounded. Build gate met: `allowlisted=122 NEW=0`.
>
> **A near-miss is on the record rather than hidden:** pairing the original's post-fixup
> velocity with the next substep entry produced a false "71% tangential-impulse deficit". A6a
> `0x00467650` runs once per frame before the substep loop and writes `+0x9b0`, so that pair
> crosses a frame boundary. Adding A6a as probe site 2 flags those pairs and the impulse then
> agrees to 0.11-0.28%.
>
> **NEXT, and it is NOT inside A6a.** Both sides leave the first bounce with nearly the same
> velocity direction (`0.7739` original, `0.7475` port — the port's is the *more* tangential)
> and nearly the same nose. What differs is the **time between contacts**: 19 frames on the
> original against 1-2 on the port, so the original gets an order of magnitude more free flight
> to rotate its velocity before the wall is asked again. **That is a contact-CADENCE question**,
> and it is the first framing in this re-open that is neither inside A6a nor slip-coupled. The
> per-contact depth series rules out penetration depth (both graze: original `-0.02056,
> -0.00038, -0.00123, -0.00011`; port `-0.0159, -0.0040, -0.0023, -0.0006`), so the question is
> how often the hull returns to the plane: `VehicleContactHistoryUpdate` `0x00470ae8` /
> `ContactHistoryLookup` `0x00468b40` and the 32-slot history at `veh+0xbfc`. §19 tested "the
> original re-latches too" and that stands; it did **not** test "how often it re-penetrates".
>
> **Do NOT re-open:** the fixup impulse (faithful to 0.11-0.28%), the substep velocity chain
> (bitwise faithful), the `+0x9e4` write order (`0x004686cc` before `0x004687f0` on the original
> too), grip-clamp #6 (§21.5, byte-faithful), or the `l_60` / `ld4` lane (§21.10, measured out).


> #### Re-close attempt 7 — 2026-09-30. **STILL REOPENED. The registered test fired, three readings are withdrawn, grip-clamp #6 is proven byte-faithful, and the defect is its INPUT `l_60`.**
>
> Full record: `re/analysis/D2_REOPEN_2026-09-29.md` §21. Commits `914465e6` (pre-registered
> rule), `1239c0be`, `4be7af5e`, `86ee9efa`, `d2ad9ec0`, `340fbf2f`, `ff90586b`.
> **No physics change was authored** — this attempt was measurement and a comment correction.
>
> **Scored, 3 of 3 port runs bit-identical (§3b satisfied), HEAD:**
>
> | metric | port | n | median speed | PASS interval | verdict |
> |---|---:|---:|---:|---|---|
> | slip 1500-2000 | **0.2033** | 20 | 1683.53 (in band) | 0.18855 .. 0.19635 | **FAIL** (+3.4% past the bound) |
> | slip 2000-2600 | **—** | 0 | — | 0.24488 .. 0.25487 | **UNSCORABLE** |
> | driving-median | **1355.66** | 54 | 1355.66 | 1904.70 .. 1982.44 | **FAIL** (-30.2%) |
>
> Median horizontal speed over the 1080-frame window **26.36**, 1080/1080 grounded — the car
> does not drive. Build gate met: `allowlisted=122 NEW=0`.
>
> **Withdrawn (three readings, one of them from this session's own §21.4):**
> - §20.15's "an over-strong bleed is refused; the lateral is never GENERATED" — the
>   registered test gives `R = |dlat| port/orig = 1.3387` with the yaw rate 2.8% apart, so
>   §21.1's branch 3 fired and no fix was authored on either candidate.
> - §21.4's "the original is on the HIGH arm" — the `|av|` fingerprint is confounded by A6a's
>   own angular-velocity integration, which runs before the clamp (`|av|` ratio 1.0103 at
>   800-2000: av GREW, which a multiply by `1-k` cannot do).
> - §20.15's `lat(n+1)/lat(n)` median of 1.0590 — it averaged a two-phase structure (the
>   port's lateral collapses in 6 frames after each bounce, then regrows) and the two cancel.
>
> **Established, all by measurement:** the lateral is removed **inside A6a** and nowhere else
> below 150 speed (`I_a6b` and `I_s1` are exactly `0.0000`, 0 pos / 0 neg, up to n=606);
> **grip-clamp #6 is byte-faithful** (`0x004687f0..0x0046897b` vs `Integrate2.cpp:713-736`,
> both arms, six constants, two floors, the `0.1` bound confirmed a FLOOR); `+0x18c` is `1.0`
> on both sides so `grip == l_60`; and the port's `grip*speed` at 100-150 is **17 992**
> against the **>= 29 491** that puts `k` on its floor and reproduces the original's measured
> `I_a6a / L = 0.1019` (n=46). At 40-70 the port's `k` is **0.9084**.
>
> **Why no fix:** `l_60 = sum ld4 * le4` and `ld4` is the sine of a wheel slip angle — the
> quantity being explained — so raising it would fit the cause to the symptom. `le4` is
> measured to agree in form on both sides.
>
> **New hard fact that may reframe the lane:** a 6658-frame original control returns the same
> `n = 2` at 40-70 and `n = 6` at 70-100 as a 2335-frame one. **The original passes below 100
> horizontal once per race, for 6-8 frames; the port spends 238 of 1352 frames at 40-70
> alone.** Residency in that band is itself the defect, so a low-speed-bleed-only fix may not
> move the scored table.
>
> **Then, same session (§21.7-§21.10): the ORIGINAL's `l_60` WAS measured, and the lane is now
> MEASURED OUT.** A6a's body has **zero `fsqrt`** and calls RwV3dLength `0x004c3ac0` nine
> times; that function takes its vector **by pointer**, so an **entry** hook reads the exact
> argument. `scenario_launch.py --mag-probe` (count-first: the function has 120 call sites
> image-wide) + `re/tools/statediff/a8_l60.py`. Gate 1 passed with a known-answer self-check —
> site `004686a9`'s vector equals the record's own `+0x9b0`/`+0x9b8` on **1424/1424** rows.
>
> | band 100-150 | ORIGINAL (n=35, med speed 132.73) | PORT (n=30) | ratio |
> |---|---:|---:|---:|
> | `grip*speed` | **33 157.4** | 17 992 | **1.84x** |
> | `l_60` | 285.243 | 141.24 | 2.02x |
> | `ld4` | 0.81376 | 0.3005 | **2.71x** |
> | `le4` | 119.972 | 100.28 | 1.20x |
> | above the 32768 knee | **18/35** | 8/30 | — |
>
> The deficit is real and the original sits ON the knee — but every factor feeding it now has a
> cross-side measurement and each one either **agrees** (`le4` 1.20x; `+0x9e8`/`f` **1.09x at
> 150-250** while `ld4` there still differs 1.93x; the wheel-axis write present on 100% of
> frames, `Integrate2.cpp:692`'s failure mode 0/1334 and 0/1628) or is **slip-coupled** (the
> wheel-point velocity direction, and `+0x9e8` at 100-150 whose **6.75x** is §20.15's slip
> ratio **6.73x** — the same measurement in another channel).
>
> **One real independent defect, quantified as insufficient before anything was built on it:**
> the ORIGINAL's front-axis deflection is **`-33.867` deg in every band, exactly its own steer
> angle and speed-INDEPENDENT**; the PORT's is **10.0% / 15.4% / 20.8% short and
> speed-DEPENDENT** (100-150 / 150-250 / 1500-2000), with the rear pairs agreeing on both
> sides. Closing it moves `ld4` `0.30050 -> 0.35632` = **10.9%** of the gap, so it cannot close
> D2 and no fix was authored. Target invariant if it is fixed later: front deflection EQUALS
> the steer angle, exactly, at every speed. **Its writer is not located** and needs Ghidra
> xrefs (memory `offset-grep-misses-dword-index`).
>
> **Verdict:** the causality cannot be broken from inside the loop — it is self-consistent both
> ways and the two sides are in **different basins**. That promotes §21.5's residency finding
> to the primary hypothesis: the original passes below 100 horizontal **once per race, 6-8
> frames of 6658**, the port spends **238 of 1352** at 40-70 alone.
>
> **Next, in order, neither a fix:** (1) a default-OFF `MASHED_A6_FORCE_HIGHARM` diagnostic to
> test bistability (deliberately unfaithful, do not ship); (2) find what puts the port in the
> low-speed basin, which §20.14 localises to the ~5 frames after the first bounce since both
> first bounces already agree. Kickoff: `re/NEXT_SESSION.md`.
>
> **Note on numbering:** there is no attempt-6 block below. Attempt 6 (the `+0x9c8` body-up-axis
> decode and fix, commit `5ec297fa`) is recorded only in `D2_REOPEN_2026-09-29.md` §19-§20; it
> left the scored table at `0.2033 / — / 1355.66`, which is where attempt 7 found it.

> #### Re-close attempt 5 — 2026-09-30. **STILL REOPENED. The arm was commanding a different manoeuvre, the 4x bounce was a transcription error, and the residual is now two `status stub` functions.**
>
> Full record: `re/analysis/D2_REOPEN_2026-09-29.md` §16-§18. Commits `30cca402`, `d9e8fd24`,
> `c231b116`, `21563b39`. **The §3 bounds were not touched.**
>
> **The arm is corrected again (third mismatch of the same class, U-9157).** Every D2 solo run
> is now
>
> ```
> py -3.12 re/tools/statediff/a8_run_port.py <dir> 90 MASHED_MEASURE_SOLO=1 \
>         MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0 "MASHED_TITLE=<label>"
> ```
>
> Why: the port held ZERO steer at full accel for **60 sim steps** (`a8_run_port.py:16`
> `MASHED_STEER_HOLD_AFTER=4` -> `exe_main.cpp:3011-3012`/`:3024`; first yaw change at sim step
> 61, yaw bit-identical for 60 steps) while the original arms accel and steer in ONE
> `E.drive(1,+1)` call (`scenario_launch.py:2180-2182`) and turns from its first moving frame at
> speed 1.8 (`orig_solo3.msd` frame 897). With that corrected the two cars arrive at the **same
> wall**: original frame 980 `pos (-1.95345,-3.62606)` `vel.x -1716.69` `speed 1815.4` vs port
> `f=80 pos (-1.98725,-3.62189)` `vel.x -1720.85` `speed 1839.7` — **0.034 world units and
> 0.24% apart**.
>
> | metric | ORIGINAL mean | attempt 4 | **attempt 5 (corrected arm)** | n | PASS interval | verdict |
> |---|---:|---:|---:|---:|---|---|
> | slip 1500-2000 | 0.19245 | 0.1605 | **0.2031** | 20 | 0.18855 .. 0.19635 | **FAIL** (+3.4% past the upper bound) |
> | slip 2000-2600 | 0.249875 | 0.2497 | **no samples** | 0 | 0.24488 .. 0.25487 | **UNSCORABLE** |
> | driving-median | 1943.57 | 1938.68 | **1355.64** | 54 | 1904.70 .. 1982.44 | **FAIL** (-30.2%) |
>
> 3 of 3 runs bit-identical, so §3b holds. **Median speed over all 1080 frames is 24.5 — the car
> does not drive**, so no figure above may be read as a pass.
>
> **U-9156 root-caused and half fixed.** `0x0046ef70`'s last-contact damp read the wrong slot
> against the wrong sentinel: the original saves SLOT 0's base once before the loop
> (`0x0046f013`/`0x0046f016`), reloads it at `0x0046f522`, reads `[rec+0x4ac]` = slot 0's KEY at
> `0x0046f5ae`, and `0x0046f5b1 cmp ecx,-2` / `0x0046f5b4 je` skips the damp ONLY on `-2`. The
> port tested SLOT 17's key against `-1`, so the damp never ran. Pre-registered prediction MET:
> the first fixup's x output `+698.16` -> **`+194.39`** (predicted `+186 ± 10`) against the
> original's `+176.47`.
>
> **The residual.** *(First attribution WITHDRAWN the same day — see §19.)* It was recorded here
> as "`0x00468d80` and `0x004694e0` are both `status stub`". **They are not stubs**: both have
> full transcriptions (`Collision/CarWorldContacts.cpp:160-253` and `:262-377`) and `STUBS.md`
> S-3440/S-3441 are struck through as resolved; the status column was stale and is now `impl`.
> A follow-up re-latch hypothesis (nothing writes the contact history at `veh+0xbfc`, so every
> contact is "new" every substep) was **refuted by measurement** — the original's 32 keys and 32
> active flags are all zero on 22 `--peek` samples over two 40 s live races, so it re-latches
> too.
>
> What is measured and stands: the scan reports slot 5 down to `d=-0.0000` **216 times in 1628
> sim steps**, the car is damped back to ~10 every time it reaches ~55, and the original slides
> ALONG the same wall at `vel.z +119..+320` and leaves by frame ~1150. The damp
> (`min(0.9, 3*(1 - min(1, abs(m)/speed)))`, all three components) with
> `abs(m)/speed = abs(cos(velocity, wall normal))` makes this a positive-feedback trap: head-on
> annihilates the velocity, and the port cannot rotate tangential because its yaw rate scales
> with speed. **The condition for attempt 6 is to find the first diverging term inside that
> loop** — the registered command is a cross-side per-frame dump of `+0x9b0` / `+0x144` /
> `+0x9e4` / `+0x9ec` over frames 980-1100.
>
> Guards on the final build: criterion (e) **PASS 3/3**; AI (b) **FAIL 3/3**, `c1_median`
> 52.5 / 38.0 / 49.0 (identical to attempts 1-4); power-ups **decision CLEAN 11/11**, `g3`
> contact diverges; oracle rule 3 **GREEN** MISMATCH=0; rva-lint **`allowlisted=122 NEW=0`**;
> Arctic non-regression **2044.85 (n=34)** vs 2060.40 (n=42), no collapse, 0 `RecoverOffMesh`
> fires.

> #### Re-close attempt 4 — 2026-09-29 (fifth session that day). **STILL REOPENED. The wall now holds, and the residual is one named behaviour.**
>
> Full record: `re/analysis/D2_REOPEN_2026-09-29.md` §15. Commits `35f418dc`, `b6a5cd1c`,
> `7908af78`. **The §3 bounds were not touched.**
>
> | metric | ORIGINAL mean | attempt 3 | **attempt 4** | PASS interval | verdict |
> |---|---:|---:|---:|---|---|
> | slip 1500-2000 | 0.19245 | 0.1605 | **0.1605** (n=13) | 0.18855 .. 0.19635 | **FAIL** (-16.6%) |
> | slip 2000-2600 | 0.249875 | 0.3086 | **0.2497** (n=28) | 0.24488 .. 0.25487 | inside |
> | driving-median | 1943.57 | 2437.93 | **1938.68** (n=66) | 1904.70 .. 1982.44 | inside |
>
> 4 of 4 runs bit-identical on every column (§3b satisfied). **D2 does NOT re-close** — §3c
> requires all three — and **the two that land inside are NOT a pass**: they are medians over
> 28 and 66 rows, for the reason below.
>
> **U-9154 CLOSED.** The car-vs-world contact chain runs. `Collision/ContactFixup.cpp` is a new
> verbatim port of `FUN_0046ef70` (and its pushed matrix argument is DEAD — balanced-ESP walk,
> no read of `[esp+0x90]` in 485 instructions); `Rw_VtableDispatch` is bound to the measured RW
> device slot `+0xc` (`call [ecx+eax+0xc]` at `0x004c3db0`) = `RwV3dTransformPointsCPU`; the
> substep runs `0x00470ae8` -> `0x00470aef` -> `0x00470afe` with the `0x00470ab0`/`0x00470b0a`
> retry; and `SyncContactRingMatrix` publishes `g_bodyBasis` into the `rec+0x928` ring, whose
> rotation rows were zero. `0x0046e9e0` was deliberately NOT re-ported — both halves already
> have bodies, so a third copy is a new dual body; §14.6's "three unported functions" was wrong
> on that one.
>
> **The wall holds on the right plane.** Reporting slots are 5 and 9, the hull corners at
> `x = box[3]`, normal `(1.000,0.000,0.000)`, depth `-0.037` — Training's `x = -2.500` plane,
> the one the original bounces off at frame 980.
>
> **U-9155 filed and half-resolved.** The 14 non-wheel contact points at `rec+0x90..+0x137` were
> ZERO; the first build of this session therefore fabricated 14 coincident contacts at the body
> centre, which improved Training and **collapsed Arctic** (median speed 1740 -> 102). Their
> producer is `FUN_0046b1c0`, called at `0x0040ed62` immediately before A3, fed by `FUN_0041f000`
> from `DAT_0063dc10 + car*0x2ac` — unreadable from the file, so the box was MEASURED live
> (`--peek`, 7 identical samples, cars 0/1/2/3 equal). The writer of `DAT_0063dc10` is still
> unidentified, so the port seeds one box for every slot.
>
> **A dual-copy hazard the guard did not catch.** `0x0046b1c0` already had a C3 Frida-GREEN
> naked-x87 port the exe cannot call (absolute `0x008815a0` base, `ds:` literals).
> `scripts/lint_rva_bodies.py` anchored NEITHER body and reported `NEW=0` with both in the exe.
> Found by hand during the tracker pass, split by target, registered CROSS-TARGET with the
> reason (allowlisted 110 -> 111, NEW=0). **Check for an existing port before writing one.**
>
> **`RecoverOffMesh` is KEPT**, now with direct evidence rather than an argument: Arctic fires
> **46 -> 0**, Training 0 -> 0. That is 2 tracks of 12, which is not unreachability, and U-9156
> shows the car can still end up pinned. Re-pickup: 0 fires across all 12 tracks.
>
> **PICK UP HERE — U-9156.** With the wall solid the port's car gets **TRAPPED** against it: all
> 73 fixups at `x = -2.02 .. -2.06`, median speed over 1080 frames **93**. The original touches
> that plane once in 2332 frames and recovers after 18 pinned frames. The difference is the
> approach — the port arrives at **2283** where the original arrives at **1717**. Next command:
> plot both loops on Training's `COLLISIONS.BSP` with `re/tools/statediff/loop_plot.py` and
> compare radius and centre. Larger port loop = U-9147's residue, upstream of contacts; matching
> loops = instrument `0x0046ef70`'s per-slot terms against the original at frame 980.
>
> **Guards on the final build** (reported, not scored; nothing tuned): criterion (e) **PASS 3/3**;
> AI (b) **FAIL 3/3**, `c1_median` 52.5 / 38.0 / 49.0, identical to attempts 1-3; power-ups
> **11/11 decision CLEAN**, contact CLEAN 10/11 with the known `g3` divergence; modes oracle
> rule 3 **GREEN** (FinishOrder 2717/2717, MISMATCH=0); build both targets clean,
> `rva-lint allowlisted=111 NEW=0`.

> #### Re-close attempt 3 — 2026-09-29 (fourth session that day). **STILL REOPENED. The arm itself was cross-track, and the residual is now a named unported function.**
>
> Write-up: [`re/analysis/D2_REOPEN_2026-09-29.md`](re/analysis/D2_REOPEN_2026-09-29.md)
> §13-§14. Commits `36600fe5` (the registration + the loop plot), `639ae31c` (the corrected
> arm + the root cause). **The §3 bounds are untouched.**
>
> **The two arms were racing DIFFERENT TRACKS.** The ORIGINAL arm raced **Training**; the
> PORT arm raced **Arctic**. Four witnesses: `scenario_launch.py:1773-1777` defaults
> `--track` to **0 = Training** and none of the four reference captures passes `--track`;
> `a8_run_port.py:16` fixes `MASHED_TRACK_SEL=0` and `GameFlow.cpp:38` `kAreas[0]` is
> **Arctic**; `mashed_re.log` under the port argv reads `R4 track load OK:
> original/TOASTART/TRACKS/Arctic.piz`; and the spawn points are 33 world units apart —
> original `(+0.5009, +0.4374, -1.9990)` (= Training's `gate0`) vs port
> `(-26.5640, +0.0375, +17.0107)`. **Same class of defect as the three-cars-vs-one that
> reopened D2.** `exe_main.cpp:2071-2079` already carried the lesson in a comment; the a8
> recipe was never moved onto the `kAreas`-wide override. Filed and closed as **U-9153**;
> the corrected port arm is `MASHED_TRACK_SEL=12` (§13.3), registered **before** measuring.
>
> **This answers §12.4's open trajectory-vs-mesh question as NEITHER.** Both loops plotted
> on their track's `COLLISIONS.BSP` soup (new read-only `re/tools/statediff/loop_plot.py`,
> plot `verify/d2_offmesh_20260929/loops_crosstrack.png`): the ORIGINAL loop is 212/212
> on-mesh on Training and 151/212 on Arctic; the PORT loop is 203/203 on-mesh on Arctic and
> 143/203 on Training. No area exists where the original drives and the port's mesh is
> missing, and the port's trajectory cannot be called wrong from fires the reference never
> reached. **§12.4's "bearings cover the whole circle, not a hole in `col_tris_`" is
> withdrawn as a reading** — the 46 fires lie on three straight axis-aligned road edges
> (`x = -24.25`, `z = +35.95`, `x = -0.40`), which a bearing histogram about their centroid
> cannot see.
>
> **The corrected arm, 3 of 3 runs bit-identical (§3b satisfied; the U-9148 second
> attractor did not appear):**
>
> | metric | PASS interval | attempt 2 (Arctic) | **attempt 3 (Training)** | verdict |
> |---|---|---:|---:|---|
> | slip 1500-2000 | 0.18855 .. 0.19635 | 0.1445 | **0.1605** | **FAIL** (-16.6%; **n=13**, fragile) |
> | slip 2000-2600 | 0.24488 .. 0.25487 | 0.2557 | **0.3086** | **FAIL** (+23.5%) |
> | driving-median | 1904.70 .. 1982.44 | 1740.54 | **2437.93** | **FAIL** (+25.4%) |
>
> Two of the three moved further out. Reported as-is: correcting the arm was not a tuning
> step and its job was to make the comparison legitimate, not to improve a number.
> `RecoverOffMesh` fires **46 -> 0** on the corrected arm, so §12's whole mechanism (35 slip
> resets per 1080 frames) was a consequence of the wrong track and is not a factor.
>
> **The residual is NOT a 25%-too-fast car.** Driving-frame speed quantiles agree at the top
> and diverge only at the bottom: p95 **0.99x**, p99 **0.97x**, max **0.97x**, but p25
> **1.65x** and p10 **2.47x**. The original's low tail is **wall impacts** (frame 980->981:
> `vel.x` `-1716.69` -> `+176.47`, `pos.x` pinned 18 frames, speed 1832 -> 191). Training's
> `COLLISIONS.BSP` has the wall — 41 of the 69 triangles in that box are XZ-degenerate,
> including planes at `x = -3.000` and `x = -2.500` — and `HeightOnSoup`
> (`TrackRenderer.cpp:2098`) discards exactly those, so the port drives through both (its
> loop reaches `x = -4.721`).
>
> **ROOT CAUSE, with RVAs — U-9154.** `VehicleContactScanUpdate` `0x00469aa0` is ported
> (`Collision/CarWorldContacts.cpp:387`) and **has no caller**. Its only call site in the
> image is `0x00470ae8` (new `re/tools/callsites.py`, which confirms each byte-pattern hit
> by disassembling it; control on A6b `0x00468980` returns its one known site `0x00470943`),
> inside **`VehicleCollisionBroadPhase` `0x004709a0`** (C2, `mapped`): integrate
> `0x0046e9e0` -> wheel contacts `0x0046f6c0` -> `0x00469aa0` -> fixup `0x0046ef70`, with the
> whole substep **re-run** while a contact is reported (`0x00470ab0` / `0x00470b0a`).
> Blockers named rather than guessed: `Rw_VtableDispatch` is a no-op stub
> (`ContactStubs.cpp:78`), A9 `0x0046e9e0` is unported so nothing reads `+0x4c0`, and the
> retry needs `0x004709a0`.
>
> **`RecoverOffMesh` is KEPT, and the reason is now evidence, not preference.** The original
> has **no off-world respawn** on this path — the car cannot leave the surface because the
> contact chain reports, fixes and re-runs. So that chain IS the scaffold's faithful
> replacement, and removing the scaffold first would only restore the
> freeze-against-an-edge loop (`TrackRenderer.cpp:2806-2817`). It is off the D2 measurement
> path (0 fires, 3 of 3 runs).
>
> **Guards on the final build, nothing tuned:** criterion (e) **PASS 3/3**; AI (b) **FAIL
> 3/3**, `c1_median` 52.5 / 38.0 / 49.0 — **identical** to attempts 1 and 2; power-ups
> **11/11 decision CLEAN**, contact CLEAN 10/11 with the known `g3` divergence; modes oracle
> rule 3 **GREEN** (SegmentCheck 3059/3059, EvaluateResult 2/2, FinishOrder 3963/3963,
> MISMATCH=0); build clean with the dual-copy guard at **`allowlisted=110 NEW=0`**.
>
> The D3 modes 3/7 hold is unchanged.

> #### Re-close attempt 2 — 2026-09-29 (third session that day). **STILL REOPENED, but U-9147 is now LOCALIZED and U-9151 is CLOSED.**
>
> Write-up: [`re/analysis/D2_REOPEN_2026-09-29.md`](re/analysis/D2_REOPEN_2026-09-29.md)
> §9-§11. Commits `73784dc7` (localization), `bb074e62` (fix), `4004a5c8` (U-9151).
> **The §3 bounds are untouched.**
>
> | metric | PASS interval | attempt 1 | **attempt 2** | verdict |
> |---|---|---:|---:|---|
> | slip 1500-2000 | 0.18855 .. 0.19635 | 0.1332 | **0.1445** | **FAIL** (-24.9%, was -30.8%) |
> | slip 2000-2600 | 0.24488 .. 0.25487 | 0.2179 | **0.2557** | **FAIL** (+2.3% — **0.00083 over the upper bound**) |
> | driving-median | 1904.70 .. 1982.44 | 1818.42 | **1740.54** | **FAIL** (-10.4%, was -6.4% — moved the WRONG way) |
>
> §3b satisfied: 4 of 5 runs identical on every column; run 3 hit the known U-9148 second
> attractor (`0.1557 / 0.2493 / 1554.50`) and is reported, not dropped.
>
> **U-9147's first divergent quantity, found by instrumentation rather than by fitting.**
> `d(bodyH)/frame` is a single *speed-independent* constant on each side:
> **-0.04249 (port) vs -0.04462 (original)**, identical across both scored bands to five
> decimals, ratio **0.9523 = 100/105**. `_DAT_00613108` is **105.0** in the running
> original (nine live `--peek` samples over 30 s); the port hardcoded `100.0f` at
> `BodyOrientationIntegrate.cpp:218` and `VehicleInit` discarded the handling-table result.
> The table was harvested live: tag 0 -> 100, **tag 6 -> 105**, tag 12 -> 95, tag 18 -> 100,
> and the original's selector resolves to 6 on the reference scenario. Fixed by publishing
> `g_handlingTorque`; `MASHED_HANDLING_TYPE=0` reverts. **The pre-registered §9.6
> prediction held exactly**: `d(bodyH)/frame` -> -0.04462 and the axis-minus-forward offset
> +0.0425 -> **+0.0446**, both now equal to the original's.
>
> **This withdraws §6.2's handling-globals elimination.** `[0x00613140]` is 0, but the A3
> walk chains through `e[3]` and matches tag 6. The elimination also argued on magnitude
> ("cannot produce 30%"); the quantity it had to produce is 4.77%, and it produces it.
>
> **A6a is CLEARED by measurement, not by argument.** New `MASHED_A6ADUMP` logs A6a's own
> `lac/la8/la4`, `f5`, `le`, `lbc`, `dF`, `l60`, `grip`, `k`, and
> `re/tools/statediff/a6a_replay.py` validates its transcription against those values
> before touching the original (self-check 1 worst relative error **7.8e-07**; `f5 <= lbc`
> 0 violations in 4300 samples; the clamp-6 transcription exact). Then: recovered
> `g_suspScale` agrees **0.993 / 0.992 / 1.019 / 1.017** per wheel — and the original's real
> value was measured at **692.3021850585938**, the port's to the last digit; block-#5 yaw
> torque agrees **1.1-1.7%**; clamp #6's applied `k_vel` agrees to ±3% with *opposite* signs
> in the two bands; wheel geometry to 0.002 rad. The old `a8_wheelfit.py` cross-side fit
> stays withdrawn.
>
> **U-9151 CLOSED.** Its blocker was mis-stated: `RwMatrixMultiply 0x004c4600` is a
> dispatcher and does no arithmetic. The multiply it calls was measured live
> (`--peek i007d4028+007d3ff8+8:u` = **`0x005cb2a0`**; `+4` = `0x00020000`, matching the
> `and eax, 0x20000` at `0x004c4622`) and ported as a naked verbatim x87 transcription.
> Two GREEN diffs against the live original: `rw_matrix_multiply_cpu` 12/12 and
> `rw_matrix_rotate_inner_cpu` (the exe's exact branch, modes 0/1/2) 10/10, both 0
> mismatches. `VehicleControl.cpp` now passes `xform`: **7 runs of 7 exit 0** where 3 of 3
> previously exited `0xC0000005`, with **25 natural samples**, `xfok=1` on all 25.
> It does **not** promote the exe copy of `0x00468980` (the witness covers the binding, not
> bit-identity of the body) and all 25 samples are `state=0` at spawn. D2 unmoved by it,
> exactly as §4.7 predicted.
>
> **Guards, nothing tuned:** criterion (e) **PASS 3/3**; AI (b) **FAIL 3/3**, `c1_median`
> 52.5 / 38.0 / 49.0 (same class as 49 / 45 / 52.5); power-ups **11/11 decision CLEAN**,
> contact CLEAN 10/11 with the known `g3` divergence; modes oracle rule 3 **GREEN**,
> MISMATCH=0; build clean with the dual-copy guard at **NEW=0**.
>
> **Still blocking re-closure: U-9147's residual.** With the body half now exact, the whole
> gap is in the velocity heading — the port's turns **6.4% / 4.0%** slower than the
> original's at the same body rate, and `|d(velH)| < |d(bodyH)|` on *both* sides, so the
> scored window is a spin-up transient, not a steady state. The next question is the
> transient (how far each side is into it when the band is scored: port n=227 vs original
> n=312 in the low band), not the tire law. `RecoverOffMesh` and U-9152 are still open and
> still unmeasured against this. The D3 modes 3/7 hold is unchanged.

> #### Re-close attempt 1 — 2026-09-29 (later the same day). **STILL REOPENED.**
>
> Full write-up: [`re/analysis/D2_REOPEN_2026-09-29.md`](re/analysis/D2_REOPEN_2026-09-29.md).
>
> **The bounds are now pre-registered** (that note §3, committed **before** any fix, in
> `d81a8df6`). Two original solo captures existed; a run-to-run spread needs three, so two
> more were taken (`verify/d2_reopen_20260929/orig_solo{3,4}.msd`). Four originals give
> mean / half-range `h`: `0.19245 / 0.0013`, `0.249875 / 0.00015`, `1943.57 / 10.005`.
> Runs 3 and 4 are bit-identical on every column, so the original is deterministic for a
> fixed argv. Rule: per metric `|port - mean| <= max(3h, 2% of mean)`, i.e.
>
> | metric | PASS interval | port (3/3 runs identical) | verdict |
> |---|---|---:|---|
> | slip 1500-2000 | 0.18855 .. 0.19635 | **0.1332** | **FAIL** (-30.8%) |
> | slip 2000-2600 | 0.24488 .. 0.25487 | **0.2179** | **FAIL** (-12.8%) |
> | driving-median | 1904.70 .. 1982.44 | **1818.42** | **FAIL** (-6.4%) |
>
> **U-9149 is DECODED and the `.asi` half is FIXED** (`e8ebefa3`). `[esp+0x3c]` at
> `0x0047093b` is `E+0x0c`, A4's `param_3` **slot**, reused as a local at `0x004706a2` to
> hold `record + [record+0x9a8]*0x40 + 0x928` — so A6b's ESI is the same pointer A5 gets as
> arg 2 and A6a as arg 3, and it is an `RwMatrix` (it goes straight to `RwMatrixRotate`).
> Both prior readings in the tree were wrong. Verified live on the anchored original over
> 144 self-test samples (64 airborne, `ndiff=0`, `xfok=1` on every one; the two dispatch
> arms are byte-identical). U-9149's "there is a `sub esp` unaccounted for" is closed —
> the frame balances exactly.
>
> **The exe half is BLOCKED, newly as U-9151.** Binding the matrix at
> `VehicleControl.cpp:195` crashes `mashed_re.exe` with `0xC0000005`, 3 runs of 3: A6b's
> only effect is two `RwMatrixRotate(..., mode 1)` calls, and mode 1 resolves through the RW
> **device** table at `0x007d4028` / `0x007d3ff8`, unmapped in the standalone
> (`Math/RwMatrixRotateInner.cpp:169-176`). Unblocker: a CPU port of
> `RwMatrixMultiply 0x004c4600`, which does not exist.
>
> **A6b is NOT the cause of U-9147, and that is now settled rather than pending.** A6b
> returns unless `+0x9e0 == 0`; the metric scores only `+0x9e0 >= 3.5`
> (`re/tools/statediff/a8_slip_axis.py:35`). Disjoint. This retires
> `PLAYER_REGRESSION_2026-09-29.md` §7.3.6's "the only one of the five still capable of
> explaining it" — all five dual-copy leads are now eliminated.
>
> **U-9147 is NOT localized. Three attempts today, all withdrawn.** The cross-side
> lateral-coefficient comparison from `a8_wheelfit.py` is unsound in both modes: its
> `port_frames` builds the port's `lat` from a 2-D velocity heading and ignores the `wld4`
> the port logs, so the two sides' fit bases differ. The arithmetic tell: with `p[-1] == 0`
> (measured on all four wheels on both sides) A6a's lateral scale `f5` satisfies
> `f5 <= lbc` always (`Integrate2.cpp:441-450`), so the reported `a/lbc = 1.840` was a
> broken fit, not a finding.
>
> **What is sound:** every input to the coefficient matches (read directly — `p[0x15]` 0.15,
> `p[0x16]` 0.0125, `p[0x1b]` 1091.8/1083.8/1084.6/536.3, `p[-1]` 0 on all four,
> `g_suspScale` 692.3 vs ~710, `le4` 1024 both sides); and A6a's own `|lat|`, which the port
> logs and the corrected original side recomputes, agrees **1.001 / 1.003 / 0.948 / 0.960**.
> On everything currently comparable the two sides are within 5%.
>
> **Next step is instrumentation, not a fix:** log A6a's real `lac/la8/la4` and `f5`
> (`Integrate2.cpp:448`/`:442`) on the port side and make `port_frames` read them. Until
> then no lateral-coefficient ratio from this tool should be quoted. The `--lat-mode
> wheelpoint` correction to the ORIGINAL side is right and stays.
>
> **Guards after the change** (reported, not gates — the shipping exe is behaviourally
> unchanged, since the only exe edit was reverted): criterion (e) `ai_speed_env.py --check`
> **PASS 3/3**; AI criterion (b) **FAIL 3/3**, `c1_median` 49 / 45 / 52.5 — identical to the
> prior recorded column; power-ups **11/11 decision CLEAN**, contact CLEAN 10/11 with the
> known `g3` divergence; `mashedmod\build.bat` clean with the dual-copy guard at **NEW=0**.
>
> **Still blocking re-closure:** U-9147 (now with a localized channel), and U-9151 for the
> exe copy of A6b. The D3 modes 3/7 hold is unchanged.

---

#### History — **CLOSED 2026-09-14** (gate RECIPE amended 2026-09-29, verdict re-checked)

> #### AMENDMENT 2026-09-29 (second pass) — the controlled arm was ASYMMETRIC; the reference is a SOLO race; there is NO player regression
>
> Supersedes the operative conclusions of the amendment below. Full note:
> [`re/analysis/PLAYER_REGRESSION_2026-09-29.md`](re/analysis/PLAYER_REGRESSION_2026-09-29.md)
> — §1-§2 were written and committed (`235e964a`) before the first run. Verdict commit
> `2348614e`.
>
> **1. The `-16.0% / -14.0% / -64.2%` below is the arm's own asymmetry, not a commit.**
> `MASHED_MEASURE_NOOPP=1` was applied at HEAD and not at `56ad3806`. Applied at BOTH ends,
> `56ad3806` gives `0.1713 / 0.2372 / 705.03`, and `647a5e24` — the *same commit* measured
> both ways — moves `0.1916 / 0.2668 / 1931.36` → `0.1726 / 0.2372 / 692.07`. The
> driving-median's `-64%` is **0%** attributable to any commit. The bisect below that names
> `09a73dc6` first-bad is invalid for the same reason: the plan's rule changed the treatment
> at exactly the commit the search was hunting.
>
> **2. The gate's ORIGINAL-side reference is a ONE-CAR race, and the port arm spawns three.**
> `verify/a8_steer_20260824/orig_steerR.msd.provenance.json` carries no `--cars`;
> `re/frida/scenario_launch.py:1739` defaults it to **1**. Re-run live on the identical
> recipe with `--oracle --rule 0` (`log/rules_oracle_rule0.json`): `cars=1`,
> `SegmentCheck 0x00410d10` **1448/1448 MISMATCH 0 with segment-end = 0**, `m1Max = -1`. So
> no second car exists and no elimination ever runs on the reference side, while
> `TrackRenderer.cpp:2441-2473` / `StartRound` hard-spawn three on the port side.
> `MASHED_MEASURE_NOOPP=1` does not fix that — it only *parks* them. A third harness knob,
> **`MASHED_MEASURE_SOLO=1`** (default-OFF, both spawn sites, `0d889eee`), runs the
> reference's scenario.
>
> **3. On that matched scenario there is NO player-car physics regression.**
>
> | arm | runs | slip 1500-2000 | slip 2000-2600 | driving-median |
> |---|---|---:|---:|---:|
> | ORIGINAL (the reference) | archived | **0.1913** | **0.2498** | **1940.59** |
> | `56ad3806` solo | 3/3 identical | 0.1374 | 0.2181 | 1760.49 |
> | HEAD solo | 4/5 identical | 0.1332 | 0.2179 | 1818.47 |
> | **HEAD vs `56ad3806`** | | **-3.1%** | **-0.1%** | **+3.3%** |
>
> and the mechanism is proved rather than argued: on a per-sim-step `%.17g` player trace,
> `MASHED_GAMEMODE_STUB=0`, `MASHED_NO_START_BOOST=1`, `MASHED_MEASURE_NOAITICK=1` and
> `MASHED_MEASURE_SOLO=1` each leave the player's own `pos`/`yaw`/`vel`/`+0xb14`/`+0xb1c`/
> `+0x9e4` **bit-identical for 156 sim steps**, and the first field that ever differs is
> `race_[0].alive`. The only knob that changes the player from frame 1 is
> `MASHED_NO_SPAWN_SETTLE=1` — deliberate, closed by user decision at U-9142, and required
> by criterion (e).
>
> **4. What IS real, and it is a D2-era question, not a D3 one.** On matched scenarios the
> port is **~28% short on `slip 1500-2000` at BOTH commits** (0.1374 and 0.1332 against the
> original's 0.1913). The row below's `0.1916` vs `0.1913` was a three-opponent port arm
> measured against a one-car original, so the port has never reproduced that statistic
> like-for-like. Filed **U-9147**; re-baselining the row on the matched arm is a user
> decision.
>
> **5. U-9141 and U-9145 are RESOLVED.** U-9145's coupling is two channels and neither is a
> shared physics global: (A) the harness's steer-hold onset was on the real clock while the
> sim runs on a real-time accumulator — **fixed**, onset now counted in sim steps; (B) the
> opponents get the PLAYER eliminated at `race_time_` 2.1-2.6 s through the ported
> `0x00410d10` zoom-saturation path, which freezes `race_[0].gate` into the player's own
> off-mesh re-aim at `TrackRenderer.cpp:2808` — carried as **U-9146**. Also open:
> **U-9148** (one HEAD solo run in five lands in a second attractor).
>
> **The recipe from here:** use `MASHED_MEASURE_SOLO=1` for any D2 gate comparison. The
> `MASHED_MEASURE_NOOPP=1` arm is deterministic but measures a scenario neither side ran.
>
> **6. The five dual-copy leads (`aa4795af`), judged against the original** (§7.3 of the note,
> commit `3e4fba77`). Three exe-copy physics defects are **FIXED**: A5 `0x0046ddb0`'s `.rdata`
> constants (**eight** of them, not the four the audit named — every one a 6-digit truncation
> of an exact round number; now `asFb(bits)`, re-audited 30 exact / 0 mismatch), A3
> `0x0046b540`'s output stride (**0x40**, settled from `add ebx, 0x40` in all three loops, not
> from symmetry), and A6a `0x00467650`'s gear clamp (bound **5**; the original declares
> `local_54[5]` and reads it unclamped). The `CarCarContacts` lead is **REFUTED** —
> `0x00469df0` has zero call sites. **A6b `0x00468980` is CONFIRMED and NOT fixed**: A4 loads
> its ESI from `[esp+0x3c]` (`0x0047093b`), so the exe's `nullptr` kills the whole
> rotation-apply — *and* the `.asi` C4 forwarder's `ESI = record` assumption is unsupported by
> the same instruction, so neither copy is established (**U-9149**). **None of the four fixes
> moves U-9147's ~28% gap**, so U-9149 is its only surviving lead among the five. Guards after
> the fixes: (e) PASS 3/3, (b) FAIL 3/3 on the same bands, power-ups 11/11 decision CLEAN,
> modes oracle rule 3 GREEN 3257/3257, **.asi untouched**.

> #### AMENDMENT 2026-09-29 — the gate recipe gains a CONTROLLED arm; the D2 verdict is re-checked against it
>
> **User decision (2) of 2026-09-29.** The `a8` held-full-lock recipe below stopped being a
> controlled instrument at `09a73dc6` (`D3_DRIVE_2026-09-28.md` §3.4, CONFIRMED: three
> RNG-driven opponents share the player's world during a 50 s donut at the start line, and
> HEAD's run-to-run spread on `slip 1500-2000` became 0.128..0.177). The recipe is therefore
> **amended**, not the bands. Full pre-registration and results:
> [`re/analysis/D2_CONTROLLED_ARM_2026-09-29.md`](re/analysis/D2_CONTROLLED_ARM_2026-09-29.md)
> — §1 and §2 of that note were written and committed (`1d0ca916`) **before the first run**.
>
> **The controlled arm (C-A8-1)** = the recipe below plus exactly two controls:
> `MASHED_MEASURE_NOOPP=1` (opponents not updated; a measurement-harness knob on the
> `MASHED_STEER_HOLD` precedent — it commands the scenario, changes no computed value, is
> default-OFF and is **not** a default-path change, proved by a knob-off control that
> reproduces the pre-knob build to every printed digit), and `--max-lines 1080` on
> `a8_slip_axis.py` / `a8_momentum.py` (a fixed 1080-frame = 18.0 s window, since the chain
> dt is pinned at `frameMs = 50`). Both ends of any comparison run it.
>
> **The arm works.** Run-to-run spread on `slip 1500-2000` is **exactly 0** on 3/3 runs at
> both HEAD and `56ad3806`, where the uncontrolled arm still spreads 0.0088.
>
> **The D2 row re-checked on the controlled arm at `56ad3806` (its own commit), 3 runs:**
>
> | | slip 1500-2000 | slip 2000-2600 | av.y | driving-median | verdict vs the row below, ±2% |
> |---|---:|---:|---|---:|---|
> | D2 row (2026-09-14) | 0.1916 | 0.2668 | 1.12 / 1.59 | 1887 | — |
> | `56ad3806` controlled | **0.1916** | **0.2669** | **1.123 / 1.591** | **1932.1 / 1932.4 / 1931.4** | slip **0.00%** ✓, slip **+0.04%** ✓, av.y exact, median **+2.39%** ✗ |
>
> So **two of the three gated statistics and `av.y` reproduce essentially exactly, and the
> driving-median is +2.39% — 0.4 percentage points outside the pre-registered ±2%.** The same
> overshoot is already in the record independent of any control: `D3_DRIVE_2026-09-28.md` §3.1
> measured today's `56ad3806` build at `1928.48 / 1914.91 / 1931.36 / 1928.48` and accepted
> that as a reproduction. So the row's `1887` is itself ~2% below what the recipe produces at
> its own commit. **The D2 CLOSURE BELOW IS NOT REOPENED** — the physics it certified is
> unchanged and the two slip statistics and `av.y` are reproduced to four decimals — but the
> `1887` figure is flagged as a recorded-value question, and re-baselining it would be a user
> decision because it changes a closed phase's numbers.
>
> **What the arm exposed, and why U-9141 is still OPEN.** With the opponents absent at BOTH
> ends, HEAD is `-16.0% / -14.0% / -64.2%` off `56ad3806` on the three statistics. The
> 2026-09-28 bisect concluded *"no commit in `56ad3806..HEAD` edits the player's solver"* and
> therefore that the drift was purely the instrument; **on a controlled instrument that does
> not hold**, and the bisect has to be redone on the controlled arm — which is now cheap and
> sound, because one run per commit is decisive at a spread of 0. Two `[UNCERTAIN]` rows carry
> it: **U-9141** (the residual HEAD-vs-`56ad3806` player difference) and the new
> **U-D2-OPPONENT-COUPLING** (the opponents move the player's median speed 2538 → 691 even
> though `VehicleCarCarContact` `0x00469df0` has **zero callers** in the port, so the coupling
> is shared mutable state; candidates and the per-global A/B next command are in §3.5 of the
> note). Next commands for both are in that note, not here.

> **CLOSED 2026-09-14.** `MASHED_REAL_PHYSICS` is inverted (`VehiclePhysicsRun.cpp`
> `VehiclePhysics_Enabled`): the ported RWP-3.7 chain, with A4 -> A5 -> A6a run before the
> substep loop (the original's FUN_00470c70 order, default since 2026-09-13), drives the car
> with NO variable set. `MASHED_REAL_PHYSICS=0` reverts to the kinematic scaffold for A/B
> only. Gate evidence, clean env (`verify/d2_close_20260914/cleanenv/`, held full-lock recipe,
> physics variable and order knob both unset) vs `orig_steerR.msd`:
>
> | | slip 1500-2000 | slip 2000-2600 | av.y | driving-median speed | max speed | momentum eff-dt port/orig |
> |---|---:|---:|---:|---:|---:|---:|
> | original | 0.1913 | 0.2498 | 1.14 / 1.46 | 1901 | 2565 | - |
> | default build | 0.1916 | 0.2668 | 1.12 / 1.59 | 1887 | 2573 | 0.98 / 0.99 / 1.03 |
>
> Drivability clause: top speed 2573 is the original's shape (2565), not a clamp; `car_yaw`
> responds to steer (the whole run is a held donut). A8 clause: the slip metric of the
> 2026-08-26 ruling passes at 1.00x / 1.07x; velocity turn rate (momentum identity) within
> 3%; turn radius within 5-11% (twenty-fourth follow-up). Mechanism and the per-law
> verification on both sides: `re/analysis/data/A8_velocity_vector_motion_20260825.md`
> follow-ups 25-27; the ramp regime is a world-level (D1/D3) mismatch and is excluded on
> evidence (follow-ups 28-29).
>
> **Residue carried out of D2, not blocking:** (a) the kinematic scaffold is RETAINED behind
> `=0`, not deleted as this phase first specified — deletion is deferred until the A/B is no
> longer needed; (b) open decision #2 below (re-measure the collision-FX skid thresholds now
> that `vel[]` is real) is now DUE; (c) the 6-11 deg per-wheel force residual and the
> original's launch-at-full-lock wedge (follow-up 29) are recorded [UNCERTAIN].

Original phase text follows.

Invert `MASHED_REAL_PHYSICS`. The ported RWP-3.7 chain drives the car by default; the
kinematic scaffold is deleted, not flagged off.

Blocked by: **the coupling reduction, not the statediff wedge** (corrected 2026-08-21).

The blocker recorded here until now was the "statediff residual wedge — ~1/6 boots,
unbisected second mechanism". That has been measured and **it is not a port defect at
all** — it decomposes into three harness issues, evidence in
`re/analysis/D2_WEDGE_REMEASURE_2026-08-20.md`:

1. **Phase-2 hang** — a Frida `Interceptor` on the hot `0x00496530` during track load,
   armed by `--statediff-drive` before the phase poke. 8/30 hangs when instrumented
   during phase 2 vs **0/26 when not** (Fisher p = 0.0041). Fixed by
   `--statediff-drive-late`, which keeps the drive and arms after phase 3.
2. **"Render collapse"** — the 20 s hold against a **frame-locked ~1090-frame race** at a
   boot-to-boot rate of 33–61 fps, so slow-but-healthy runs were cut off mid-race.
   Gone with `--hold 38` (0/5 vs 5/14).
3. **NOFILE** — Frida `error: could not attach`, cause not investigated.

Also corrected: the original "~1/6 healthy" figure rested on six trials whose five
"healthy" captures (`flake_2..6.msd`) never reach the countdown anchor, so they contain
zero frames in the window where a drive verdict is defined. The planned majority-vote
index bisection would have hunted a culprit hook that does not exist — **do not run it.**

**The actual blocker** (`re/analysis/D2_REALPHYS_REMEASURE_2026-08-21.md`): with
`MASHED_REAL_PHYSICS=1` **the car will not steer.** `car_yaw` is frozen at 1.5498 for the
whole run under `steer=+0.50`; the scaffold control on the identical recipe sweeps
1.5498 → 4.97 and turns normally. Re-measured 2026-08-21 on a post-B5e build and
**identical to the decimal** to the 2026-07-01 and 2026-07-14 measurements, so the B5e
solver-island merge (`021a9f38`) did not close it.

**Localised** via `MASHED_COUPLING_DIAG=1`: over 220 diag samples the chain velocity's
components change (100 distinct x, 98 distinct z) but its **direction never does** (x/z
0.0203 → 0.0210). The vector only scales, so the velocity heading `velH` is pinned at
1.5498, and `yaw` follows it because the alignment block (`VehiclePhysicsRun.cpp:582-589`)
steers `io.yaw` toward the velocity direction rather than from the steer input. So the
missing term is specifically **steer → lateral velocity**, not the coupling wholesale.

Correspondingly narrowed from the prior description: emitted speed is **fine** — `bs`
tracks `desired` exactly (0.012 → 12.000), and actual car motion is ~11.9 units/s against
the scaffold's ~20, i.e. **slower, not faster**. (An earlier version of this section said
"75x too fast"; that misread `PLAY-DEMO`'s `speed=` field, which reports the chain's
internal saturated velocity rather than the car's motion. Corrected 2026-08-21.) The
internal integrator does saturate `kSafetyInternal = 1500`
(`VehiclePhysicsRun.cpp:480`) — real, but it never reaches the car. Deterministic and
instrumented, so a fix is directly measurable: `velH` must move when `steer` is non-zero.

Note the bar this exposes: physics has been **5/5 C4** (A3/A4/A5/A6a/A6b) since
2026-07-01, and this phase's original exit condition ("gated OFF until A6a/A6b reach C4")
was met that day. Per-hook C4 and a drivable default build are different bars and the
ledger tracks only the first.

**Root cause FOUND AND FIXED 2026-08-21** (`9cc41fa8`): `Math/RwMatrixRotate.cpp` read
pi/180 and 1.0f from the MASHED absolute addresses `0x005cd7a8` / `0x005cc320`. Correct in
the injected `.asi`; in the standalone exe **both read 0** (measured), so
`angle_rad = 0`, `one_minus_cos = -1`, and Rodrigues produced `I - K^2 = diag(2,1,2)` — a
scale instead of a rotation. Steered wheels got 2x body-forward, hence no lateral force.
Fixed by materialising the bit patterns as literals. **The car now steers**
(`car_yaw` 1.5123 → 1.4984 → … , path curves) with no regression on the default path, and
the restored rotation is exact (|fwd| 1.0000, rotation = steer angle to 0.01 deg).

**But the fix is one instance of a class — see `re/analysis/RVA_TUNNEL_AUDIT_2026-08-21.md`.**
`exe_main.cpp:5348` maps 0x00500000–0x009fffff as a zero-filled wedge, so MASHED addresses
read **0 silently instead of faulting**; only 8 addresses hold correct values. The audit
found **547 runtime tunnels across 84 of 205 exe TUs**, 405 of them silent data reads. The
**densest cluster is the physics/collision code this phase intends to switch on**: ~80
macros of the form `#define _DAT_005cxxxx (*(const float*)0x005cxxxx)` whose true values
are 1.0f / 0.5f / -1.0f / 2.0f / 0.99f / FLT_MAX, **all evaluating to 0.0f**, currently
dead only because `MASHED_REAL_PHYSICS` is OFF. Inverting the flag activates them
simultaneously. Any plan that treats D2's inversion as a one-line flag flip is wrong on
this evidence.

**UPDATE 2026-08-24 — the physics-cluster tunnel clause of this gate is MET, and the
mechanism above is corrected twice.** Three commits from 2026-08-21/22 settle it:

- `625e91d0` — **there is no runtime zero-filled wedge.** `MapMashedDataSection()` fails
  (80 granules blocked, 0 covered) because `mashed_re.exe`'s **own image** already covers
  the range: `ImageBase 0x00010000`, `SizeOfImage 0xCBD000`, with an 11.9 MB `.data`. The
  silence is a *static image-based* wedge, not a VirtualAlloc one. The paragraph above
  citing "`exe_main.cpp:5348` maps 0x00500000–0x009fffff as a zero-filled wedge" describes
  a mapping that does not take effect.
- `15f088a3` — the **address-collision risk does not exist.** `mashed_re.map` (14,437
  symbols) places **zero** real symbols inside `0x00400000..0x00A00000`; the whole range
  sits inside one named array, `g_b17_low_arena_pad[0x00A00000]` (`exe_main.cpp:241-242`),
  at `0x0019A2E0` followed by the image's largest gap. So a tunnel read returns
  zero-initialised pad and a tunnel write scribbles on pad. The residual risk is about
  **values, not memory safety**.
- `53e5c05d` — **all 83 zeroed physics constants are fixed**, resolved from
  `original/MASHED.exe.unpatched`'s PE section table rather than from the comment glosses
  (per memory `plate-hex-gloss-authoritative`): 83 found, 83 resolved from `.rdata`, 0
  unresolved, 0 gloss mismatches. Verified 2026-08-24: **zero `(const float*)0x00…`
  derefs remain anywhere in `Collision/`.** Measured consequence — the default scaffold
  arm is unchanged, and `MASHED_REAL_PHYSICS=1` moved in the 4th decimal, which proves the
  solver genuinely consumes them. It also cheaply killed a large hypothesis: the 1500
  saturation was **unchanged** with all 83 corrected, so it had a different cause (since
  found — see the saturation thread below).

What that leaves is **not** a D2 blocker: the other ~460 tunnels live in non-physics TUs
and belong to a general hygiene lane, not this gate.

Note also for the A8 task below: `Vehicle/VehicleControl.cpp:155` passes `nullptr` for
`orient` with the comment "orient bound at A8". Binding it reaches
`Math/RwMatrixRotateInner.cpp:159-166` mode 1, which calls through a function pointer read
out of the zero wedge — currently **nullptr**. A8 must handle that.

**Gate:** clean-env race on ported physics that is actually drivable — top speed bounded
below the safety clamp and `car_yaw` responding to steer; A8 velocity/position diff
against original telemetry on matched inputs; ~~and the physics-cluster RVA tunnels
resolved, not merely inactive~~ **(this clause MET 2026-08-24 — `53e5c05d`, 83/83 resolved
from the binary, 0 float-RVA derefs left in `Collision/`)**. (Dropped from the gate:
"wedge rate zero" — it was a harness-configuration property, not a property of the port.)

So the gate now reduces to **A8**: the velocity/position diff against original telemetry
on matched inputs. Drivability is measured — `car_yaw` responds to steer since `9cc41fa8`,
and top speed ramps-and-resets in the stock shape since `8917e29c`. The original-side
capture is already taken (`verify/a8_steer_20260823/orig_steerR.msd`, 2026-08-23); the
standalone-side capture and the diff are what remain. Open sub-question: the **steer sign**.

> **STATUS 2026-09-13 — the slip deficit has a MEASURED mechanism and an A/B that closes it.** The port ran
> A4/A5/A6a after the substep loop; the original runs them before it (FUN_00470c70 step 3 then step 5).
> Behind `MASHED_A8_A4_FIRST=1` the held-lock slip is 0.192/0.263 vs the original's 0.191/0.250 (1.00x/1.05x),
> av.y and the one-frame axis phase match, and the run has no spin-out and 12 reseeds vs 40. Evidence and every law verified on
> both sides: `re/analysis/data/A8_velocity_vector_motion_20260825.md` twenty-fifth and twenty-sixth
> follow-ups. **DEFAULT since 2026-09-13 (same day):** the order is the default build's; `MASHED_A8_A4_FIRST=0`
> is the A/B revert. Clean-env held-lock run: slip 0.192/0.263 vs 0.191/0.250, speed 1874 vs 1901 (twenty-seventh
> follow-up). Open residue: the ramp regime sits ~10% ABOVE the original's slip at full lock and its old-order
> control could not be captured. An original-side ramp capture WAS then taken (twenty-eighth follow-up): the
> original leaves the road and respawns within 2 s of the half-steer onset and is wedged thereafter, while the
> port's collision scaffold keeps it driving — the ramp is not like-for-like at the WORLD level (D1/D3
> residue), so it is neither for nor against the physics port. The held full-lock regime the ruling cites matches.

**RULING 2026-08-26 — the gate metric is SLIP, and D2 stays OPEN.** The standalone-side
capture and diff now exist (`verify/a8_velvec_20260825/cleanhold_motion.log`, held full
lock via the new `MASHED_STEER_HOLD`; reducers `re/tools/statediff/a8_momentum.py` and
`a8_radius.py`), and they put a genuine question on the table: **every trajectory
quantity matches while slip does not.** Radius within 3-11%, yaw rate within 4-10%,
speed within 1% on the ramp run (1778 vs 1760; the held-lock run's gap is attributed to
50 RecoverOffMesh halvings but was never quantified), force magnitude and direction and
the grip chain within 4-22% — but median slip angle is **1.36x to 4.12x short**, and
eight candidate causes are eliminated by measurement (A8 follow-ups seventeen through
twenty-four).

It was proposed that D2 be re-gated on the trajectory instead, on the precedent of
`p[0x1b]` (wrong by 31x, proven behaviourally inert by the reciprocal cancellation in
the seventeenth/eighteenth follow-ups). **That was declined.** Slip remains the metric.
D2 does not close on a matching trajectory while the state variable that generates it is
4x wrong and unexplained. Rationale: this is a port, and an unexplained internal
discrepancy is an unexplained internal discrepancy — re-gating to the quantity that
happens to pass is how a port talks itself into being done, and the project has a
standing rule against exactly that.

Consequence: **the A8 slip deficit is the sole remaining blocker on D2**, and it is a
known-unknown rather than a missing measurement. What is NOT blocking: the off-mesh /
reseed rate (50 per 1100 frames), which the twenty-fourth follow-up establishes is a
`GroundHeight` **collision-scaffold** artifact — the physics record reports `gnd` = 4.0
in 1097/1097 port and 1441/1441 original frames — and therefore belongs to D1/D3, not
here.

~~**Recommended first step, cheap and decisive:** re-run with the wedge granules set
`PAGE_NOACCESS`.~~ **DONE AND REFUTED 2026-08-21 (`625e91d0`) — do not re-run it as
written.** `MASHED_WEDGE_TRAP=1` exists and does exactly what this item asked (maps the
granules `PAGE_NOACCESS` with a vectored handler that logs fault address / EIP / access
kind, unprotects the one 4 KB page and resumes). **It armed 0 granules**, correctly: the
pages report `type=0x1000000` (`MEM_IMAGE`) and the trap refuses `MEM_IMAGE` by design.
Per `15f088a3`, `PAGE_NOACCESS` on `g_b17_low_arena_pad` can never be safe while the exe
overlaps the MASHED range, so **relinking the exe at a base/size that does not span
`0x00400000..0x00A00000` is the only route to trapping** — a general-hygiene project, not a
D2 prerequisite. Keep the trap: it is env-gated and is the cheapest way to re-derive this.
`Math/RwSqrt.cpp:42-63` remains the correct remediation model for individual tunnels
(`RwLutGuard` validates the resolved root and falls back to a CPU path).

Closes v2's **R5**.

### D3 — Default AI, powerups, modes

> **BLOCKED 2026-09-29 (user decision): D2 is REOPENED and must close again before the modes
> 3/7 port (`FUN_00414c30` + `FUN_00484c70`) starts.** See §D2's REOPENED block. Everything
> below still records D3's measured state accurately; what is on hold is starting the next
> port leg. Note also that 8 of the AI rows criterion (b) runs on were demoted C3→C2 on
> 2026-09-29 (`re/analysis/DUAL_COPY_FIX_2026-09-29.md`) — the exe copies differ from the
> bodies the C3s were earned on, which is a live candidate explanation for (b) and is not yet
> tested.

The remaining scaffolds that the default build still runs. WS-C (AI: the FUN_00418860
family replacing the gate-ribbon lane-follower), WS-D (powerup effects: the FUN_0045bba0
dispatcher + 9-entry type table), WS-G (real per-mode rules replacing the env-mapped
elim/laps scaffold).

**Step-1 audit done 2026-09-14** — `re/analysis/D3_AUDIT_2026-09-14.md`. Verdicts:
**modes are already the ported path** (nothing to invert); **powerups** run the ported
dispatcher + 9 decision functions but under a synthetic 1-2 frame invocation with no
opponent owners (G-D1/G-D2); **AI is the real gap** — `Ai_Standalone_Tick` (FUN_00418860)
has zero call sites, so the ported control step FUN_00416250, per-vehicle step
FUN_00418560 and rubber-band FUN_004177b0 never execute. **No `MASHED_*` flag reachable
in a clean-env race is scaffold-selecting**, so step 5 is a port-and-wire task, not a
flag inversion.

~~Note the AI gating is currently mis-documented: `TrackRenderer.cpp:22,44` reference
`MASHED_REAL_AI`, but no `getenv("MASHED_REAL_AI")` exists anywhere.~~ **Fixed 2026-09-09:**
both comments now name the real gates, `MASHED_GATE_RIBBON_AI` (`TrackRenderer.cpp:1813`,
reverts to the ribbon scaffold) and `MASHED_AI_DRIVES_PLAYER` (`TrackRenderer.cpp:2660`);
`MASHED_AI_PUREPURSUIT`, `MASHED_AI_STEERFLIP`, `MASHED_AI_NAV` remain as A/B knobs.

**Gate:** clean-env race where opponents, powerups and mode rules are all the ported
implementations.

**Pass criteria per third (added 2026-09-26, so the gate is falsifiable like D2's):**

- **AI.** (a) `FUN_00443300` and `FUN_00443dc0` are ported, not stubbed. (b) On the
  `verify/d3_ai_20260914/` recipe (original `scenario_launch.py --poke-ctrl-slots
  --statediff-aistep` vs standalone `MASHED_AI_STEPDUMP`), each opponent's control-byte
  distribution (steer [0]/[1], accel [4], brake [5]) matches the original within a tolerance
  **written into this section before the post-port capture is taken** (distinct-value count
  and median, per car). (c) Opponents are driven by the ported ctrl bytes, not the
  `TrackRenderer.cpp:2831-2960` scaffold motion model. (d) `MASHED_AI_TICK` is deleted.

  **(b) tolerance, written 2026-09-26 BEFORE any post-port capture.** Window, per car
  v = 1..3: the 220 AI-step calls starting at the car's first call with accel `c4 != 0`
  (the original steps the AI through the countdown with an all-zero block first; 220 is the
  shortest racing span the recipe gives any original car). Checker:
  `py -3.12 re/tools/ai_ctrl_window.py --check <aistep.csv>`. Each standalone car must lie
  inside the ORIGINAL's envelope, min..max over 12 observations (4 runs x 3 cars:
  `verify/d3_ai_20260914/orig_step_slots` + `verify/d3_ai_20260926/o_spread1..3`):

  | metric (220-call window) | band | | metric | band |
  |---|---|---|---|---|
  | `c0` distinct | 13..37 | | `c0` median | 0 |
  | `c1` distinct | 17..70 | | `c1` median | 0 |
  | `c1-c0` distinct | 29..96 | | `abs(c1-c0)` median | 0..23 |
  | `c4` distinct | 2..4 | | `c4` median | 25..255 |
  | `c5` distinct | 2 | | `c5` median | 0 |

  Why the envelope is pooled across cars and not per car: the original's run-to-run
  spread on ONE car is as wide as its spread across cars. Runs 0-2 are bit-identical call
  for call on all three cars; run 3 (`o_spread3`) is not (accel takes 25/102, the
  `DAT_0089a368 == 1` accel-rescale at `0x004169e0`), and moves car 3 from 96 to 73 steer
  values, `|steer|` median 23 -> 8, `c4` median 255 -> 102. A per-car band would therefore
  be tighter than the original reproduces itself. Measured 2026-09-14 standalone baseline
  (`sa_step.csv`) fails 4-7 of the 10 bands per car. Record: `re/analysis/D3_AI_PORT_2026-09-26.md`.
- **Powerups.** (a) G-D1 closed: the dispatcher `FUN_0045bba0` ticks every frame as in the
  original, not in a 1-2 frame burst. (b) G-D2 closed: opponent slots 1..3 can own and fire.
  (c) Every one of the 9 types fired in a `--statediff-puhook` capture diffs clean on ammo
  decrement, cooldown and fire-mode transition. A type whose contact outcome is blocked is
  recorded as blocked, not passed. **Blocker corrected 2026-09-27**: it is NOT
  `Collision/ContactStubs.cpp` — `Powerup/*.cpp` has no call path into `Collision/*.cpp`
  at all, and 3 of the 4 functions that call `Rw_TransformPoints` are dead. The blocker is
  the unported contact chain (`0x004b4b60`, `0x0045c350`, `0x004b4cd0`, `0x004b4d10`,
  `0x004b4650`, `0x004b5080`, `0x00455910`, `0x00455100`, all C2 with no impl) plus the
  scaffold `IPowerupBackend`. Evidence: `re/analysis/D3_CONTACT_2026-09-27.md`.
  **Narrowed 2026-09-28** (`re/analysis/D3_CONTACT_PORT_2026-09-28.md`): six of those
  eight are now ported in `Powerup/PowerupContact.cpp` — `0x004b4cd0` (and its
  `0x004b4b20` sibling), `0x004b4650`, `0x004b5080`, `0x0045c110`, plus the sweep pair
  `0x004b4b60`/`0x0045c350` — so OIL, P_MINE, SHOTGUN, DRUM and the dispatcher's armed
  sweep measure clean per call site. What still blocks the other four types is NOT the
  chain but their **per-type projectile-update path** (the pools `DAT_006883xx` and the
  TICK bodies): their contact sites sit there, not in FIRE. DRUM is the worked exemplar —
  its pool and state machine are now in `Powerup/PowerupEffects.cpp`.
- **Modes.** (a) The live oracle covers all 11 rules, not 3. (b) The finish-order APPEND
  branch fires at least once (`ord.appends > 0`). (c) G-G1 closed: the round target is derived
  from the rule, not `StartMatch(3)`. (d) G-G2 `rule_engine_on_` default resolved.

"No `MASHED_*` flag is scaffold-selecting" (below) is a claim about flags only. The AI third
~~still RUNS a scaffold by default (the motion model in (c))~~ no longer does since
2026-09-26 (`D3_AI_PORT_2026-09-26.md` §3); the gate is still not met on AI (b). The modes
criteria are all met since 2026-09-26 (`D3_MODES_2026-09-26.md`).

#### D3 STATUS 2026-09-27 (AI row re-measured 2026-09-27; powerups + modes rows 2026-09-26) — NOT CLOSED (AI (b) open). Gate table:

> **AI row update 2026-09-29** (`D3_DRIVE_FORCE_2026-09-29.md`, and the closure block above):
> criterion **(e) is now MET on all three AI cars to within 0.05%** — the unported A6a start-boost
> block was the whole of U-9140's 8-9x drive-force gap. **(b) is the sole remaining D3 blocker**,
> and it is now measured under matched speed for the first time, where it fails wider
> (`c1_median` 42-48 against a band of `[0,0]`, `abs_steer_median` 46.5-58 against a ceiling of 23).
> The (b) prose in the AI row below was scored on cars running 46-90% slower than the original
> over the same window; treat its band numbers as superseded, not its cause analysis.

| Third | Default path today | Measured against the original | Verdict |
|---|---|---|---|
| **AI (WS-C)** | **ported tick FUN_00418860 every frame; its ctrl bytes drive the opponents through the ported physics chain** (2026-09-26) | control-byte diff vs the (b) tolerance, re-captured 2026-09-27 on the post-`4ff428ad` `MASHED_ROUND` route (`verify/d3_ai_20260927/s6rng.csv` vs `o4.msd.aistep.csv`): **identical window result**, steer 80-96 distinct per car (original 29-96). Step INPUTS now captured on both sides (`D3_AI_RESIDUE_2026-09-27.md`) | **(a) met, (b) NOT met - unchanged: cars 2/3 pass all 10 bands, car 1 fails the same 2 (`c0`/`c1` distinct 7/82 vs 13..37/17..70), (c) met, (d) met.** Cause LOCALISED 2026-09-27: `DAT_0089a368` is 0 for the whole standalone window and 1 for 159 of the original's 220 calls (one-shot 20% roll at `0x00417c43`), which changes the spline bank -> curvature (median 10 vs 106) -> the `curv>20` steer multiplier at `0x0041665c` -> the `c0`/`c1` distinct split, and the accel byte at `0x004169e0`. REFUTED: the lookahead target (a zeroed `own_x` faked it) and the phase-8 wall-march. `FUN_00534870` RNG now ported verbatim (U-D3-AIRAND resolved) and it does NOT close (b). Targeting modes 1..10 still stubbed, blocked on the world-object query `FUN_00484c70` |
| **Powerups (WS-D)** | ported dispatcher, **per-frame over 4 slots since 2026-09-26** (G-D1 closed) | 2026-09-26: `--statediff-puhook` capture of all 9 types, replayed through the port TUs: **all 9 decision traces CLEAN, floats bit-exact** (7 of 9 were wrong before and fixed); control RED on 7/9 (`D3_POWERUPS_2026-09-26.md`) | **(a) met, (b) met 2026-09-26, (c) met on decision fields; contact outcomes measured 2026-09-27** - slots 1, 2, 3 armed and FIRED in a 180 s standalone race (`FUN_00415220` ported; OIL is the only type the Training orbs gave, so only that branch is observed live). **Contact half, `--puhook-contacts` captures `verify/d3_contact_20260927/c2,c3` (`D3_CONTACT_2026-09-27.md`): FLASH clean (makes no contact call at all), P_MINE DIVERGES (the port drops on every press edge; the original gates each drop on `0x004b4cd0 != 0` at `0x00457caa` and `0x0045c110 == 0` at `0x00457cfe`, and refused 6 of 7 press edges, then the armed sweep deactivated the slot at call 1229), the other 7 still blocked with a counted reference each. Blocker re-identified: NOT `ContactStubs.cpp` (no Powerup->Collision path exists) but the 8 unported C2 chain RVAs.** **2026-09-28 (`D3_CONTACT_PORT_2026-09-28.md`, captures `verify/d3_contact_20260928/g2,g3`): contact chain PORTED (`Powerup/PowerupContact.cpp`) for the OIL and P_MINE FIRE paths and for the dispatcher's armed sweep `0x0045bcb2..0x0045bd11`. Criterion (c) now **5 clean + the sweep / 4 blocked** (was 1 clean / 1 diverges / 7 blocked): OIL query/gate/lerp/basis 10/10/10/10, P_MINE 2/2/2/2, SHOTGUN 8/8 and 7/7, DRUM 83/83 on two captures, sweep 309/309 and 207/207 — every count exact per call site, with the original's own query verdicts injected through `re/tools/pu_replay`. P_MINE went 78 decision mismatches -> 0 on the same `c2` capture. `0x0045c110` is now instrumented (`scenario_launch.py` `PU_CONTACT`) and returned 0 on every power-up call, which resolves `D3_CONTACT_2026-09-27` §8 item 1: the gate that refused 6 of 7 P_MINE edges is the world query, not the surface. Remaining 4 are blocked on their per-type projectile-update path, not on the chain; three of them (MORTAR, GUN, MISSILE) go through ONE shared routine `FUN_00459620` (callers `0x00453bd9` / `0x00455c29` / `0x004569c5`), which is the single next slice, and R_FLAME `FUN_0045ae80` is the cheapest single-owner one.** **2026-09-28b (same note, new captures `verify/d3_contact_20260928b/m1,m2`): criterion (c) is now **7 clean + the sweep + the shared acquisition / 1 near-clean / 1 partial**. R_FLAME ported (100-record spark pool `0x0068bd00`; query 510/510 on `c3` and 547/547 on `g4`, 546-vs-548 on `g3` with the 2-query residue diagnosed to the unported per-group sort `FUN_0045ac40`). `FUN_00459620` ported (`Powerup/PowerupAim.cpp`) and CORRECTED: it is a target-ACQUISITION routine, not a projectile one — record `0x0068b9f8` stride `0x58`; fallback query `0x459c19` 187/187 with both non-degeneracy controls diverging (`MASHED_AIM_FORCE=none` -> 348, `=lock` -> 0). That CLOSED GUN outright (no site beyond the acquisition four ever appears in a GUN window). MORTAR's own chain ported (`Powerup/PowerupMortar.cpp`; pool `0x00684ea8` stride `0x110`, 32 records): `0x453789` 280/280, gate `0x4537bb` 2/2, lerp 1/1, basis 1/1, and the port carrying its own state drifts **4.77e-07** over 280 updates / 3 projectile lives, against 6.03 / 6.29 / 0.226 for its three controls — the drift is now part of `CONTACT VERDICT` because the counts alone pass a broken integrator. Two new capture channels: `--puhook-aim` and `--puhook-mortar` (the replay's `Backend::car[4]` was never populated, which was the real blocker, not the chain). **MISSILE is the only type left**, mapped in §4.2g: 5 own sites (one, `0x455df9`, missing from every earlier list), one unported leaf `0x004b4d10`, and its tick `0x00455c90` walks two interleaved pools and is not a defined Ghidra function.** **2026-09-28c (captures `s1`,`s2`): MISSILE PORTED (`Powerup/PowerupMissile.cpp`), and criterion (c) CLOSES at **8 clean + the sweep + the shared acquisition / 1 near-clean (R_FLAME)**. Its tick `0x00455c90` was read via a new `--create` mode on `re/tools/decomp_pc.py` (a TRANSIENT function definition against a `-readOnly` pool clone -- no master write). Pools: aim `0x006885d0` stride `0x2c` x5, projectiles `0x006883b0` stride `0x6c` x5, loop walking DOWNWARD. The sphere query is gated on `DAT_007f101c` PARITY (`0x00455d83`..`0x00455d99`), which is why `0x455de0` is ~half of `0x455e59` in every capture. Measured: sphere 78/78 and 44/44, gate 1/1 and 1/1, ground 155/155 and 87/87, and the `+0x28` ground-bias VALUE exact on `s2` once the capture gained a `hit_t` column (the replay had hardcoded `t = 0.5`). `FUN_004b4d10` decoded as the sphere sibling of `0x004b4cd0` (4 dwords, tag 3, same walk) and ported as `Contact::SphereQueryAt`. Controls `noparity` and `flatbias` DIVERGE; `nolife` is INERT (no projectile aged out -- the 3.0 s gate has no control coverage, note OPEN 3c). **Separately, a NEW DECISION-half defect was found and shown NOT to be this work's**: on a MORTAR-then-MISSILE sequence (`s2`) the port fires a MISSILE the original refuses, 181 mismatches, reproduced exactly by `build_control.bat 5bb0d5e3` against the pre-R_FLAME effects. It inflates the dispatcher sweep by 33 calls, which is the whole of `s2`'s contact DIVERGES; every MISSILE/MORTAR/AIM row on it is clean. That is criterion (b) territory and is the next slice.** **2026-09-28d (`D3_BOX_STATE_2026-09-28.md`): that defect is FIXED and criterion (c) is MET on all 9 types.** It was never in the MISSILE chain -- `FUN_00455150`'s own refusal (`DAT_006885d8 + slot*0x2c < 1`, 0x00455163) is already verbatim and the original never reached it. The dispatcher reads a per-slot BOX STATE `DAT_0068d1f0[slot]` at 0x0045bc6b, BEFORE the armed test at 0x0045bcab, and three values short-circuit the whole per-slot pass: 4 (0x0045bc75), 2 (latch 3 at 0x0045bc85, deactivate if the +0xa8 entry is non-null at 0x0045bc98, RA 0x45bc9d), 3 (0x0045bca5). On `s2` slot 0 the original sat at 3 for the whole third activation with EMPTY `fire_modes` and EMPTY `canfire_rets` for all 72 frames; the port called both. Ported into `Powerup/PowerupSystem.cpp`; the five PRODUCERS stay un-ported and are named (`FUN_0045ba00` @0x0045ba00 is the setter, called with 1/4 from `FUN_004111c0` @0x004111c0, 1 from `FUN_0040e590`/`FUN_00424eb0`, 2 from `FUN_00422fd0`/`FUN_0040be50`), so the host supplies the value. MEASURED across all 11 captures (`verify/d3_box_20260928/sweep.txt`): `s2` decision **181 -> 0** and contact **DIVERGES -> CLEAN** (sweep `0x45bcd8` 152-vs-185 -> 152/152, and the compared window on that activation WIDENS 33 -> 72 frames); o3/o4/c2/c3/g2/g4/m1/m2/s1 unchanged; g3 still on exactly its R_FLAME 2-query residue. Control `MASHED_PU_FORCE=nobox` reproduces 181 and the +33. Second witness `b3` at a chosen MORTAR->MISSILE timing via the new `scenario_launch.py --pu-box` (which drives the original's own `FUN_0045ba00`): CLEAN with the gate, **374** mismatches without. Two natural runs `b1`/`b2` never left box state 1, so the instrument is what makes the branch reachable on demand. Naming overclaim on `0045ba00`/`00422fd0` filed as U-9139 (Blocks = nothing).** |
| **Modes (WS-G)** | ported `RaceModes` -> `RuleEngine`, **default-on on every route** (G-G2 closed 2026-09-26); match target = ported `FUN_0040b180` seed + `FUN_00410510` score target (G-G1 closed) | 2026-09-26: live oracle, **all 11 rules**, 0 mismatches. Each rule's own `FUN_00410d10` branch ran on non-degenerate inputs (`seen` ranges); APPEND fired on rules 4/7/8/9; rule 10 needs mode 3/4/5 to tick (`D3_MODES_2026-09-26.md`) | **(a) met, (b) met, (c) met, (d) met.** Residues (§6 of the note): teams seed on the frontend route, delta -1000 display, time-attack flag, all `motion0` exits unreached |

#### D3 closure state 2026-09-28 (orchestrator reconciliation)

- ~~**Powerups (c) is NOT met yet**~~ — **MET 2026-09-28d.** The 181 `s2` decision
  mismatches are gone: `s2` replays **CLEAN** on ammo, cooldown and fire-mode, and its contact
  verdict went DIVERGES -> CLEAN with it. The kickoff's stated hypothesis (the aim record
  `0x006885d0 + slot*0x2c`) was **REFUTED**: `FUN_00455150` does consult it at 0x00455163 and
  the port already matches that verbatim — the original never reached the FIRE call at all.
  The real gate is the dispatcher's per-slot BOX STATE `DAT_0068d1f0[slot]` (read 0x0045bc6b,
  three short-circuit values at 0x0045bc75 / 0x0045bc85 / 0x0045bca5), now ported. Criterion
  (c) therefore reads: all 9 types clean on the decision fields, contact outcomes 8 clean +
  the sweep + the shared acquisition, R_FLAME recorded as a bounded 2-query-of-546 residue on
  one of three captures rather than passed. Evidence: `re/analysis/D3_BOX_STATE_2026-09-28.md`,
  `verify/d3_box_20260928/`, commits `2e7a2b92` + `be06d381`.
- **AI (b): cars 2 and 3 met. Car 1 is named residue D3-R1** (user decision 2026-09-28,
  taken after the speed investigation). Evidence: against the original's pure regime-0
  envelope (24 runs, `81e5ef7b`), car 1 fails `c0_distinct` 7 (floor 13), `c1_distinct` 82
  (ceiling 70), `abs_steer_median` 6 (floor 7). It is also outside the flag-1 regime. Cause
  measured in `D3_SPEED_GAP_2026-09-28.md` (`3f0477c7`): behaviour modes 3/7 are unported, so
  car 1's heading error never exceeds 4.23° where the original reaches 86.56°.
  Substituting the original's speed trace moves only `c1` (82→63). **Route:** port
  `FUN_00414c30` and the world-object query `FUN_00484c70`. **Due before D5.** The bands
  were NOT moved.
- **New finding, D2 scope: U-D3-DRIVE.** Under byte-identical ctrl the ported physics
  accelerates AI cars differently: +25.5% / +6.8% / +5.2% full-throttle median gain (cars
  1/2/3), and a slow launch (+182 vs +1542/+2053 over the first 11 calls, all wheels grounded).
  D2's gate was measured on the player car only. The gearbox pair `+0x490`/`+0x494` is the
  first suspect. Next command: `D3_SPEED_GAP_2026-09-28.md` §6.3. **GATES D3 (user decision
  2026-09-28).** New AI criterion (e): under matched ctrl bytes, AI-car speed gain (full-throttle
  median and the first-11-call launch) matches the original within its own run-to-run spread,
  with the spread measured and written here before the post-fix capture.
- **AI criterion (e) — the tolerance, written 2026-09-28 BEFORE any post-fix capture.**
  Derivation, definitions and the reasoning: `re/analysis/D3_DRIVE_2026-09-28.md` §2.
  Scorer: `py -3.12 re/tools/ai_speed_env.py --check <csv>` (the numbers below are its
  `REFERENCE` / `BAND_PCT` / `GATED` constants). Window = `ai_ctrl_window.py`'s 220 calls
  from the first `c4 != 0`, so (b) and (e) score the same span.

  | stat | definition | gated |
  |---|---|---|
  | `launch` | `rec_9e4[idx 11] - rec_9e4[idx 0]` | **yes** |
  | `ft_median_m0` | median `rec_9e4` over window calls `[0,k)` with `c4 == 255` and `c5 == 0`; `k` = index of the ORIGINAL's first `ai_mode != 0` call, measured 100 / 23 / 39 for cars 1 / 2 / 3 | **yes** |
  | `ft_median` | the same median over all 220 calls | no — reported |

  The full-window median is reported, not gated, because past `k` the original's speed
  history has run through behaviour modes 3 and 7 that the standalone cannot have while
  **D3-R1** is open (mode 7 sets `ctrl[4] = 0x40` at `0x0041688d`), so a median taken
  there scores the missing modes rather than the drive model. `[0,k)` is the only span on
  which both sides are in mode 0. This is not a weakening: the current gap on
  `ft_median_m0` (-50 / -90 / -82%) is **larger** than on the full-window median
  (+25.5 / +6.8 / +5.2%).

  Reference = the original's measured value, from **10 regime-0 captures** (8 committed
  `verify/d3_ai_20260927b/p{1,2,3,4,8,10,11,12}` + 2 same-day `verify/d3_drive_20260928/e3,e4`).
  All ten return these to the printed 0.1; only `start_frame` varies. **The original's
  run-to-run spread is exactly zero**, so the band cannot come from it and is stated as
  inherited: **±2%**, the speed bound already on record in `U9138_FIX_2026-09-28.md` §5
  ("driving-median speed: within 2%"), and looser than the 0.74% D2 itself accepted on
  the player car.

  | car | `launch` ref → band | `ft_median_m0` ref → band | port today | port today |
  |---|---|---|---:|---:|
  | 1 | 1425.7 → 1397.2..1454.2 | 2551.6 → 2500.6..2602.6 | 182.1 (-87.2%) | 1264.5 (-50.4%) |
  | 2 | 2052.5 → 2011.5..2093.6 | 2052.5 → 2011.5..2093.6 | 182.1 (-91.1%) | 210.6 (-89.7%) |
  | 3 | 2055.0 → 2013.9..2096.1 | 2278.1 → 2232.5..2323.7 | 182.1 (-91.1%) | 407.0 (-82.1%) |

  **(e) PASSES iff both gated stats are inside the band on all three cars.** Regime
  condition: `flag_a368` (`DAT_0089a368`, `== 1` arm at `0x004169e0`) must be 0 on every
  window call — it takes 0, 1 and 2 on this recipe, and a capture with `regime0=0` on a
  car is re-taken, not scored. `e1`/`e2` are the two off-regime originals, committed as
  evidence and excluded from the envelope.
- **PHYSICS DRIFT, D2 scope, opened 2026-09-28.** The D2 clean-env recipe on the **player**
  car now gives slip 1500-2000 `0.1283` and driving-median `2508`, where D2 closed
  2026-09-14 with `0.1916` / `1887` (original `0.1913` / `1901`). Present in pre-U-9138
  builds, so a D3 commit landed after 2026-09-14 moved physics. Bisect plan (endpoints,
  17-commit candidate set, discriminator, classification rule):
  `re/analysis/D3_DRIVE_2026-09-28.md` §1. **The D2 table has to be back inside its
  2026-09-14 values before (e) is scored.**
- **D3 closes** when powerups (c) replays `s2` clean AND U-D3-DRIVE meets (e). Powerups (c)
  is DONE (2026-09-28d, `s2` decision CLEAN and contact CLEAN); **U-D3-DRIVE is now the only
  remaining gate**. D3-R1 (car 1) then carries forward like the D1 residue.

#### D3 closure state 2026-09-29b — the CLOSURE PATH is fixed by user decision, and the "player regression" is refuted

Session note: `re/analysis/PLAYER_REGRESSION_2026-09-29.md`. Commits `235e964a` →
`2348614e`.

- **USER DECISION (Mariano, 2026-09-29): D3 CLOSES BY PORTING BEHAVIOUR MODES 3 AND 7** —
  `FUN_00414c30` plus the world-object query `FUN_00484c70` — once the player-regression
  question below is settled. That is the closure path; AI criterion (b) is expected to move
  with it, because modes 3 and 7 are what make the original brake and lift where the port
  holds the throttle pinned.
- **USER DECISION (same): D3-R1 IS NO LONGER A CARRIED RESIDUE.** It was filed 2026-09-28 as
  "car 1 only" against a band scored on cars running 46-90% slower than the original. With
  criterion (e) met to 0.05%, **AI (b) now fails on ALL THREE cars at correct speed**
  (`c1_median` 42-48 against `[0,0]`, `abs_steer_median` 46.5-58 against a ceiling of 23), so
  there is no car-1-specific residue to carry — there is one open criterion, (b), on three
  cars. The 2026-09-28 block below is left as history; this bullet supersedes its D3-R1
  framing.
- **The "player-car physics regression since D2" is REFUTED.** On the D2 reference's own
  scenario (`MASHED_MEASURE_SOLO=1` at both ends) HEAD reproduces `56ad3806` to
  `-3.1% / -0.1% / +3.3%`, and per-sim-step traces show the player's own state is
  bit-identical for 156 sim steps under every D3 knob. Nothing in D3 touches the player's
  force path. Detail and the two new rows (U-9146, U-9147, U-9148) are in the §D2 amendment
  above; **U-9141 and U-9145 are RESOLVED.**
- Guards unchanged by this session: criterion (e) **PASS 3/3** and identical to `sa_b2` to
  every printed digit; AI (b) **FAIL 3/3**, byte-identical to the row below; power-up sweep
  **11/11 decision CLEAN** with `g3` the known R_FLAME contact residue; modes oracle rule 3
  **GREEN 3064/3064 with 2 segment-ends** (wider than the previous 2468) and the new rule 0
  **GREEN 1448/1448**; both build targets clean with the **.asi untouched**.

#### D3 closure state 2026-09-29 — **D3 is NOT CLOSED**; criterion (e) is MET, AI (b) is the sole remaining blocker

Session note: `re/analysis/D3_DRIVE_FORCE_2026-09-29.md`. Commits `9573f3a3` (the port +
criterion (e)), and this block's commit (guards + note). The §2.4 band of
`D3_DRIVE_2026-09-28.md` was NOT touched.

- **Criterion (e): PASSES on all three AI cars, on both gated statistics, all six values
  inside 0.05% of the reference** against a ±2% band (`re/tools/ai_speed_env.py --check`,
  `verify/d3_force_20260929/e_b_check.txt`): `launch` **1426.4 / 2053.0 / 2055.2** against
  1425.7 / 2052.5 / 2055.0; `ft_median_m0` **2550.7 / 2053.0 / 2278.3** against 2551.6 /
  2052.5 / 2278.1. Was -85.9/-90.2/-90.2% and -46.5/-90.2/-84.5% on 2026-09-28. `sa_b3`
  repeats `sa_b2` to every printed digit.
- **Root cause of U-9140's 8-9x: A6a's START BOOST block was never ported.**
  `Integrate2.cpp` carried one line, "[U-A6A-ST0] boost-state machine (+0xbf8 == 1 / == 2)
  ... shape only". Ported verbatim from `FUN_00467650` `0x00467d3a..0x00467e44`: while
  `+0xbf8 == 1` each state-2 active wheel adds `ff` × its forward axis into `+0xb14/18/1c`
  and decrements `+0xbf4` by `dt`; `ff` = 5e6, or `_DAT_005cea28` = 8e6 when
  `FUN_0040e340() == 4` and the car index is `DAT_0088e668`/`66c`. MEASURED on three
  separate original captures (`verify/d3_force_20260929/orig_boost_launch.txt`): the residual
  after the drive-only law is **2 × 5e6 for car 1 and 2 × 8e6 for cars 2 and 3**, exactly
  while `+0xbf8 == 1`, one wheel's worth on the run-out frame and 0.02-0.9% of the drive term
  after. The player is boosted in **neither** player recipe.
- **The cadence question of §4.4 is SETTLED and it does not differ**, both sides, with no new
  probe. Port: render-tick `+0xb14` equals the consumption-time value on 898/899 frames
  (`cad1/cadence.txt`). Original: A4 `FUN_00470670` zeroes the accumulator at entry
  (`0x004706af/b5/bb`) and calls A6a exactly once (`0x0047094c`, no loop, one chunk at budget
  50) — **and** `dt·(+0x54)·kDt × captured +0xb1c` reproduces the original's own per-frame
  speed gain on 9 consecutive frames, which a multi-pass accumulation cannot do. So
  `[A8-B14CADENCE]` is refuted for this comparison, and the force→velocity conversion was
  never the defect. **U-9140 RESOLVED.**
- **U-9142 CLOSED 2026-09-29 by USER DECISION (Mariano): KEEP the spawn settle, default-ON,
  `MASHED_NO_SPAWN_SETTLE=1` retained as the A/B revert.** Consequence for this gate, and it
  is a scoping change rather than a loosening: **AI criterion (b) is RE-BASELINED WITH THE
  SETTLE ON.** Every (b) number quoted in this section from 2026-09-29 onward is the
  settle-on arm, and the settle-off arm is no longer the reference for it.
  **The (b) bands are NOT moved.** The two things the decision was weighed against are
  recorded as FINDINGS against those unchanged bands:
  - cars 2/3's margin failures — car 2 `c1_distinct` 72 and car 3 75 against a ceiling of 70,
    car 2 `steer_distinct` 104 against a ceiling of 96;
  - `accel_distinct` / `brake_distinct` falling to 1, which is the **removal of an artefact**:
    pre-settle the port's only non-255 accel call in the 220-call window was the one the
    spawn transient produced, so those two bands were being satisfied by the defect.
  The same-day measurement supports the decision independently and is stronger than the
  argument available when U-9142 was filed: with the start boost ported,
  `MASHED_NO_SPAWN_SETTLE=1` **fails criterion (e)** on all three cars by -2.1% to -6.9%, so
  the settle is required for the very gate the (b) cost was being traded against. (b) also
  now fails in both arms on all three cars, so it can no longer discriminate between them.
- **AI criterion (b): still NOT met, and now WIDER — the sole D3 blocker.**
  `MASHED_NO_START_BOOST=1` reproduces the 2026-09-28 (b) numbers exactly, so no new defect
  was introduced; the default arm adds `c1_median` 42-48 against a band of `[0,0]` and
  `abs_steer_median` 46.5-58 against a ceiling of 23. **And the framing changes: the previous
  (b) numbers were scored on cars running 46-90% slower than the original over the same
  window, so they were not a measurement of the AI law. (b) is now measured under matched
  speed for the first time.**
- Also decoded here, each replacing an `[UNCERTAIN]` or a wrong constant: A6a `param_1` is the
  CAR INDEX (dispatcher `0x00471071` → A4 `0x0047094c`; was passed 0);
  `FUN_0040e340` = `DAT_008a94d0` the participant count; `DAT_0088e668/66c` are the two
  least-progressed cars (`FUN_00470c70` `0x00470e2e` seed + descending sort by
  `FUN_00408a50` = `*(float*)(0x008a96e8 + car*0x30c)`, `0x00470e66..0x00470f0a`), ported as
  `Fi_UpdateBoostOrder()`; and `Fi_GameMode()` corrected **0 → 6** (`FUN_0040e350` =
  `DAT_0063ba8c`; in-race value 6 on two independent witnesses), audited call-site by
  call-site to have exactly one live consequence. Reverts: `MASHED_NO_START_BOOST=1`,
  `MASHED_GAMEMODE_STUB=0`.
- **Open:** `[UNCERTAIN] U-D3-BOOST-ARM` — the writer of `+0xbf8 = 1` / `+0xbf4 = 1300` is not
  located (`findoffset.py --writes 0xbf8 0xbf4 0xbf0` puts every `.text` access inside
  `FUN_00467650`, so the arming store uses a base the displacement sweep cannot see). Only the
  SEED is measured; the force law is transcribed. Next command in the note §2.5. Consequence:
  the arm is restricted to `slot != 0`, which is what every capture shows, so a human player
  who does not jump the lights is not boosted — unported, not decided. Plus
  `[UNCERTAIN] U-D3-BOOST-ORDER` (the progress float has no writer in the standalone, so the
  8e6 pair cannot evolve with race order; no effect on (e)).
- Guards: power-up sweep **11/11 decision CLEAN**, contact CLEAN 10/11 with `g3` DIVERGES
  unchanged; modes oracle rule 3 **GREEN** (2468/2468, 2/2, 3382/3382, MISMATCH 0, and this
  run *did* produce 2 segment-ends so the rule-3 tail arm is covered); both build targets
  clean and the **.asi untouched** (418 objects up to date — every edited file is exe-only).
  **D2 clean-env MOVES +2..10%** (slip 1500-2000 0.128 → 0.132-0.141, driving-median 2489 →
  2539) and is **attributed by a same-session `MASHED_NO_START_BOOST=1` control** that
  reproduces the 2026-09-28 row to four decimals. The player's force path is unchanged by
  construction (arm gated `slot != 0`); the move is the three opponents launching correctly
  into the shared world — the mechanism §3.4 CONFIRMED and **U-9141** already filed as "this
  recipe is no longer a controlled instrument".

#### D3 closure state 2026-09-28e — **D3 is NOT CLOSED**, measured strictly against the gate

Session note: `re/analysis/D3_DRIVE_2026-09-28.md`. Commits `3ec611b7` (step 0, the plan and
the band, before any edit), `9aca6371` (the drift bisect), `83a7b6ea` (the spawn-settle fix),
`b5923d20` (criterion (e) scored, and the (b) regression).

- **Criterion (e): FAILS on all three AI cars, on both gated statistics.** Scored by
  `re/tools/ai_speed_env.py --check`; both arms deterministic.
  `launch` 200.5 against 1425.7 / 2052.5 / 2055.0 = **-85.9 / -90.2 / -90.2%**;
  `ft_median_m0` 1364.3 / 200.5 / 353.2 against 2551.6 / 2052.5 / 2278.1 =
  **-46.5 / -90.2 / -84.5%**. The gap was -87.2 / -91.1 / -91.1% and
  -50.4 / -89.7 / -82.1% before this session's fix, so the fix is worth ~+10% and (e) is
  nowhere near met.
- **One real defect found and fixed — the SPAWN SETTLE.** The gearbox pair named as the first
  suspect is the right site and the wrong cause: `+0x498` = 40000 and `+0x49c` = 4000 on
  **both** sides. The cause is its input. `VehiclePhysics_Init` memsets the record, so the
  port's first step ran with the per-wheel suspension loads at **zero**, gravity was unopposed
  for that step and the body picked up 216.67 of downward velocity; that made A4's slide
  measure `+0xb0c` (`VehicleControl.cpp:113`, at-rest zero arm `:106` / `0x0047072c`) read
  216.668 instead of ~0, which shortened `fVar5_base` at `Integrate2.cpp:126`
  (`FUN_00467650`) and tripped the gear-0 upshift at `:136-143` — and gear 0 is the only gear
  with a nonzero drive term at standstill (`:134`). The original sits at rest with `vy` 0.000,
  `+0xb0c` 0.0000 and loads `1083.3 / 1083.3 / 1083.3 / 541.7`
  (`verify/d3_drive_20260928/orig_gearbox_launch.txt`). Fixed; verified field-for-field.
- **[UNCERTAIN] U-9140 is the remaining (e) blocker.** After the settle the launch is still
  ~10x short with the gear, the input bytes, the four contacts, the wheel states `2/2/1/1` and
  the gearbox constants all matching on both sides. Localised to the drive accumulator
  `+0xb14`/`+0xb1c` (the original's is 8-9x the port's at matched speed) but **not
  established**, because the `[A8-B14CADENCE]` comment already warns a render-tick `+0xb14`
  may be a post-substep residue. Cadence check first — `UNCERTAINTIES.md` U-9140.
- **REGRESSION, and it is why this session's acceptance is not met: [UNCERTAIN] U-9142.** The
  settle costs AI criterion **(b) on cars 2 and 3** (PASS -> FAIL: `c1_distinct` 72 and 75
  against a ceiling of 70, `steer_distinct` 104 against 96, `accel_distinct`/`brake_distinct`
  down to 1). Reported with a true control. The accel/brake half is the **removal of an
  artefact** — pre-fix the port's only non-255 accel call in the 220-call window was the one
  the spawn transient produced — and the `c1`/`steer` half is 2-8 counts of margin. The fix
  ships **default-ON with the A/B revert `MASHED_NO_SPAWN_SETTLE=1`** (v3 rule: a flag may
  only turn the ported behaviour off), both arms measured. **Keep-or-revert is a user
  decision and was not taken.**
- **The PHYSICS DRIFT row above is answered, and the answer changes the criterion.** First bad
  commit `09a73dc6`; **it is not a drive-law change** — no commit in `56ad3806..HEAD` edits
  the player's solver, and `f39747af`'s `Collision::Rw_TransformPoints` rebind (the suspect
  U-9138 left unproven) is measured inert. What moved the table is that three RNG-driven
  opponents now share the player's world (`09a73dc6`) and the race got longer (`4ff428ad`,
  G-G1/G-G2) — both the port becoming **more** like the original. Today's `56ad3806` still
  reproduces the D2 table 4/4, so the drift is real and its cause is the instrument.
  **Therefore "restore the D2 table" is not achievable by fixing a drive law**; the `a8`
  held-lock recipe needs a controlled arm (fixed frame count, opponents absent or seeded)
  before it gates anything again. Residual filed as **U-9141**. Changing a closed phase's
  gate recipe is a user decision and was not taken.
- Guards, post-fix: D2 clean-env table **unchanged** (slip 0.1281 / 0.1281 / 0.1266,
  driving-median 2489 / 2489 / 2508 vs 0.1283 / 2507.86); power-up sweep **11 of 11 decision
  CLEAN**, contact CLEAN on 10 of 11 with `g3` DIVERGES unchanged (the known 2-of-546 R_FLAME
  residue); modes oracle rule 3 **GREEN** (`SegmentCheck` 2589/2589, `FinishOrder` 2843/2843,
  MISMATCH 0); both build targets clean.

Notes: `D3_AI_RESIDUE_2026-09-27.md` (AI (b) cause + the two refutations + the RNG port),
`re/analysis/D3_AUDIT_2026-09-14.md` (step 1), `D3_AI_TICK_WIRING_2026-09-14.md`
(step 2), `D3_MODES_2026-09-14.md` (step 4), `D3_POWERUPS_2026-09-26.md` (powerups),
`D3_AI_PORT_2026-09-26.md` (AI port + powerups (b)), `D3_MODES_2026-09-26.md` (modes (a)-(d)
+ the rule-10 per-round re-seed refutation).

**No `MASHED_*` flag reachable in a clean-env race is scaffold-selecting** — step 5 has
nothing to invert. ~~The AI gap is a PORT: `FUN_00443300` and the `FUN_00443dc0` tail are
stubbed~~ **Refined 2026-09-26** (`D3_AI_PORT_2026-09-26.md` §2): `FUN_00443300` was already
verbatim. The bang-bang steer had a larger cause: nothing in `mashed_re.exe` advanced the AI
clock (`DAT_007f1008`/`DAT_007f0ff4`, `0x0040fc63`/`0x0040fe5e`), so `FUN_00416250` never
issued a fresh steer. Also fixed: the `FUN_00443dc0` wall-march tail, the missing
`FUN_00443440` curvature walk, four band inputs, `FUN_004a2c48` = truncation, the forward
vector heading, `FUN_00443080` = 0, and the difficulty tables.

~~`MASHED_AI_TICK=1` is a NEW default-OFF gate~~ **DELETED 2026-09-26**, not inverted.

Also found and fixed, wider than D3: every `scenario_launch.py` race left the AI
output-slot table `0x007f1a14[0..3]` at its `.bss` zeros, so all four cars wrote
controller 0's ctrl block. `--poke-ctrl-slots` restores what the original's own allocator
commits at `0x0043f895`. A8/D2 physics captures are unaffected (they drive block 0 directly).

### D4 — Breadth to close P-DoD

Only now does per-function coverage become the driving metric again, and only over the
first-party denominator plus the ~1,788 undiscovered race-closure RVAs. WS-F (data
formats), WS-J (audio remainder), and the C4 verification lane (WS-H) run here.

Named D4 work items (added 2026-09-26, previously implicit):

- **Link the Audio TUs.** D0.7 found 585 audio rows unlinked because the code is
  hook-shaped. Plan the move from hook-shaped to standalone-linked before promoting any of it.
- **Link the Save TUs.** `Save/`, 16 files, 28 C4, unlinked and RVA-tunneled (D0.7). The
  standalone must read and write a `gamesave.bin` the original accepts.
- **D1-residue R1-R3** if they are not already closed during D3.
- **Burn the dual-copy allowlist down to zero** (added 2026-09-29). `re/tools/dual_copy_allowlist.txt`
  currently holds **110** entries — 72 `DUP-IN-TARGET`, 27 `DUP-INSTALL`, 11 `CROSS-TARGET`. Each is
  an RVA with more than one body, which is how five defects shipped in `mashed_re.exe` while
  `hooks.csv` said C4 (`re/analysis/DUAL_COPY_AUDIT_2026-09-29.md`,
  `re/analysis/DUAL_COPY_FIX_2026-09-29.md`).
  **The fix per entry is consolidation, not re-verification:** collapse the pair into ONE shared TU
  judged against `original/MASHED.exe`, so `exe_file == file` and the same body serves both targets.
  Where the two copies genuinely need different wrappers (a register-ABI naked thunk for the `.asi`,
  standalone state for the exe), put the arithmetic core in the shared TU and keep only the thunks
  target-specific — the audit's P4. `scripts/lint_rva_bodies.py` fails the build on any NEW pair, so
  the list can only shrink.
  Order of attack, by shipping risk: (1) the physics A-chain `0x00467650`, `0x00468980`, `0x0046b540`,
  `0x0046ddb0`, `0x00470670` — all on the default race path, all demoted 2026-09-29;
  (2) the AI copies in `Ai/AiStandalone.cpp`, which D3 criterion (b) depends on;
  (3) the 27 `DUP-INSTALL` rows, where the fix is deleting one `RH_ScopedInstall` (six are C4; this
  is the U-9065 class and it is tracker hygiene, not behaviour);
  (4) everything else.
  **Each consolidation re-earns the row's C-level through the normal gates** — the exe copy has no
  evidence of its own, and a copy fixed by reading is a C2-grade statement (`re/CONFIDENCE.md`,
  "Which copy the evidence covers").

**Gate:** every subsystem S-DONE under the clarified S-DoD, **and
`re/tools/dual_copy_allowlist.txt` is empty**.

### D5 — v1.0 ship

P-DoD met. Dev `.asi` out of the shipping matrix. `DEFERRED.md` holds only justified
rows. Shipping checklist (added 2026-09-26):

- D1-residue closed (no legacy D3D9 race renderer, no `MASHED_RENDER_LIBRW=0`).
- `mashed_re.exe` runs without the dev d3d9 shim, `d3d9_real.dll`, the dinput8 ASI loader,
  or any `original/MASHED.exe` boot patch. Those are dev harness only.
- First run works with no `videocfg.bin` / `contcfg%d.bin` present (defaults are written).
- Runtime dependencies listed (CRT, D3D9, DirectShow for video) and checked on a clean machine.
- Multiplayer (WS-I) is out of v1.0 scope per D-11063, and this is stated in the P-DoD.
- Every default-OFF `MASHED_*` gate on a finished port is deleted, and only dev/debug
  knobs remain.

---

## Workstream ledger (carried forward from v2)

Definitions live in the archived v2 §Workstreams. Status as of 2026-09-26 (rows re-checked
against the D1 split and D3 steps 1-4):

| WS | Scope | Status | Phase |
|---|---|---|---|
| WS-A | Vehicle physics | A1–A8 done; **D2 CLOSED 2026-09-14** — ported chain is the default drive model (`MASHED_REAL_PHYSICS=0` reverts); A8 slip diff passes 1.00x/1.07x | D2 ✓ |
| WS-B | Collision / RW-Physics | B5e port DONE (K1..K24, `021a9f38`); ported chain is the default since D2 closed 2026-09-14. The per-row C4-verify campaign is NOT part of the D2 gate. Its remaining scope is unrecorded here [UNCERTAIN: needs a `hooks.csv` filter of B5e rows below C4] | D2 ✓ / D4 |
| WS-C | AI drivers | **D3 2026-09-26:** the tick drives the opponents by default (ctrl bytes -> physics chain), `MASHED_AI_TICK` deleted, `FUN_00415220` fire decision + `FUN_00443440` + `FUN_00443dc0` tail ported; (b) 2 of 3 cars inside the tolerance (`D3_AI_PORT_2026-09-26.md`). Open: targeting modes 1..10, `FUN_00442a60`, car 1 `c0`/`c1` split, brake fraction. **D3 step 2 (2026-09-14):** `Ai_Standalone_Tick` is now wired, behind the new default-OFF `MASHED_AI_TICK`. Measured RED because `FUN_00443300` + `FUN_00443dc0` are stubbed (2-3 distinct steer values vs the original's 33-96). Earlier status: Port DONE (`Ai/AiStandalone.cpp`), `Ai_Standalone_Tick` (FUN_00418860) had **zero call sites**; the default build drives opponents from the ported racing-line target + a scaffold motion model (`TrackRenderer.cpp:2831-2960`). Audit `re/analysis/D3_AUDIT_2026-09-14.md` §1 | D3 |
| WS-D | Powerup effects | **D3 2026-09-26:** dispatcher FUN_0045bba0 ticks every frame over 4 slots (`TrackRenderer::TickPowerupDispatch`, G-D1 closed); all 9 per-type decision traces diff CLEAN against the original on replayed inputs (`D3_POWERUPS_2026-09-26.md`). G-D2 closed: slots 1..3 own and fire (`D3_AI_PORT_2026-09-26.md` §5). **2026-09-27:** the two `ContactStubs.cpp` residuals are bound to the real ports and bit-exact, but that was never the blocker — `Powerup/` has no path into `Collision/` (`D3_CONTACT_2026-09-27.md` §1). Contact outcomes now measured per type: FLASH clean, P_MINE diverges, 7 blocked on the unported chain. Leaves are standalone reimpls via `IPowerupBackend` by design | D3 |
| WS-E | librw renderer | librw is the **default** since `f4815877` (2026-08-19; decision DEC-2 of 2026-07-31 chose it). Open: D1-residue R1-R3 | D1 ✓ / D1-residue |
| WS-F | Data formats | No work since 2026-06-16 | D4 |
| WS-G | Modes & frontend | Ledger was stale. `Race/RaceModes` + `Race/RuleEngine` are the **default** path (`exe_main.cpp:2219/2244`, `RaceSession.cpp:129`, `TrackRenderer.cpp:3840`); every mode flag is revert-only or a post-derivation dev override. Open residues: G-G1 hardcoded `StartMatch(3)`, G-G2 `rule_engine_on_` defaults false. Audit §3 | D3 |
| WS-H | Verification / C4 | Continuous; audit stale | D0, then continuous |
| WS-I | Multiplayer | Deferred — D-11063, justification corrected 2026-08-14 | post-v1.0 |
| WS-J | Audio remainder | No work since 2026-06-16 | D4 |

Critical path unchanged in shape: **D1 (render) and D2 (physics) were the two long poles** (both closed: D1 2026-08-19 as a default flip with D1-residue R1-R3 still owed, D2 2026-09-14)
and are independent of each other. Everything else is the proven parse/port/verify loop
and parallelises.

---

## Trajectory correction

2026-07-01 → 08-14, 374 CHANGELOG entries: **206 discovery promotions (C0/C1→C2) vs 60
C2→C3 vs 8 C3→C4.** Effort has gone into breadth while the things the default build
actually runs went unverified. v3's phase order is the correction.

A third strand ran in August: the QoL work. **The first draft of this paragraph was wrong
on four checkable points and is corrected here rather than quietly edited**, because the
error is instructive — it was written from commit subjects and an impression of volume,
not from the code.

What is actually true:

- It does **not** patch the original exe on disk. It is an in-memory `mashed_qol.asi`
  plus shim env vars and a launcher (`dadacde6`); it adds zero `patch_mashed_*.py`. The
  on-disk unlock patches are from **June** (`28badb07`), part of R0 triage.
- The headline "framerate decoupling, borderless, unlocks" misses the bulk: 27 of 35
  commits are render-interpolation and powerup-render RE.
- **"Advances the port by zero" is false for at least one item.** Borderless/`MASHED_RES`
  made the backbuffer deliberately differ from the client rect, which exposed a real bug
  in our own librw adoption — `mashedmod/deps/librw/MASHED_PATCHES.md:16` (P5): librw
  silently allocates a private depth surface and its camera goes blind to everything
  D3D9 already drew, "inert at 640×480" and therefore invisible until borderless forced
  the mismatch.
- It already stopped. Last QoL commit `3499b0df`, 2026-08-03 — eleven days before v3.

What survives the correction: the strand is **untracked**. It was scoped in its own doc
(`re/analysis/QOL_PATCH_PLAN_2026-08.md`, 833 lines, explicitly "separate from the
RE/standalone lanes"), but appears in no tracker — zero hits across `hooks.csv`,
`DEFERRED.md`, `UNCERTAINTIES.md`, `STUBS.md`. Volume is also less than commit count
suggests: 35 of 65 August commits (54%) but 3,162 of 77,444 insertions (4.1%), across
three days. And there is no shared boot risk — `scripts/repatch_original.py` lists nine
boot patches and no QoL patch is among them; the `.asi` is default-off and env-gated.

So the open decision is not "stop it" (it stopped) but how to file it: dual-use items
like the librw P5 exerciser belong to the port, and the rest belongs to a side-product
with its own tracker.

---

## Process rules adopted 2026-08-14

- **Branch teardown is mandatory** in `frida-sweep` and `ghidra-sweep`, using
  `git branch -d` (never `-D`) so an unmerged branch refuses deletion and surfaces as a
  finding. Skipping this produced 75 stale merged remote branches.
- **Worktrees are removed only via `py -3.12 scripts/diag.py wt-remove`.** Never
  `git worktree remove --force` — it follows the `original/` junction and wipes the game
  install (incidents 2026-06-27, 2026-07-01).
- **Cited evidence must be committed.** The D-S3-BANK closure cited a directory that was
  never in version control; it survived only by luck.
- **Never overwrite an append-only tracker.** `2dee9c67` destroyed 477 CHANGELOG entries
  by writing where it should have prepended; recovered 2026-08-14.

---

## Open decisions

All open decisions carried into v3 were settled on 2026-08-15. Recorded here because the
reasoning matters more than the verdicts.

**QuadRenderer stays 2D-only.** It no longer draws — `Render()` has zero call sites and
`RenderAt()` reaches only an uncompiled branch and a background-load failure path. librw's
I3/I6 cover both of its remaining roles and nothing in any tracker is blocked by it.
Branch `b6/transform` deleted (was `ff527c4b`).

**`promote-c4`'s features: all ported, branch retired.** Shim draw counters (`0a1cbf65`),
then `MASHED_CAM_POSE` + `MASHED_DBG_TEXMATCH` + collision FX (`53855ee1`). The prelight
knob was dropped as superseded by the WS-E atomic-lighting model. The split-screen spike
is preserved as a committed patch under `re/analysis/split_screen/` rather than as a
branch, so D-11063 cites a file that cannot rot instead of commits that deletion would
dangle. `promote-c4` and `ws-visual-polish` retired.

**`CHANGELOG.md` is the tracker audit trail, not a commit log.** That is already what
`re-classify` writes into it, and it is the one role git history cannot serve — git tells
you what changed, not why a confidence moved or why a deferral was upheld. Trying to also
mirror every commit is what made it drift, twice. Git log plus per-lane docs cover the
rest. **Consequence for D0.5:** the task is not "restore completeness" but "state the
scope in the file header and stop measuring it against commit count".

**The QoL strand is filed, not stopped** — it stopped on its own on 2026-08-03. See the
Trajectory-correction section above for the four ways v3's first draft got this wrong.
Remaining work is bookkeeping: give the strand a tracker entry, and reclassify as port
work only those items with a **named consumer** (borderless has one — librw P5; the
render-interpolation findings currently do not).

| # | Follow-up | Owner | Phase |
|---|---|---|---|
| 1 | Give the QoL strand a tracker row; reclassify borderless as port work (librw P5 exerciser). **Still open 2026-09-26**, and in the NEXT_SESSION housekeeping backlog | next housekeeping session (re-classify) | D3 housekeeping |
| 2 | Re-measure the collision-FX thresholds once `MASHED_REAL_PHYSICS` is the default (real `vel[]` makes the slip term carry signal). ~~Suspected over-firing~~ **investigated and dismissed 2026-08-14** — see below. **DUE since D2 closed 2026-09-14**, in the NEXT_SESSION housekeeping backlog | next housekeeping session | D3 housekeeping |
| 3 | ~~Verify collision FX in a race capture~~ **DONE 2026-08-15** — emission verified, `verify/fx_verify/`. Residual: the visual contribution of skid smoke specifically was NOT isolated (`MASHED_NO_PARTICLES` disables the whole particle block), and dark smoke `0x303030` on a night track may be invisible in practice | — | — |
| 4 | Sweep evidence citations into `log/` (D0 item 6: 2,217 files under `log/`, 27 tracked). Every tracker row citing an untracked `log/` path either gets its file committed or gets re-cited. Added 2026-09-26, it had no owner | next housekeeping session | D3 housekeeping |

**Over-firing: investigated 2026-08-15, no defect.** The "2,164 skids/race vs the 251 the
calibration was tuned to" alarm was an **instrumentation artifact, not a behaviour
change**. `fx_skids_` was a single counter summed over all four cars while the debug line
printed only slot 1's inputs, so a stationary car appeared to emit thousands of skids and
no figure was attributable. Made per-slot, the distribution is unremarkable —
772/358/241/378, player highest and genuinely cornering (`skidI=2.82`) — and the earlier
total was further inflated by `MASHED_DRIVE_HOLD` extending the race well past the natural
one the 251 came from. The two numbers were never like-for-like.

The load-bearing check is pool occupancy, since cumulative emit counts cannot answer "is
this starving the shared pool": **peak 175 of 1,200 (14.6%), median 130.** The rate is
comfortably within budget. Thresholds stand at the current drive model.
