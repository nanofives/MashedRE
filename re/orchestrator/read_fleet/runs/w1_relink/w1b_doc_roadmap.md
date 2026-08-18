I have the complete evidence chain. Let me verify one final detail on the mirror fix's effect on the A/B versus the original faithfulness number, which is already covered in the files I read. I have everything needed.

---

# D1 reconciliation ÔÇö ROADMAP.md lines 213-261

Read read-only: `ROADMAP.md` (full), `re/analysis/CHANGELOG.md` (newest ~50 entries, header + the 2026-08-15/16 block), all 11 `verify/d1_*/RESULT.md`, `verify/d1_measure/MEASUREMENT.md`, `verify/librw_ref/MANIFEST.md`. `verify/librw_ref` holds only `MANIFEST.md` (no RESULT/MEASUREMENT there).

---

## (1) Chronological evidence chain for the D1 diagnosis

The "one problem" is the D3D9-vs-librw renderer A/B divergence that blocks inverting `MASHED_RENDER_LIBRW`. The diagnosis was wrong **five successive times**, each retracted by the next measurement. The docs' own internal count ("the fourth wrong diagnosis", `d1_fxcut/RESULT.md:36`) starts at #2 below, treating #1 as the initial measurement rather than a diagnosis.

| # | Date | Claim | Where filed | Retracted by |
|---|---|---|---|---|
| **1** | 2026-08-15 | **"The divergence ACCUMULATES ÔåÆ leaked or unreset state; inverting would ship a renderer that drifts."** Figures 71.61 / 69.15 / 68.94 / 21.69. | `verify/d1_measure/MEASUREMENT.md:23-51`; CHANGELOG 2026-08-15 "D1 renderer inversion" ÔÇö **this is the exact text ROADMAP 213-261 is built on** | Same-day bisect (#2): *"MY EARLIER FRAMING WAS BACKWARDSÔÇª inverting would REMOVE a visible defect."* |
| **2** | 2026-08-15 | **"Per-channel R/G gain, D3D9-side, blue channel untouched, switches on at end of round 1."** | CHANGELOG 2026-08-15 "D1 bisect + U-9039 correction"; `verify/d1_measure/BISECT_ROUND_BOUNDARY.md` | 2026-08-16 R/G hunt (#3): *"MY 'PER-CHANNEL GAIN' DIAGNOSIS WAS WRONG."* |
| **3** | 2026-08-16 | **"D3D9 world-coverage failure ÔÇö the world draw stops covering the result-screen view from round 2."** | CHANGELOG 2026-08-16 "D1 R/G hunt" | Same-day drawstream (#4): world submits at 13 batches on every frame ÔåÆ *"WORLD-COVERAGE HYPOTHESIS REFUTED."* |
| **4** | 2026-08-16 | **The "REFUTED" was itself overstated** (the instrument couldn't identify result-screen frames), then resolved into *"TrackRenderer::Render NEVER runs during Results, so neither renderer draws the world on those frames."* | CHANGELOG 2026-08-16 "drawstream refutation WEAKENED" ÔåÆ "ÔÇªquestion SETTLED" | Overturned by CHANGELOG 2026-08-16 "CAPMODE moved to the capture SINK": the `round*_result` shots are measured **InRace**, not Results, so they *are* valid renderer-A/B frames. |
| **5** | 2026-08-16 | **"A second, independent sky-colour divergence ÔÇö librw sky orange, D3D9 grey cloud."** (`01_action` rises to 77.72% with FX removed.) | `verify/d1_nopart/RESULT.md` Finding 2; repeated in `verify/d1_fxbloom/RESULT.md` | `verify/d1_fxcut/RESULT.md:33-49` ÔÇö *"the fourth wrong diagnosis"*: every "residual vs librw" was measured against `verify/allmode/librw/`, a **stale baseline still containing the FX bloom**; fix one arm, re-run the other. |

**What currently stands (authoritative):** `verify/d1_fxcut/RESULT.md` (2026-08-16), *"the renderer divergence is CLOSED, and there was never a sky problem."* The **entire** A/B divergence was **one defect on the D3D9 side present in both runs**: the scaffold FX particle class `kind==2` ÔÇö 36 fully-opaque (`0xffÔÇª`) spin-out billboards that each subtend the whole viewport (`d1_fxbloom/RESULT.md:39-50`). It is now cut from the default build (draw-time kind mask defaulting to 3; `MASHED_PARTS_KINDS=7` restores it). Result: **16 of 16 shots Ôëñ0.64%**, and the accumulation pattern is gone (round1 0.01% ÔåÆ round2 0.18% ÔåÆ round3 0.25%). The "accumulation" was never leaked state ÔÇö spin-outs are eliminations, so the diverging frames were simply the ones captured just after an elimination (`d1_fxbloom/RESULT.md:50`). Determinism held throughout (post-change control reproduces pre-change baseline at 0.00%).

A **separate faithfulness lane** ran the same day (this is the ROADMAP's own "does not settle which renderer is faithful" caveat, lines 231-234). Its chain: wrong pose field `ctrl+0x40` ÔåÆ the RwCamera frame (`d1_carproj`); invented 60┬░ lens ÔåÆ measured 48.46┬░ adopted, far plane = `COURSE.LUA Setup_Fog` arg2 (`d1_lens`); and the **root faithfulness finding** ÔÇö the standalone was rendering the world **mirrored** relative to the original (`d1_basis`), fixed by negating the camera right axis on both paths and reverting the compensating `[D-S3-4]` librw negation (`d1_mirrorfix`). That mirror fix moved orig-vs-standalone from 89.68% to 33.79% at a transplanted pose and left the D3D9-vs-librw A/B unchanged at max 1.01% (a shared reflection cancels inside an A/B). The current A/B max of **1.01%** (post-48.46┬░-lens re-baseline) matches `.happy/project-info.json` (2026-08-17).

---

## (2) Verdict

**The ROADMAP's stated D1 blocker is SUPERSEDED ÔÇö disproven, not merely stale.**

The specific claim in lines 218-229 ÔÇö *"the inversion is BLOCKED onÔÇª an accumulating divergenceÔÇª leaked or unreset stateÔÇª inverting now would ship a default renderer that drifts as you play"* ÔÇö is false. The divergence was a single, static, D3D9-side scaffold defect (FX `kind==2`), the accumulation was a capture-timing artifact of when eliminations occur, and after the cut the A/B is 16/16 Ôëñ0.64% with the accumulation pattern eliminated.

**The artifact that settles it:** `verify/d1_fxcut/RESULT.md` (2026-08-16), corroborated by `verify/d1_nopart/RESULT.md` (isolates the particle pass) and `verify/d1_fxbloom/RESULT.md` (isolates it to `kind==2`), with determinism verified on every run.

**Nuance the replacement must preserve (not the same blocker):** a clean A/B is *"a precondition for invertingÔÇª not proof the result matches Mashed"* (`d1_fxcut/RESULT.md:84-86`). What genuinely remains open is (a) original-faithfulness adjudication ÔÇö now materially advanced by the mirror/lens fixes but not closed, residual 33.79% is sim-moment + lighting + texture (`d1_mirrorfix/RESULT.md:69-72`), and (b) the sky cloud-layer/UV-scroll animation item, still open on its own terms (`d1_fxcut/RESULT.md:87-88`, `DIVERGENCE_LEDGER_3D.md:17-19`). Neither is "an accumulating renderer divergence."

---

## (3) The two internal contradictions in ROADMAP.md

**A. Env-var count: 128 (line 58) vs 138 (lines 156-159).**
**138 is right.** Lines 28-33 and 58 carry the earlier premise figure (128 real env vars / 149 raw tokens). Line 156-159 (D0.2, DONE) supersedes it: the 128 came from *"too strict a regex"* that missed the real `envSet(...)`/`EnvSet(...)` accessors; the generated inventory gives **150 tokens, 138 live env vars, 8 non-env, 4 dead**. Lines 58 and 28-33 were simply never updated after D0.2 completed the same day.
**Source that settles it:** `re/analysis/FLAG_INVENTORY_2026-08-15.md` (cited ROADMAP line 157), *"generated rather than hand-listed."* (Note the token count also drifts: 149 at line 58 vs 150 at line 158 ÔÇö 150 is the generated figure.)

**B. Linkage figures (lines 151-155) vs the D0.7 correction the same day.**
Lines 147-155 (D0.1) assert *"links 193 of 433 .cppÔÇª Save/ 0 of 17, Audio/ 4 of 25ÔÇª absent from the deliverable and no env var can reach them"* and add a "second clause" on the premise of **forgotten/drift linkage**. The **CHANGELOG 2026-08-15 "D0.2 + D0.3 + D0.7ÔÇª LINKAGE CLAIM CORRECTED"** entry retracts this the same day:
- The count is **exe = 198**, not 193 ÔÇö the first parser missed 5 isolated `.obj` (QhullBridge + RwBridge/RwRasterBridge/RwSceneBuild/RwRaceSubmit) that compile before the main `cl` block.
- The framing is wrong: of 218 asi-only files, **215 are `RH_ScopedInstall` hook installers** that patch the *original* binary and **cannot** be linked into a standalone ÔÇö *"There is no drift to fix."*
- The real finding is different and worse: all 16 `Save/` asi-only files are hook installers, so **the standalone has no save subsystem at all ÔÇö absent, not unlinked.**

So the right numbers/framing: **198 linked TUs (not 193); Save is absent-by-construction, not unlinked-by-drift.** The ROADMAP is internally inconsistent because lines 151-155 keep the pre-correction figures/framing *and* line 201-203 (item 7) still poses *"decideÔÇª whether Save/ and Audio/ are unlinked by intent or by drift"* as open ÔÇö a question D0.7 already answered (by construction).
**Source that settles it:** CHANGELOG 2026-08-15 "D0.2 + D0.3 + D0.7 ÔÇª LINKAGE CLAIM CORRECTED" (which itself cites the diff against `mashedmod/asi_sources.rsp`).

---

## (4) Proposed replacement for lines 213-261

*(Matches the file's own v3 style ÔÇö dated corrections, prose, a Gate line, "Closes v2's R4". Not written to any file.)*

---

```markdown
### D1 ÔÇö Default renderer

Invert `MASHED_RENDER_LIBRW`. librw becomes the shipping path; the hand-written D3D9
renderer becomes the fallback, then goes away.

**The A/B divergence that blocked this is CLOSED (`verify/d1_fxcut/RESULT.md`,
2026-08-16).** The clean-env D3D9-vs-librw A/B is now **16 of 16 shots Ôëñ1.01%** across the
standard set ÔÇö max 1.01% is `r5/car_3_weave`, a pre-existing indexed-vs-unindexed fill-rule
delta, not a residue of this work. The accumulation pattern that this section was originally
written around (round1 0.01% ÔåÆ round2 68.94% ÔåÆ round3 69.15%) is gone: 0.01 / 0.18 / 0.25.

**The 2026-08-15 diagnosis in the first draft of this section was wrong, five times over,
and is recorded because the failure mode is instructive.** The divergence was read in turn as
(1) an accumulating "leaked or unreset state" that would make the default renderer drift as
you play; (2) a per-channel R/G gain on the D3D9 side; (3) a D3D9 world-coverage failure;
(4) ÔÇö after that was refuted ÔÇö a claim that the result screen never re-renders the world at
all; and (5) a second, independent "orange sky" colour divergence. Every one was retracted by
the next measurement (chain in `re/analysis/CHANGELOG.md`, 2026-08-15ÔåÆ16). None was leaked
state. The single actual cause is a **scaffold FX particle defect on the D3D9 side, present in
both runs**: `ParticleSystem` kind==2 spawns 36 fully-opaque spin-out billboards that each
subtend the whole viewport (`verify/d1_nopart`, `verify/d1_fxbloom`). It is now **cut from the
default build** (draw-time kind mask defaulting to ambient+car-spray; `MASHED_PARTS_KINDS=7`
restores it; re-pickup condition: the ported `Particle/` system lands). The "accumulation"
was a capture-timing artefact ÔÇö spin-outs are eliminations, so the diverging frames were the
ones captured just after one. Fog was tested and ruled out (`MASHED_NO_FOG=1`, 0.00-0.14%).
Determinism held throughout: the post-change control reproduced the pre-change baseline at
0.00% on all 16 shots.

**A clean A/B is a precondition for inverting, not proof the port is faithful.** The two
paths now agree with each other; neither has been fully adjudicated against MASHED.exe. That
lane advanced materially the same day and is no longer blind:

- **The standalone was rendering the world MIRRORED relative to the original**
  (`verify/d1_basis/RESULT.md`) ÔÇö self-consistent, so gameplay looked normal and it survived
  a clean A/B for months. Fixed by negating the camera right axis on both paths, and the
  compensating librw negation `[D-S3-4]` was reverted (librw's built-in X negation is the
  original's convention; it had been tuned to match a mirrored reference). The A/B is
  unchanged by this ÔÇö a shared reflection cancels inside a D3D9-vs-librw comparison
  (`verify/d1_mirrorfix/RESULT.md`).
- **Lens is measured, not invented.** `fovy = 2┬Àatan(0.45) = 48.46┬░`, `near = 0.1`, and the
  far plane = `COURSE.LUA Setup_Fog`'s far argument, all read live from `RwCamera::viewWindow`
  and adopted (`verify/d1_lens/RESULT.md`). Renderer parity survives the change.
- Against the original at a transplanted pose the figure is 89.68% ÔåÆ **33.79%** after the
  mirror/lens work; the residual is a different sim moment plus lighting and texture, not a
  structural transform (`verify/d1_mirrorfix/RESULT.md`). Whole-frame imgdiff cannot validate
  a pose transplant, so it is not treated as a gate number either way.

**Still open, and NOT the old blocker:** (a) a frame-accurate original-vs-standalone parity
number, which needs the pose read synchronised to the capturing Present; (b) the sky
cloud-layer / UV-scroll animation item (`re/analysis/DIVERGENCE_LEDGER_3D.md:17-19`) ÔÇö note
librw draws no sky, so this is a D3D9-vs-original question in both runs.

Accepted delta on record: D-S3-BANK closed at floor 2026-08-04 ÔÇö transform exact to 4.6e-4
px, residual is a 1-2 px grazing-silhouette fill-rule difference from indexed sector-major
(librw) vs unindexed material-major (D3D9) submission of identical vertices. Evidence
`verify/s3bank_iso/`. Blocks nothing; it is the `car_3_weave` 1.01% shot above.

~~Blocked by R10b.~~ **R10b CLOSED 2026-08-15** ÔÇö the gate has a zero noise floor on every
shot (16/16 byte-identical across runs), root-caused as ambient DirectInput
(`DISCL_BACKGROUND | DISCL_NONEXCLUSIVE`) plus a `MASHED_DETERMINISTIC` backdrop that pinned
the frame index but not wall-clock; both fixed, so every delta above is signal.

**Gate:** clean-env `mashed_re.exe` renders a race through librw; `drawlist_diff.py` GREEN or
every remaining row cited (the A/B condition is met ÔÇö 16/16 Ôëñ1.01%); and an original-side
capture at a matched, frame-synced pose adjudicates faithfulness before the flag default is
flipped. R10b closed so the result is reproducible.

Closes v2's **R4**.
```

---

**Note on style:** the ROADMAP file uses em-dashes and semicolons throughout, so the replacement matches that (per your "in the file's own style" instruction). The project VOICE.md ban on em-dashes/semicolons governs outward text under Mariano's name, not this internal roadmap ÔÇö flagging the tension so you can override if you'd rather I strip them.

One thing I could **not** independently confirm read-only: the exact current A/B table behind `.happy`'s "16/16 max 1.01%" ÔÇö I reconciled it from `d1_lens/RESULT.md` (max 1.01% at 48.46┬░, `car_3_weave`) and `d1_mirrorfix/RESULT.md` (unchanged by the mirror fix). The `d1_fxcut` table itself shows max 0.64% at the *earlier* 60┬░ lens. These are consistent (the +0.37 pt on `car_3_weave` is the lens re-baseline, `d1_lens/RESULT.md:89-93`), so [UNCERTAIN] only in that no single artifact prints the post-mirror-fix full 16-row table ÔÇö the three cited files together imply it.
