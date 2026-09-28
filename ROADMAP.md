# Mashed RE Roadmap — v3 (2026-08-15)

Supersedes v2 (2026-06-09), archived verbatim at
`re/analysis/archive/ROADMAP_v2_2026-06-09.md`. v2's workstream definitions (WS-A..WS-J)
are carried forward below and remain the unit of work; what changes is the **gate**.

> **Naming, 2026-09-26.** In this file, D0–D5 are **phases**. The four user decisions of
> 2026-07-31 in `RE_MASTER_PLAN_2026-07.md` §5 are **DEC-2/4/6/7**. Older text that says "Gate
> D2 (2026-07-31)" means DEC-2 (librw ships), not phase D2 (default physics).
> **State 2026-09-26:** D0 ✓, D1 ✓ (default flip, residue R1-R3 open), D2 ✓, **D3 active**
> (AI: ported tick drives the opponents, (b) 2 of 3 cars in tolerance; powerups (a)(b) met,
> (c) met on decisions; modes GREEN on 3/11 rules), D4-D5 not started.

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

### D2 — Default physics — **CLOSED 2026-09-14**

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

| Third | Default path today | Measured against the original | Verdict |
|---|---|---|---|
| **AI (WS-C)** | **ported tick FUN_00418860 every frame; its ctrl bytes drive the opponents through the ported physics chain** (2026-09-26) | control-byte diff vs the (b) tolerance, re-captured 2026-09-27 on the post-`4ff428ad` `MASHED_ROUND` route (`verify/d3_ai_20260927/s6rng.csv` vs `o4.msd.aistep.csv`): **identical window result**, steer 80-96 distinct per car (original 29-96). Step INPUTS now captured on both sides (`D3_AI_RESIDUE_2026-09-27.md`) | **(a) met, (b) NOT met - unchanged: cars 2/3 pass all 10 bands, car 1 fails the same 2 (`c0`/`c1` distinct 7/82 vs 13..37/17..70), (c) met, (d) met.** Cause LOCALISED 2026-09-27: `DAT_0089a368` is 0 for the whole standalone window and 1 for 159 of the original's 220 calls (one-shot 20% roll at `0x00417c43`), which changes the spline bank -> curvature (median 10 vs 106) -> the `curv>20` steer multiplier at `0x0041665c` -> the `c0`/`c1` distinct split, and the accel byte at `0x004169e0`. REFUTED: the lookahead target (a zeroed `own_x` faked it) and the phase-8 wall-march. `FUN_00534870` RNG now ported verbatim (U-D3-AIRAND resolved) and it does NOT close (b). Targeting modes 1..10 still stubbed, blocked on the world-object query `FUN_00484c70` |
| **Powerups (WS-D)** | ported dispatcher, **per-frame over 4 slots since 2026-09-26** (G-D1 closed) | 2026-09-26: `--statediff-puhook` capture of all 9 types, replayed through the port TUs: **all 9 decision traces CLEAN, floats bit-exact** (7 of 9 were wrong before and fixed); control RED on 7/9 (`D3_POWERUPS_2026-09-26.md`) | **(a) met, (b) met 2026-09-26, (c) met on decision fields; contact outcomes measured 2026-09-27** - slots 1, 2, 3 armed and FIRED in a 180 s standalone race (`FUN_00415220` ported; OIL is the only type the Training orbs gave, so only that branch is observed live). **Contact half, `--puhook-contacts` captures `verify/d3_contact_20260927/c2,c3` (`D3_CONTACT_2026-09-27.md`): FLASH clean (makes no contact call at all), P_MINE DIVERGES (the port drops on every press edge; the original gates each drop on `0x004b4cd0 != 0` at `0x00457caa` and `0x0045c110 == 0` at `0x00457cfe`, and refused 6 of 7 press edges, then the armed sweep deactivated the slot at call 1229), the other 7 still blocked with a counted reference each. Blocker re-identified: NOT `ContactStubs.cpp` (no Powerup->Collision path exists) but the 8 unported C2 chain RVAs.** **2026-09-28 (`D3_CONTACT_PORT_2026-09-28.md`, captures `verify/d3_contact_20260928/g2,g3`): contact chain PORTED (`Powerup/PowerupContact.cpp`) for the OIL and P_MINE FIRE paths and for the dispatcher's armed sweep `0x0045bcb2..0x0045bd11`. Criterion (c) now **5 clean + the sweep / 4 blocked** (was 1 clean / 1 diverges / 7 blocked): OIL query/gate/lerp/basis 10/10/10/10, P_MINE 2/2/2/2, SHOTGUN 8/8 and 7/7, DRUM 83/83 on two captures, sweep 309/309 and 207/207 — every count exact per call site, with the original's own query verdicts injected through `re/tools/pu_replay`. P_MINE went 78 decision mismatches -> 0 on the same `c2` capture. `0x0045c110` is now instrumented (`scenario_launch.py` `PU_CONTACT`) and returned 0 on every power-up call, which resolves `D3_CONTACT_2026-09-27` §8 item 1: the gate that refused 6 of 7 P_MINE edges is the world query, not the surface. Remaining 4 are blocked on their per-type projectile-update path, not on the chain; three of them (MORTAR, GUN, MISSILE) go through ONE shared routine `FUN_00459620` (callers `0x00453bd9` / `0x00455c29` / `0x004569c5`), which is the single next slice, and R_FLAME `FUN_0045ae80` is the cheapest single-owner one.** **2026-09-28b (same note, new captures `verify/d3_contact_20260928b/m1,m2`): criterion (c) is now **7 clean + the sweep + the shared acquisition / 1 near-clean / 1 partial**. R_FLAME ported (100-record spark pool `0x0068bd00`; query 510/510 on `c3` and 547/547 on `g4`, 546-vs-548 on `g3` with the 2-query residue diagnosed to the unported per-group sort `FUN_0045ac40`). `FUN_00459620` ported (`Powerup/PowerupAim.cpp`) and CORRECTED: it is a target-ACQUISITION routine, not a projectile one — record `0x0068b9f8` stride `0x58`; fallback query `0x459c19` 187/187 with both non-degeneracy controls diverging (`MASHED_AIM_FORCE=none` -> 348, `=lock` -> 0). That CLOSED GUN outright (no site beyond the acquisition four ever appears in a GUN window). MORTAR's own chain ported (`Powerup/PowerupMortar.cpp`; pool `0x00684ea8` stride `0x110`, 32 records): `0x453789` 280/280, gate `0x4537bb` 2/2, lerp 1/1, basis 1/1, and the port carrying its own state drifts **4.77e-07** over 280 updates / 3 projectile lives, against 6.03 / 6.29 / 0.226 for its three controls — the drift is now part of `CONTACT VERDICT` because the counts alone pass a broken integrator. Two new capture channels: `--puhook-aim` and `--puhook-mortar` (the replay's `Backend::car[4]` was never populated, which was the real blocker, not the chain). **MISSILE is the only type left**, mapped in §4.2g: 5 own sites (one, `0x455df9`, missing from every earlier list), one unported leaf `0x004b4d10`, and its tick `0x00455c90` walks two interleaved pools and is not a defined Ghidra function.** **2026-09-28c (captures `s1`,`s2`): MISSILE PORTED (`Powerup/PowerupMissile.cpp`), and criterion (c) CLOSES at **8 clean + the sweep + the shared acquisition / 1 near-clean (R_FLAME)**. Its tick `0x00455c90` was read via a new `--create` mode on `re/tools/decomp_pc.py` (a TRANSIENT function definition against a `-readOnly` pool clone -- no master write). Pools: aim `0x006885d0` stride `0x2c` x5, projectiles `0x006883b0` stride `0x6c` x5, loop walking DOWNWARD. The sphere query is gated on `DAT_007f101c` PARITY (`0x00455d83`..`0x00455d99`), which is why `0x455de0` is ~half of `0x455e59` in every capture. Measured: sphere 78/78 and 44/44, gate 1/1 and 1/1, ground 155/155 and 87/87, and the `+0x28` ground-bias VALUE exact on `s2` once the capture gained a `hit_t` column (the replay had hardcoded `t = 0.5`). `FUN_004b4d10` decoded as the sphere sibling of `0x004b4cd0` (4 dwords, tag 3, same walk) and ported as `Contact::SphereQueryAt`. Controls `noparity` and `flatbias` DIVERGE; `nolife` is INERT (no projectile aged out -- the 3.0 s gate has no control coverage, note OPEN 3c). **Separately, a NEW DECISION-half defect was found and shown NOT to be this work's**: on a MORTAR-then-MISSILE sequence (`s2`) the port fires a MISSILE the original refuses, 181 mismatches, reproduced exactly by `build_control.bat 5bb0d5e3` against the pre-R_FLAME effects. It inflates the dispatcher sweep by 33 calls, which is the whole of `s2`'s contact DIVERGES; every MISSILE/MORTAR/AIM row on it is clean. That is criterion (b) territory and is the next slice.** |
| **Modes (WS-G)** | ported `RaceModes` -> `RuleEngine`, **default-on on every route** (G-G2 closed 2026-09-26); match target = ported `FUN_0040b180` seed + `FUN_00410510` score target (G-G1 closed) | 2026-09-26: live oracle, **all 11 rules**, 0 mismatches. Each rule's own `FUN_00410d10` branch ran on non-degenerate inputs (`seen` ranges); APPEND fired on rules 4/7/8/9; rule 10 needs mode 3/4/5 to tick (`D3_MODES_2026-09-26.md`) | **(a) met, (b) met, (c) met, (d) met.** Residues (§6 of the note): teams seed on the frontend route, delta -1000 display, time-attack flag, all `motion0` exits unreached |

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

**Gate:** every subsystem S-DONE under the clarified S-DoD.

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
