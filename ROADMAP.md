# Mashed RE Roadmap — v3 (2026-08-14)

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

Verified 2026-08-14 against the source, not against a doc:

| Ported subsystem | Gate | Default |
|---|---|---|
| librw renderer | `MASHED_RENDER_LIBRW` (`LibRw/RwRaceSubmit.cpp:135-136`, requires exactly `"1"`) | **OFF** |
| Ported vehicle physics | `MASHED_REAL_PHYSICS` (`Vehicle/VehiclePhysicsRun.cpp:154-156`, requires the var to merely exist) | **OFF** |

With neither set, the shipping exe runs a hand-written D3D9 renderer (which is neither
verbatim RW nor librw) and the kinematic drive model that v2 itself called "explicitly
NOT the ported physics". `LibRw/RwRaceSubmit.cpp:218` states it plainly: *"with no env
set the shipping D3D9 path still runs"*. There are **146 distinct `MASHED_*` names** under
`mashedmod/src/`, so "what the exe does" currently has no single answer.

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
4. **The flag inventory is a tracked number.** 146 today. It should fall.

This rule costs something and the cost is worth naming: some flags exist because the
ported path is *not yet good enough* to default. Inverting those flags will make the
default build visibly worse before it gets better. That is the point — it converts
invisible debt into visible, fixable regressions.

---

## Honest baseline (2026-08-14)

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

1. Refresh `SESSION_VERIFICATION_AUDIT` — what is scaffold vs verbatim vs absent, as of
   today, covering B5/librw/2026-07.
2. Publish the flag inventory: all 146 `MASHED_*`, each classed **verification** /
   **migration-in-progress (owner + exit condition)** / **dead**. Delete the dead.
3. Reconcile UNCERTAINTIES `Type` against `Blocks` so "2,530 blocking" becomes a real
   number.
4. Fix `STUBS.md`'s self-inconsistent census (1,072 vs 1,109) and the 13 rows with a date
   in the subsystem cell.
5. Restore `re/analysis/CHANGELOG.md` as a complete index, or explicitly demote it to a
   highlights log and name what replaces it. August has 4 entries against ~20 commits;
   right now it is neither.

**Gate:** every number in this roadmap is reproducible from the repo by a stated command.

### D1 — Default renderer

Invert `MASHED_RENDER_LIBRW`. librw becomes the shipping path; the hand-written D3D9
renderer becomes the fallback, then goes away.

Blocked by, and therefore includes: **R10b** — up to 8 of 13 screenshots differ between
two builds of *identical source*. Nondeterminism in the capture harness makes every
pixel-parity claim unfalsifiable, and it blocks E3' outright. Fix this first; it is
cheap relative to what it unblocks.

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

Blocked by: the **statediff residual wedge** — ~1/6 boots still wedge on an unbisected
second mechanism (the first, Ring5ab980's implicit-EAX defect, was fixed 2026-07-31 per
U-6701). This caps physics C4 evidence at ~5/6 and blocks the B5e verify campaign and
WS-A8.

**Gate:** clean-env race on ported physics; A8 velocity/position diff against original
telemetry on matched inputs; wedge rate zero.

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

Definitions live in the archived v2 §Workstreams. Status as of 2026-08-14:

| WS | Scope | Status | Phase |
|---|---|---|---|
| WS-A | Vehicle physics | A1–A7 done; **A8 blocked on the statediff wedge** | D2 |
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

A third strand appeared in August: QoL patches to the **original** `MASHED.exe`
(framerate decoupling, borderless, unlocks). These advance the port by zero. They are
legitimate as a separate product, but they must not be counted as roadmap progress —
if they continue, they get their own tracker.

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

| # | Decision | Owner | Blocks |
|---|---|---|---|
| 1 | Do the 5 unlanded renderer/shim features on `promote-c4` get ported, or dropped? The branch is their only copy. | user | branch retirement |
| 2 | Is `re/analysis/CHANGELOG.md` a complete index or a highlights log? It is currently neither. | user | D0.5 |
| 3 | Do the original-exe QoL patches become a tracked side-product or stop? | user | trajectory hygiene |

Resolved 2026-08-14: **QuadRenderer stays 2D-only** — it no longer draws (`Render()` has
zero call sites; `RenderAt()` reaches only an uncompiled branch and a background-load
failure path), librw's I3/I6 cover both of its remaining roles, and nothing in any
tracker is blocked by it. Branch `b6/transform` deleted (was `ff527c4b`).
