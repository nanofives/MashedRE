# Next session — kickoff prompt

## => D2 slip-angle: MECHANISM FOUND 2026-09-13 (twenty-sixth follow-up in the A8 data note)

> **The port ran A4/A5/A6a AFTER the substep loop; the original runs them BEFORE it** (step 3 vs step 5
> of FUN_00470c70). With `MASHED_A8_A4_FIRST=1` the port reproduces the original on the held-lock recipe:
> slip 0.192/0.263 vs 0.191/0.250 (fwd), 0.149/0.221 vs 0.147/0.205 (wheel axis), av.y 1.12/1.58 vs
> 1.14/1.46, the one-frame axis phase (+0.0425 vs +0.0446), and 12 reseeds and no spin-out vs 40 reseeds and 87 spike-window
> rows in the same-build control. Every A6a law was verified on both sides first (a8_wheelvel_orig.py,
> a8_angvel_orig.py, a8_angvel_port.py: 0.99 / 0.998 one-frame predictions).
> **DONE 2026-09-13:** A4-first is the DEFAULT (`MASHED_A8_A4_FIRST=0` reverts). Clean-env held-lock run
> matches: slip 0.192/0.263 vs 0.191/0.250, speed 1874 vs 1901 (twenty-seventh follow-up). Ramp regime:
> port ~10% ABOVE at full lock; old-order ramp control not obtainable (exits ~14 s in, no frames).
> DONE (twenty-eighth follow-up): original-side ramp capture taken with the port's schedule — the original
> crashes, respawns and wedges within 2 s of half steer; the port's collision scaffold keeps it driving. The
> ramp is a WORLD-level mismatch (D1/D3), not a physics one. scenario_launch.py now has
> `--statediff-steer-schedule` and magnitude steer bytes. NEXT is the owner's D2 decision on the held-lock
> evidence alone. Gentler two-sided ramp TRIED (twenty-ninth follow-up): three more original captures; the
> original's launch at full lock from standstill spins out at ~1.5 s and sometimes stays stopped until the
> steer is released (partial bytes exonerated: 33.87x0.75 = 25.37 deg lands exactly). Under wall-clock input
> timing no schedule repeats, so the port-side knob was NOT added. Prerequisite for any ramp comparison:
> frame-anchored injection on the original (steps keyed to the 0x004c1be0 tick counter), then the port knob.
> ~~NEXT: an ORIGINAL-side ramp capture (re/frida scenario capture, .msd) for a like-for-like ramp comparison;~~
> then the owner decides D2's close. Old text follows:
> ~~**OWNER CALL NEEDED:** make the original order the default (v3 default-build rule) and re-gate D2 on~~
> the standing SLIP metric, which now passes; then the D2 gate reduces to the ramp regime re-run.

## => D2 slip-angle session 1 done (superseded by the twenty-sixth) 2026-09-13 (twenty-fifth follow-up in the A8 data note)

> **Read section 6 of the twenty-fifth follow-up for the next measurement.** Settled: the orientation half
> MATCHES (both sides rotate the body from steer x grip, not from +0x9c0; the prompt below is STALE on that
> point - the "alignment block" no longer exists). The per-wheel force law matches on all four wheels. What
> differs: the A6a angular-velocity state +0x9c0 is 22-42% low in the port at equal body rotation, and the
> rotation-path wheel-point velocity term weighs half as much per unit av on the port. The 500-1000 "5x" is a
> regime mismatch and is withdrawn. Tools: a8_orient.py, a8_wheelfit.py, a8_run_port.py; capture
> verify/a8_orient_20260913/. Next: port Rw_MatrixFromAxisAngle + the wheel-point velocity to Python and
> evaluate it on the original record; then log block-#5 torque and #6 damping per frame on the port.

## (stale on the orientation point) KICKOFF PROMPT - D2 slip-angle session (written 2026-09-13)

```
Session goal: explain the A8 slip-angle deficit on ported physics (ROADMAP section D2,
"RULING 2026-08-26"). D2 stays gated on SLIP, not trajectory. Do not re-gate it. Do not
close it by inventing a mechanism. Phase = measure -> localise; port only if the
localisation names a specific line.

STATE YOU INHERIT (do not re-derive):
- Default build: librw is the default renderer (D1 closed 2026-08-19). Physics default is
  still the kinematic scaffold; MASHED_REAL_PHYSICS=1 selects the ported RWP-3.7 chain
  (Vehicle/VehiclePhysicsRun.cpp:203). All 83 zeroed physics constants are fixed
  (53e5c05d); the car steers (9cc41fa8); top speed ramps in the stock shape (8917e29c).
- On matched full-lock inputs EVERY trajectory quantity matches the original: turn radius
  within 3-11%, yaw rate within 4-10%, ramp-run speed within 1% (1778 vs 1760), summed
  per-wheel force magnitude and direction and the grip chain within 4-22%.
- Median slip angle is 1.36x-4.12x SHORT (worst at 500-1000 speed, ~5x; ~1.3x at
  1500-2600). Numbers: re/tools/statediff/a8_momentum.py header and
  re/analysis/data/A8_velocity_vector_motion_20260825.md follow-ups 19-24.
- EIGHT causes are ELIMINATED BY MEASUREMENT. Do not re-test them: (1) grip/clamp chain
  (l_60/ld4/le4/grip match), (2) per-wheel force magnitude, (3) force direction (lateral
  fraction within 2%), (4) the constants (5 wrong-bit literals fixed, sub-0.02% effect),
  (5) force->velocity application (velocity-turn momentum identity gives the same
  effective dt on both sides, confirmed on three regimes), (6) steer-regime mismatch
  (matching regimes changed nothing), (7) a tighter turn radius (radius matches),
  (8) the off-mesh/reseed rate as a fidelity signal (it is a GroundHeight
  collision-scaffold artifact; both sides report gnd=4.0 in every frame).
- ONE PARTIAL LEAD, not yet run to ground (twenty-second follow-up): a body-basis reseed
  ZEROES slip and it takes >12 frames to rebuild; ~29% of port driving frames sit in
  that window. It explains part of the 1000-2000 bands and NOTHING at 500-1000 or
  2000-2600. g_bodyBasisReseed is set only by VehiclePhysics_ResetOrientation
  (VehiclePhysicsRun.cpp:409-418), reached from spawn/grid/off-mesh recovery.
- THE UNTESTED HALF (a8_momentum.py says it in its own header): with the force->velocity
  half proven equal, "the remaining suspect is the orientation half (bodyH)". The port's
  slip is velH - io.yaw (VehiclePhysicsRun.cpp:775) and io.yaw comes from an ALIGNMENT
  block that steers yaw toward the velocity heading (VehiclePhysicsRun.cpp:582-589), i.e.
  the port's body heading is partly derived from velocity. The original's slip is between
  independently stored record fields: forward axis +0x9d4/+0x9dc vs velocity
  +0x9b0/+0x9b8 in the 0xd04 vehicle record (field_trace.py:65-70). A heading that is
  pulled toward the velocity direction cannot hold a large slip angle. This is a
  HYPOTHESIS, not a finding: it has not been measured.

TASK, in order:
1. Measure the orientation half per side, the way the momentum identity was measured for
   the velocity half: d(bodyH)/dt per frame vs the integrated angular velocity (port:
   av=(x,y,z) in motion_diag.log; original: the record's yaw-rate source, which you must
   locate). If the port's bodyH rotates at a rate the original's does not, or is clamped
   toward velH, that is the mechanism. Inputs already on disk, no game run needed:
   verify/a8_steer_20260824/orig_steerR.msd and
   verify/a8_velvec_20260825/cleanhold_motion.log (1097 samples, 50 reseeds) plus the
   ramp run in verify/a8_standalone_20260824/. Reducers:
   re/tools/statediff/a8_momentum.py, a8_radius.py (extend, do not fork).
2. In Ghidra (ghidra-pool skill, read-only slot), find the ORIGINAL's writer of the
   forward axis +0x9d4/+0x9dc. Offset reads are register-relative, so reference_to on the
   record base DAT_008815a0 is the wrong tool; start from FUN_0046b540's init (0x0046bb30
   wheel loop, WS-A1 note) and the A-series plates in re/analysis/ for the per-tick
   orientation integration, and state mechanically what rotates the body basis and from
   which quantity. Cite RVAs. If it is the angular-velocity integrator that B5c ported
   (Vehicle/RwpIntegrator.cpp), diff that path's INPUTS per frame, not its output.
3. Only then compare with VehiclePhysicsRun.cpp:582-589 and Vehicle/VehicleControl.cpp:155
   (orient passed as nullptr, "orient bound at A8" - binding it reaches
   Math/RwMatrixRotateInner.cpp:159-166 mode 1 through a function pointer that is
   currently nullptr; A8 must handle that).
4. Nail down or drop the weakest standing claim before building on anything: the
   held-lock run's 897-vs-1941 median speed gap is ATTRIBUTED to 50 RecoverOffMesh 0.5x
   halvings (TrackRenderer.cpp:1989) but the magnitude was never quantified.
5. If step 1 names a mechanism, port the fix behind an env A/B knob, re-run the held-lock
   recipe, reduce with a8_momentum.py, and report slip per speed band both sides. The
   acceptance bar is the RULING: slip within the same tolerance the other quantities
   already meet, on a run with the reseed contamination quantified.

RECIPE for a port-side capture (from verify/a8_velvec_20260825/PROVENANCE.txt):
  MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1 MASHED_GOTO=6
  MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0 MASHED_DRIVE_HOLD=1 MASHED_WIN_POS=left-bl
  MASHED_MOTION_DIAG=1 MASHED_STEER_HOLD=1 MASHED_STEER_HOLD_AFTER=4 MASHED_MUTE=1
  Reduce: py -3.12 re/tools/statediff/a8_momentum.py <motion.log>
          verify/a8_steer_20260824/orig_steerR.msd --orig-steer-min 33.0 --port-steer-min 0.9
  Kill only the MASHED/mashed_re PID you spawned. Record the capture's git HEAD in
  PROVENANCE.txt. *.log is gitignored: git add -f, as the existing captures did.

RULES THAT BIT EARLIER A8 SESSIONS (all in the data note's "traps"):
- A quantity computed from our own formula is not a measurement (trap 3).
- age>=N / steer>=N filters are regime filters; report n per band, do not quote n<60
  bands as solid (trap 6).
- Re-measure on the CURRENT build before trusting any prior number (nineteenth
  follow-up: every prior figure was stale).
- Log both sides from record fields where possible; no Frida Interceptor on 0x00496530
  during phase 2 (hangs 8/30).
- Write findings into re/analysis/data/A8_velocity_vector_motion_20260825.md as the
  twenty-fifth follow-up, same shape: what was measured, what was refuted, what is open.
  Tracker moves only via re-classify.
```

## ⇒ CURRENT STATE (2026-09-12, uncertainty-drain + 8 promotion rounds) — READ THIS FIRST

Branch `race/first-frame-parity`, tree clean, **27 commits** this session. Zero worktrees,
zero Ghidra pool locks, zero stray processes. Anchor verified.

| measure | value | how to re-derive |
|---|---:|---|
| C4 / C3 / C2 / C1 | 184 / **1029** / 3865 / 821 | `Import-Csv hooks.csv \| Group-Object confidence` |
| gating uncertainties | **0** | `Blocks` cell exactly `C2->C3` or `C3`, Active section only |
| open uncertainty rows | 3,031 | rows `^\| *U-[0-9]+` between the Active and Resolved headers |
| promotion rounds run | 256 | ledger `rounds_run` |
| non-canonical column count | 131 | pre-existing baseline, unchanged all session |

## ⇒ THE TWO THINGS MOST LIKELY TO BE MISREAD

**1. "0 gating" does NOT mean "everything is answered."** 46 rows were gating this morning.
17 were **resolved on evidence**; **29 were DE-GATED** and are still open, carrying their
evidence and next command. The de-gate rests on `re/CONFIDENCE.md`'s own C2→C3 wording — a
field may be named *or* explicitly marked `[UNCERTAIN]` with the marker recorded — so a
properly recorded row is the rubric's sanctioned alternative to a name. Owner-approved.
The test applied to each: *would a byte-for-byte verbatim transcription be wrong without
this answer?* For all 29, no.

**2. If you hit a function you cannot transcribe, that is a NEW finding.** Open a new row;
do not assume an old one covers it.

## What landed

### Uncertainty drain: 46 → 0 gating
17 resolved with citations. The ones worth knowing:
- **U-4780** — takes **five** stack args, not three; `[E+0xc]` proven never addressed.
- **U-4700** — the three constants are DirectShow GUIDs (`MEDIATYPE_Video`,
  `MEDIASUBTYPE_RGB24`, `FORMAT_VideoInfo`), named from the **local Windows SDK**
  `uuids.h`, matched at `+0x00/+0x10/+0x2c` = `AM_MEDIA_TYPE` per `strmif.h`.
- **U-4713** — a full 4×4 carrying translation, not a normal matrix: exactly four `fchs`
  negate the whole first row *including* `-pos.x`.
- **U-4701** — `0x00494b65` **is not a function**; it is the `jne` target inside `0x00494b50`.
- **U-5654** — the `-3` is one uniform 3-pixel inset on both axes (float `3.0` at `0x005cc31c`).

**Three corrections to the existing record**, which matter more than the wins:
- Ghidra's *"could not recover jumptable at `0x0041dec0`"* is a **misclassification** — it is
  `jmp dword ptr [eax+0x48]`, a vtable tail call. That was the only genuine transcription
  gate among the 46 and it existed because nobody disassembled the address.
- **U-4313's fourth write site does not exist.** `0x0043f8d5 mov [ebx+8],edx` never lands on
  `0x007f1a1c`; EBX is never loaded with that constant in the range.
- **U-4583's `DAT_006668f8`** is a digit transposition of `DAT_007668f8`.

### 18 promotions across rounds 249–256
`RwEngineRegisterPlugin`, `DriverSystemDispatch`, `RwErrorModuleDtor`, four r252 leaves,
three r253, `Mat4x3InvertOrthonormal`, two r255 walkers, five r256 forwarders.
**One deliberate non-promotion:** `0x004f10e0` is GREEN 8/8 with path2 PASS and **stays C2** —
both callers are anonymous (`FUN_004e4300` C1; `FUN_004e41e0` has no hooks.csv row), so
promoting would be an island promotion. Evidence banked; unblock by raising either caller.

### Harness work (all SWEEP-CRITICAL)
- **`observe_bufs`** on `stub_dispatch_observe` (path1) — three arms of `0x004c2c90` all
  return 1 and differ only through an out-pointer, so return-only observation could not tell
  a correct port from a swapped one.
- **path2 buffer-arg support** — `bgra_encode` / `ptr_seed_observe` / `stub_dispatch_observe`
  added to the verify template, the missing CONFIG forwarding added to `run_verify_hook.py`,
  and an `orch-iter21` test-shape branch narrowed because it was **shadowing** the new
  handlers. Unblocked 15 registry entries; all 17 `arg_layout` entries re-run, no regressions.
- **`re/tools/caller_screen.py`** — applies the C2→C3 caller rule *before* any code is
  written. On the r256 slice: 227 promotable, 69 caller-blocked.

## ⇒ STANDING RULE LEARNED THE HARD WAY: a new arg_type has FOUR homes
`diff_template.js`, `verify_hook_install_template.js`, **and** the config builders in
`run_diff.py` **and** `run_verify_hook.py`. Both builders are **whitelists that drop unknown
keys silently**. Miss the template and path1 goes GREEN while path2 dies `bad argument
count`; miss the builder and the handler runs against an EMPTY config. This bit three
separate ways in one round. Also check `callFn`'s branch ORDER — a branch keyed off *test
shape* rather than arg_type will shadow later handlers (same lesson as U-9067).

## ⇒ AND: a name may not claim more than its comment does
A naming audit of this session's own work **withdrew or corrected 11 of 18 names**.
`RwFrameHeadSet` asserted a frame type and a head field, neither ever read.
`PizOpenDefaultMode` sat on a **particle** row. `RwRGBAToIntensityScaled` claimed a channel
order its own comment explicitly declined to claim. Grounded names were kept (a C4 or
named-library callee gives you one); otherwise `Fwd<callee>_<literal>` says enough.
**An export rename is not cosmetic — re-run both paths.** The rename script itself replaced
by dict order and substituted `RwPluginListDispatch` *inside* `RwPluginListDispatch3`; only
the re-run caught it. Sort replacement keys by descending length.

## PICK ONE

> **Options A and B were DONE 2026-09-12 (same day, later session).** U-9135: attestation NARROWED, not re-banded
> (13 render rows -> psgp on dispatcher/table evidence, 92 keep `render`, 66 hlsl rows -> psgp; new U-9136 for
> the hlsl band). U-9134: Lua ends at `0x004c0735`; 16 rows -> render, 10 -> unknown. 0 C-levels moved.
> Method that settled both: `reference_to`/`reference_from` per function, never the range. U-9136 (hlsl band) also DONE the same way: 5 -> psgp, 68 -> d3dx9-shader-compiler, range label retired. Remaining pick: **C**, or D2.

### A. **U-9135 — decide the PSGP band disposition.** (recommended, and it is a decision, not research)
103 rows are tagged `render` (first-party) while carrying a note asserting they are
statically-linked Microsoft PSGP. The attested range `0x004ec000..0x004fc9e0` holds 198 rows
that the per-row tag splits into **four contiguous, non-interleaved blocks** — render 25,
psgp 79, render 80, psgp 14 — and only 93 are tagged `d3dx9-psgp`, while 12 psgp-tagged rows
sit outside the range. The clean block structure is the evidence. Same shape as U-9134.
Either **narrow the attestation** to the two genuine psgp blocks, or **re-band** the 103 rows
under library-skip. Do not edit either field before deciding: both are range-assigned and the
wrong choice mislabels 100+ rows. Seven rows in the range are already C3 (five promoted this
session) — **all seven are in the render blocks**, so under the per-row tag they are fine.

### B. **U-9134 — audit the `0x004b4a80..0x004c4000` lua band.** Same class, one row proven
mis-banded (`004c0c20` dereferences RwGlobals). May be hiding reachable first-party work.

### C. **More promotion rounds.** The pool is deep and the loop is cheap now: no path2
friction since r252, and `caller_screen.py` removes ~30% of dead ends before authoring.

## Carried over, still needing YOUR call
- **`area/frontend` is the one unmerged branch**, deliberately (WIP `PanelSortInit` hook).
- **D-11069** — 4 duplicate-RVA rows need a `hooks.csv` schema change.
- **U-9087** — 4 C4 rows may need demotion; their install proof is a byte the original has.
- **`main` is level with HEAD**; nothing pushed to `origin` (182 commits ahead).

## Harness wishlist (measured, per the ledger's own rule)
- **`ptr_to` cannot express buf+OFFSET**, which blocks intrusive CIRCULAR lists whose
  sentinel is an interior address — `0x004c59c0` (`param_1+8`), `0x004d8280`/`0x004d8300`
  (`param_1+0x90`). NULL-terminated lists are unaffected. **Count the rows before spending a
  round on it.**
- Absolute-global seeding combined with `stub_at` — blocks `0x004c9f60` (spec in ledger L2c,
  including the `__stdcall` vtable-slot hazard at `0x004c9f85`).

## Standing rules that bit earlier sessions
- Shadow lane: single-boot verdicts are unreliable; require two boots.
- Races need `--cars 4 --hold 60`; a 1-car race never fires the contact solver.
- Never `git worktree remove --force` — use `py -3.12 scripts/diag.py wt-remove`.
- Kill only PIDs you spawned; never blanket-kill MASHED by name.
- A Python script that reads text and writes text **strips CRLF**. Read/write bytes.
  `git diff --stat` is the tell (3,240 lines "changed" = you did it).
- Do not put a literal `|` in an UNCERTAINTIES cell — it splits the row. Spell it `OR`.

## Ready-to-paste kickoff

> Resume the Mashed RE lane on `race/first-frame-parity`. Read `re/NEXT_SESSION.md` first —
> note that **gating uncertainties are 0 but 29 of the 46 were de-gated, not resolved**, and
> that **U-9135 is an open owner decision affecting 103 rows**. Then pick A (decide U-9135),
> B (audit the lua band, U-9134), or C (more promotion rounds — run
> `py -3.12 re/tools/decomp_pc.py --file rvas.txt --callers --json -o batch.json` then
> `py -3.12 re/tools/caller_screen.py batch.json` and author only from the PROMOTABLE list).
