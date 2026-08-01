# Mashed RE — Master Execution Plan (REBUILT 2026-07-31)

Companion to `ROADMAP.md` (v2, phases R0–R8). ROADMAP defines the *gates*; this plan defines the
*route*. **This is a full rebuild** of the 2026-07-03 plan (orchestrate iter27b; the prior text is
in git history — `git log -p re/analysis/RE_MASTER_PLAN_2026-07.md`). The old doc had drifted
materially: it listed B5e as "OPEN, next big lane" 11 days after the port merged (`021a9f38`,
2026-07-20), and its §1 counts were three refresh cycles old. Per-item history lives in
`re/analysis/CHANGELOG.md`; this doc is the strategic index only.

> Active phase: **R7 scaffold→verbatim conversion** (R0–R6 closed).
> Maintenance rule: refresh §1 + §3 after each merged lane; a claim contradicted by CHANGELOG
> is a bug in THIS file — fix it the day it is noticed.

## 1. Where we actually are (refreshed 2026-07-31)

**Confidence ladder** (`hooks.csv`, 5,897 rows): C1 795 · C2 4,005 · **C3 881 · C4 185**
(C3+ = 1,066, 18.1%) · 31 untagged utility rows. Trackers: DEFERRED 672 D-rows (open+struck),
STUBS 1,107 S-rows, UNCERTAINTIES 3,003 U-rows (the large majority data-semantic/non-blocking).

**Milestone/lane status:**

- **M1 "playable whole game" — CLOSED 2026-07-13.** All 9 tail items done (D-11056/57/58/59,
  WS-G4, WS-D unblocked slice, WS-J M1 slice, video scope D-11062, MP gate D-11063).
- **B5 physics lane — PORT COMPLETE.** B5a (07-14), B5b qhull vendor bit-identical (07-14),
  B5c integrator subset (07-15), B5d coupling bridge C3 (07-15), **B5e solver island K1..K24
  merged main `021a9f38` 2026-07-20**. "System 2" = RenderWare Physics 3.7, vendored
  qhull-2002.1 (`REALfloat=1`, `REALepsilon=1e-6`).
- **B5e C4-verify campaign — ACTIVE, owned by the defect session** (branch
  `fix/u9025-recharacterise-and-regabi-defects`): KV batches A/B (07-24/25, vtable + call-frame
  recoveries), then the **statediff warp-wedge hunt** — Ring5ab980 implicit-EAX culprit found by
  index bisection and fixed (07-31, U-6701), full-set boots 0/~19 → 5/6.
  **Residual: ~1/6 boots still wedge; second mechanism unbisected** (possibly warp-timing race).
- **Lane B (batch C2→C3 promotion) — the synthetic getter lane is MINED OUT** (orchestrate
  iter27, measured: 32 fresh rows across 6 subsystems → 0 SAFE non-degenerate getters; iters
  22/24 covered the other 4 subsystems, same result). What remains at C2 is mutators / teardown /
  device / dispatch — further yield requires harness capital (§4).

**Structural facts that shape everything below:**

1. **Two live sessions.** The defect session owns the statediff/regabi lane and these files:
   `scenario_launch.py`, `PromoLoop_sessionB.cpp`, `UNCERTAINTIES.md`,
   `re/analysis/bucket_audio_005ab710_005af040/0x005ab980.md`. Other sessions must not touch
   that lane; tracker writes needing UNCERTAINTIES.md go through multi-session coordination.
2. **The implicit-EAX / register-ABI defect class is real and recurring** (U-9025
   AudioThreadDescInit, Ring5ab980/U-6701): a synthetic C-level diff cannot see a caller-side
   register contract. Any port of a function whose caller consumes EAX/ESI implicitly needs the
   raw listing read at port time, and statediff/bisection at verify time.
3. **Harness safety, not the caller gate, bounds the synthetic promotion lane** (iters 24–27).
   The caller gate is solved (resolver in `orch_make_brief_queue.py` + `orphan_owners.tsv`);
   the survivors die on TEARDOWN / WRITES_GLOBAL / DESTROYS_DEVICE / CALLS_UNKNOWN.

## 2. Finish line (unchanged)

`ROADMAP.md` §DoD: standalone `mashed_re.exe` boots from data files, plays the full canonical
loop faithfully, subsystem DoD via the C-ladder + parity harness. Function-level acceptance is
unchanged: `diff-original` Frida diff (or the parity harness for visual work) — never
compile-and-doesn't-crash.

## 3. Milestones (rebuilt)

### M2 — "Faithful race" — IN PROGRESS, critical path = the statediff residual
The physics pole is ported; what remains is **convergence evidence**, then the gated tails:
1. **Statediff residual wedge** (~1/6 boots, second mechanism) — defect session's lane.
2. **B5d/B5e C4 per-field body-state bit-diff** (angular fields at the documented x87 ULP
   floor) — folded into the statediff campaign.
3. **VECCAP-2 `FUN_00566200`** (AABB×matrix): confirmed defect, 490/513 replay fail; needs a
   disasm-order/float10 re-transcribe (`Collision/RwpSolverMath2.cpp`). Adjacent to the defect
   session's lane — coordinate before starting.
4. **WS-A8 true diff + steer calibration** — actionable now that B5e is ported; folds into the
   statediff campaign's evidence.
5. **M2 backlog now nominally unblocked by B5-completion** [UNCERTAIN — re-inventory before
   starting]: WS-D gated slice (MISSILE velocity, projectile pools), WS-J impact/skid FX,
   U-9016 vehicle→engine-class map.

### M3 — "Faithful render" — GATED ON D2, do not start until decided
~770 render rows < C3 + ~217 stubs (drifting down slowly; a handful promoted 07-30/31).
**Gate D2 (open):** commit to the RW-subset verbatim port (months) vs ship on `librw`/the
D3D9 spike (deviates from pure verbatim). Every M3 token spent before D2 is decided is at risk.

### M4 — "Ship" — HUMAN-GATED tail
D-11060 (interactive track/car select playthrough) + D-11061 (full-loop desktop recording)
need a human at the keyboard; G3 cup place-names needs one Frida session; D-11062 intro movies
optional. None of this is autonomous work — schedule it, don't queue it.

## 4. Lane B (promotion) — capital required, else parked

The flat lane is mined out; these are the only routes with expected yield, cheapest first:

1. **`entity_field_set` per-side sentinel reset** — fixes a latent false-GREEN in a *shipped*
   handler (Orig pre-writes the slot; a do-nothing reimpl reads it back GREEN — iter26) and
   unlocks the strided-global-setter class (e.g. `0x0047cde0`, menu-window only).
2. **`stub_ret_buf` + `observe_buf` on `stub_dispatch_observe`** (iter26 spec — the first
   genuine handler gap after 7 false alarms) — unlocks callee-stubbed setters
   (`0x004b8080`/`0x004b7fd0` Lua pair).
3. **HB-1: x87 ST0 float-return handler** — unlocks 6 named frontier rows (3 sin-getters,
   3 RwV3d bbox accessors).
4. **Mutator snapshot/restore driver** — only 2 of 44 rows are AB_READY (iter14) and both need
   a call-frequency boot first; lowest ROI.

Standing rule regardless of route: **any new/extended handler must RED on a known-wrong port
before it certifies anything.** Discovery (C0→C1/C1→C2, `discover-c1-batch`) needs no harness
and remains available as low-judgment volume work.

## 5. Open decision gates (STOP-AND-ASK — these need the user)

- **D2 — renderer commitment** (blocks all of M3). RW-subset verbatim vs `librw`.
- **D4 — airborne 1-ULP bit-identity** (U-8991): accept as C4-grounded vs naked-asm float10 shim.
- **D6 (new) — lane-B capital**: fund which of §4's items, or park lane B entirely until M2/M3
  need it.
- **D7 (new) — schedule the human tail** (D-11060/D-11061): when, and on what machine setup.

Resolved gates for the record: D1 collision Option A (07-06) → executed as B5a..B5e;
D3 MP out for v1.0 (07-11, D-11063); D5 M1-breadth-first (07-06) → M1 closed.

## 6. Operating model (unchanged in substance)

- **Two-session etiquette**: defect session = statediff/regabi lane + its four files; the
  orchestrator session takes non-colliding slices only; UNCERTAINTIES.md writes coordinate via
  the multi-session skill. Kill only your own PIDs; game runs serialize on the machine lock.
- **Account2 worker** for every read-only leg (surveys, brief screens, doc drafts) — measured
  ~$0.4–0.8/unit, zero session quota. The brief pipeline's gates are fixed (`73273edd`): the
  queue builder drops library-band / HALT-cited / already-C3+ rows and resolves plate paths
  **and callers** orchestrator-side (`function_callers`, else `reference_to` +
  `orphan_owners.tsv`; a thunk JUMP or DATA ref is not a caller).
- **Orchestrator ledger** (`re/orchestrator/state.json` via `orch.ps1`) is the source of truth
  over any handoff prose — reconcile briefed-item notes against `hooks.csv` before declaring
  anything dry (iter22 lesson).
- **Ghidra pool**: slot 9 healthy; 5/10/11 hold leaked in-JVM `.lock~` until the MCP JVM
  restarts; `acquire` stakes a `.lock` that Ghidra itself trips over — clear the stake before
  opening; never pool-wide `cleanup` with another session live.
- Token economy per CLAUDE.md: focused sessions, no raw-transcript reads, `ARG_TYPES.md`
  (never raw `diff_template.js`) for handler lookup, one-liner state queries.

## 7. Next-sessions queue (rebuilt 2026-07-31; every item leads with a worker leg)

**Non-colliding, autonomous-capable (pick per D6/priorities):**
1. **HUD sweep D-6160..D-6173** (9 HUD fns: powerup sprite, player-count, lap formatter,
   layout Y-base, slot occupancy, vehicle icon, background rect, split-check). *Worker:* map
   each RVA to the standalone's HUD implementation status. *Account3:* parity/draw-list check
   to CONFIRM the gap, then port only what is actually missing. Truest "fix the game" slice.
2. **Lane-B capital item per D6** (§4.1 is the recommended first: small, reusable, fixes a
   real bug). Must include the known-wrong-port RED proof.
3. **Plating drains** (no harness needed): D-7000..3 longjmp callees, D-0281 teardown bucket,
   D-0245..63 / D-9280 render-frame buckets, D-8140 slot reassign. Volume work for cheap models.
4. **candidate_buckets.json validity pass** (`0x004d8530` is not a function start — iter24)
   + the `0x0041c090` C1-vs-C2 plate/hooks.csv conflict → `re-classify`.

**Defect-session lane (do NOT pick up from here):** statediff residual wedge, KV C4 campaign,
WS-A8 diff.

**Coordinate-first:** VECCAP-2 `FUN_00566200` (collision math adjacency).

**User-gated:** D2 decision session (unblocks M3), D4, the human tail (D-11060/61, G3 Frida
session).

## 8. Risks

- **Doc drift** (this file's own failure mode): B5e sat "OPEN" for 11 days post-merge and a
  fresh worker survey reproduced the stale claim as fact. Mitigation: the header maintenance
  rule, and the orchestrator ledger as primary truth.
- **False-GREEN debt in the harness** (iter26): `entity_field_set` is the known instance; the
  other single-fn handlers have not been audited for the same Orig-pre-writes-shared-state
  pattern. The audit is cheap worker work — attach it to the next lane-B session.
- **Second wedge mechanism** unbisected — until it falls, full-set statediff evidence caps at
  ~5/6 confidence and C4 claims on the physics loop wait.
- **M3 size** (~770 rows) with D2 undecided — the largest avoidable token risk in the project.
