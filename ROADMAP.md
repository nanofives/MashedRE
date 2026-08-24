# Mashed RE Roadmap — v3 (2026-08-15)

Supersedes v2 (2026-06-09), archived verbatim at
`re/analysis/archive/ROADMAP_v2_2026-06-09.md`. v2's workstream definitions (WS-A..WS-J)
are carried forward below and remain the unit of work; what changes is the **gate**.

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
set the shipping D3D9 path still runs"*. A raw grep finds 149 distinct `MASHED_*` tokens under `mashedmod/src/`, of which
**128 have an actual `getenv`/`GetEnvironmentVariableA` site** — the remainder are
include guards, absolute-address macros, a compile-time `#define`, a filename and a
prose prefix. So "what the exe does" currently has no single answer, and the honest
flag number is **128**, not the 146 this document first claimed (that figure counted
raw tokens; corrected 2026-08-15 during D0).

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
4. **The flag inventory is a tracked number.** 128 real env vars today (149 raw tokens).
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
   `build.bat` links 193 of 433 `.cpp` into `mashed_re.exe`; `Save/` contributes 0 of 17
   files and `Audio/` 4 of 25, so 585 audio and 32 save rows — *including 28 save C4s* —
   are absent from the deliverable and **no env var can reach them**. The default-build
   rule therefore needs a second clause: a capability counts only if its TU is linked
   into the exe *and* reached on the default path.
2. ~~Publish the flag inventory.~~ **DONE 2026-08-15** —
   `re/analysis/FLAG_INVENTORY_2026-08-15.md`, generated rather than hand-listed. **150
   tokens, 138 live env vars, 8 non-env tokens, 4 dead flag names.** Note the count moved
   twice: v3 first said 146 (raw prefix grep), I corrected it to 128 (too strict a regex),
   and the true figure is **138** — `envSet(...)` and `EnvSet(...)` are real accessors that
   the stricter regex missed. Only **3 flags are migration debt** (`MASHED_RENDER_LIBRW`,
   `MASHED_REAL_PHYSICS`, `MASHED_RW_RENDER` which is inert), and 3 are already correctly
   inverted. The 4 dead names are comment-only: a comment naming a flag that does not
   exist is a false map — delete or implement.
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

### D1 — Default renderer

Invert `MASHED_RENDER_LIBRW`. librw becomes the shipping path; the hand-written D3D9
renderer becomes the fallback, then goes away.

**Measured 2026-08-15, and the inversion is BLOCKED on a new finding
(`verify/d1_measure/MEASUREMENT.md`).** With the R10b-fixed gate, a like-for-like run
differing only in that flag gives: 12 of 16 shots at or near parity (≤0.92%), and four
that diverge — `01_inrace_track` 71.61%, `round3_result` 69.15%, `round2_result` 68.94%,
`01_action` 21.69%.

**The divergence accumulates.** `round1_result` 0.01% → `round2_result` 68.94% →
`round3_result` 69.15%; `01_grid` (early race) 0.02% → `01_action` 21.69% →
`01_inrace_track` (late) 71.61%. Parity holds for the first round and the start of a race
and degrades after. That is leaked or unreset state, not a static shading difference —
inverting now would ship a default renderer that drifts as you play. The
round-1/round-2 boundary is a clean bisection point.

Note also what the measurement does **not** settle: which renderer is *faithful*. Both were
compared to each other, not to the original. Resolving that needs an original-side capture
at the same pose, which became possible today (`MASHED_CAM_POSE` + the shim's
`draw3d.json`).

~~Blocked by R10b.~~ **R10b CLOSED 2026-08-15 — the gate now has a zero noise floor on
every shot (16/16 byte-identical across runs).**

The "8 of 13 shots differ between builds of identical source" figure this section was
written around was already stale: R10b was root-caused on 2026-08-01 as ambient
DirectInput (the device is opened `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE`, so typing in
another window flew the camera mid-capture), fixed, and reduced to 3 unstable shots.

The residual was closed today by running the diagnostic the sizing doc had already
specified but never executed — compare `RELIGHT_CAP` headings across two runs. **Headings
were bit-identical while `02_back_to_menu` differed on 17.30% of pixels**, which localises
the divergence to the renderer, not the simulation. Cause: `MpegVideoTexture::Update()`
pulls whatever frame the live DirectShow graph is on, and `MASHED_DETERMINISTIC` pins the
frame *index*, not wall-clock. Deterministic mode now freezes the backdrop after one
upload — the menu still shows real video (93.2% non-black), and the frame reproduces.

Accepted delta on record: D-S3-BANK closed at floor 2026-08-04 — transform exact to
4.6e-4 px, residual is a 1–2 px grazing-silhouette fill-rule difference from indexed
sector-major (librw) vs unindexed material-major (D3D9) submission of identical
vertices. Evidence committed at `verify/s3bank_iso/`. Blocks nothing.

**Gate:** clean-env `mashed_re.exe` renders a race through librw; `drawlist_diff.py`
GREEN or every remaining row cited; R10b closed so the result is reproducible.

Closes v2's **R4**.

### D2 — Default physics

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

Note the AI gating is currently mis-documented: `TrackRenderer.cpp:22,44` reference
`MASHED_REAL_AI`, but **no `getenv("MASHED_REAL_AI")` exists anywhere**. The real gates
are `MASHED_AI_DRIVES_PLAYER` (`TrackRenderer.cpp:2413`), `MASHED_AI_PUREPURSUIT`,
`MASHED_AI_STEERFLIP`, `MASHED_AI_NAV`. D0's flag inventory fixes this.

**Gate:** clean-env race where opponents, powerups and mode rules are all the ported
implementations.

### D4 — Breadth to close P-DoD

Only now does per-function coverage become the driving metric again, and only over the
first-party denominator plus the ~1,788 undiscovered race-closure RVAs. WS-F (data
formats), WS-J (audio remainder), and the C4 verification lane (WS-H) run here.

**Gate:** every subsystem S-DONE under the clarified S-DoD.

### D5 — v1.0 ship

P-DoD met. Dev `.asi` out of the shipping matrix. `DEFERRED.md` holds only justified
rows.

---

## Workstream ledger (carried forward from v2)

Definitions live in the archived v2 §Workstreams. Status as of 2026-08-15:

| WS | Scope | Status | Phase |
|---|---|---|---|
| WS-A | Vehicle physics | A1–A7 done; **A8 blocked on the coupling reduction** (not the statediff wedge — corrected 2026-08-21, see §D2) | D2 |
| WS-B | Collision / RW-Physics | B5e port DONE (K1..K24, `021a9f38`); C4-verify campaign open | D2 |
| WS-C | AI drivers | C1 done (`re/analysis/ai_controller.md`); port + wire open | D3 |
| WS-D | Powerup effects | D1 done (`structs/powerup_system.md`); D2/D3 gated on a Ghidra fn-split of 0x453f60–0x45be81 | D3 |
| WS-E | librw renderer | Gate D2 (2026-07-31) made librw the shipping renderer — **not yet the default** | D1 |
| WS-F | Data formats | No work since 2026-06-16 | D4 |
| WS-G | Modes & frontend | No work since 2026-06-16 | D3 |
| WS-H | Verification / C4 | Continuous; audit stale | D0, then continuous |
| WS-I | Multiplayer | Deferred — D-11063, justification corrected 2026-08-14 | post-v1.0 |
| WS-J | Audio remainder | No work since 2026-06-16 | D4 |

Critical path unchanged in shape: **D1 (render) and D2 (physics) are the two long poles**
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
| 1 | Give the QoL strand a tracker row; reclassify borderless as port work (librw P5 exerciser) | — | D0 |
| 2 | Re-measure the collision-FX thresholds once `MASHED_REAL_PHYSICS` is the default (real `vel[]` makes the slip term carry signal). ~~Suspected over-firing~~ **investigated and dismissed 2026-08-14** — see below | — | D2 |
| 3 | ~~Verify collision FX in a race capture~~ **DONE 2026-08-15** — emission verified, `verify/fx_verify/`. Residual: the visual contribution of skid smoke specifically was NOT isolated (`MASHED_NO_PARTICLES` disables the whole particle block), and dark smoke `0x303030` on a night track may be invisible in practice | — | — |

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
