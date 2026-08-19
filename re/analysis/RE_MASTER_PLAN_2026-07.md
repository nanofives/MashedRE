# Mashed RE — Master Execution Plan (RECONCILED 2026-08-18)

Companion to `ROADMAP.md` (**v3, 2026-08-15, phases D0–D5**). ROADMAP defines the *gates*; this
plan defines the *route*. **This is a reconciliation** of the 2026-07-31 rebuild against ROADMAP v3
(prior text in git history — `git log -p re/analysis/RE_MASTER_PLAN_2026-07.md`). The 2026-07-31
doc had drifted onto a superseded phase model: it called itself a companion to ROADMAP v2's R0–R8
and claimed "Active phase R7 (R0–R6 closed)" — both repudiated by ROADMAP.md §"Roadmap — v3"
(v2 superseded and archived) and ROADMAP.md §"Phase ledger — corrected" (v2's own text marks R4/R5
OPEN; v3 records them open; there is no R7). Per-item history lives in `re/analysis/CHANGELOG.md`;
this doc is the strategic index only.

> Phase model: **v3 D0–D5**. D0 "tell the truth again" DONE 2026-08-15 (ROADMAP.md §D0, items
> D0.1–D0.8); its linkage item **D0.7 answered 2026-08-18** (ROADMAP.md §D0 item 7). The active
> long poles are **D1 default renderer** and **D2 default physics**, independent of each other.
> The old R-phases and the M1–M4 milestone names in §3 are superseded — read them mapped onto
> D-phases (M2 → D1+D2, M3 → D1, M4 → D5).
> Governing rule (ROADMAP.md §"The default-build rule" + the D0.7 linkage clause): **a capability
> counts only if it runs in `mashed_re.exe` with no `MASHED_*` var set AND its TU is linked into
> the exe.** An env-gated opt-in path is a fallback, i.e. not landed.
> §1 and §3 below are the **pre-D0 snapshot (2026-07-31)**, kept for history; where any number
> conflicts with ROADMAP's "Honest baseline (2026-08-15)", the ROADMAP figure governs.
> Maintenance rule: refresh §1 + §3 after each merged lane; a claim contradicted by ROADMAP or
> CHANGELOG is a bug in THIS file — fix it the day it is noticed.

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

### M3 — "Shipping render via librw" — D2 DECIDED 2026-07-31
**Gate D2 resolved (user): librw is the shipping renderer**, reversing the 2026-06-10
RW-verbatim ratification (~770 rows + ~217 stubs of avoided batch work). Verbatim RW ports
continue only demand-driven where a behavior diff requires them. New lane (ROADMAP §WS-E,
redefined): E1' vendor+build librw → E2' feed it from our renderer-agnostic loaders →
E3' viewpoint-parity pass with documented deltas → E4' verbatim islands only on behavioral
parity failures. **First step is a sizing session** (librw x86/MSVC fit, TXD/DFF bridging
surface) — the lane is new and unsized.

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

## 5. Decision gates — ALL FOUR RESOLVED 2026-07-31 (user-decided, same session)

- **D2 — RESOLVED: librw is the shipping renderer.** Reverses the 2026-06-10 RW-verbatim
  ratification; verbatim RW ports become demand-driven behavior islands only. M3 redefined
  (§3); ROADMAP §WS-E redefined (E1'–E4'); memory + gate brief note the supersession.
- **D4 — RESOLVED: the A5 airborne 1-ULP float10 residual (U-8991) is accepted as
  C4-grounded.** No float10 shim. Consistent with the angular-field ULP floor already
  accepted in the B5 verify campaign. U-8991's close goes through `re-classify` after
  multi-session coordination (UNCERTAINTIES.md is the defect session's file).
- **D6 — RESOLVED: fund the `entity_field_set` per-side sentinel fix first.** Smallest
  capital; fixes a shipped false-GREEN (iter26) and unlocks the strided-global-setter
  class. Acceptance for the fix itself: a known-wrong (non-writing) port must RED.
  The other §4 items stay ranked but unfunded.
- **D7 — RESOLVED: the human tail runs SOON — the next session the user is present.**
  D-11060 interactive playthrough + D-11061 full-loop recording (+ the G3 cup place-names
  Frida session while hands-on). Accepted cost: repeating the verification pass at ship.
  Prep checklist for that session: patched boot per BOOT_PATCHES.md, d3d9 shim deployed,
  unlocked desktop, no intro-minimize, kill-by-PID hygiene.

Earlier gates for the record: D1 collision Option A (07-06) → executed as B5a..B5e;
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

## 7. Next-sessions queue (reconciled 2026-08-18 against ROADMAP v3; every item leads with a worker leg)

D0 is CLOSED (2026-08-15, all 8 items; D0.7 answered 2026-08-18 — ROADMAP.md §D0). The two long
poles, **D1 (default renderer)** and **D2 (default physics)**, are independent; either can advance.
Priority order:

**D1 — default renderer (invert `MASHED_RENDER_LIBRW`). The renderer A/B is CLEAN; faithfulness
adjudication is what remains:**
1. **Adjudicate librw-vs-D3D9 faithfulness against the ORIGINAL.** The A/B compared the two
   renderers to *each other* (now 16/16 ≤1.01%, `verify/d1_recheck_20260818/REPORT.md`), never to
   the original (CHANGELOG 2026-08-15 "D1 renderer inversion"). The original-side capture is now
   possible (`MASHED_CAM_POSE` + the shim's `draw3d.json`); the pose reader's +0x4c bug is fixed
   (it is a direction delta, Xbox twin 0x00446520, CHANGELOG 2026-08-16) and same-track capture is
   wired (`Campaign_SetSelectedTrackDev`; on Training, D3D9-vs-librw = 1.13%, original-vs-standalone
   = 90.15% from a higher/further eye, not a frame mismatch — a figure since further advanced by
   the 2026-08-16 mirror/lens fixes). *Worker:* collate the campose/track-watcher evidence.
   *Account3:* the paired original capture + adjudication.
2. **Confirm the once-suspected in-race divergence stays closed.** The 2026-08-15 measurement read
   `01_inrace_track` 71.61%, `01_action` 21.69%, `round2/3_result` ~69% as an accumulating renderer
   defect (CHANGELOG 2026-08-16 "CAPMODE moved to the capture SINK"). The 2026-08-18 re-measure on
   a clean rebuild shows those same shots at 0.48% / 0.01% / 0.06% / 0.10% — the accumulation was a
   scaffold FX particle defect on the D3D9 side, now cut from the default build
   (`verify/d1_recheck_20260818/REPORT.md`, control pair 16/16 byte-identical
   `verify/d1_control_20260818/REPORT.md`). Keep the gate green as librw becomes the default.

**D2 — default physics (invert `MASHED_REAL_PHYSICS`). Defect-session lane — coordinate first:**
3. **Statediff residual wedge** (~1/6 boots, second unbisected mechanism; the first, Ring5ab980
   implicit-EAX, fixed 2026-07-31 / U-6701). Caps physics C4 at ~5/6 and blocks A8 and the B5e
   C4-verify campaign (ROADMAP.md §D2).
4. **WS-A8 velocity/position diff vs original telemetry** on matched inputs, once the wedge falls.

**D3 — default AI / powerups / modes (scaffolds the default build still runs):**
5. **WS-C AI** (FUN_00418860 family). Note U-9040 (CHANGELOG 2026-08-16): both the verbatim-band
   and the pure-pursuit steer conventions are DEAD on the default path — AI is steered by
   TrackRenderer's own world-space controller — so the steer-sign contradiction cannot be resolved
   until the ControlStep path executes.
6. **WS-D powerups** (FUN_0045bba0 dispatcher + 9-entry table; gated on a Ghidra fn-split of
   0x453f60–0x45be81) and **WS-G per-mode rules.** *Worker:* map each RVA to standalone status first.

**D4 — breadth (ONLY after D1–D3):** HUD sweep D-6160..D-6173; plating drains D-7000..3 / D-0281 /
D-0245..63 / D-9280 / D-8140 (cheap-model, no harness); `candidate_buckets.json` validity +
0x0041c090 C1-vs-C2 conflict → `re-classify`; WS-F formats; WS-J audio remainder. **Linkage gap —
ANSWERED, not open:** `Save/` and `Audio/` TUs are absent from `mashed_re.exe` (0 of 17 save,
4 of 25 audio), so C4-verified save/audio hooks contribute nothing to the deliverable — but D0.7
(ROADMAP.md §D0 item 7, answered 2026-08-18) settled that this is not a build-list bug: the code is
hook-shaped (`Register` is no-op'd in the exe) and its bodies deref MASHED addresses unmapped at
base 0x10000, so bulk-linking it would grow the binary and the tracker without shipping a feature.
Add-backs are gated on NO MASHED ADDRESS IN ANY CODE PATH (batch 1 of six landed 2026-08-18); the
rest is porting work, not a counting fix. Do NOT re-open D0.7.

**Next hands-on session (human tail, D5):** D-11060 playthrough + D-11061 recording + G3 cup
place-names Frida session (prep: BOOT_PATCHES boot, d3d9 shim, unlocked desktop, no intro-minimize,
kill-by-PID). Still deferred — not yet run.

**Coordinate-first (do NOT pick up cold):** VECCAP-2 `FUN_00566200` [UNCERTAIN — status not confirmed
in the reconciliation read window]; U-8991 close via `re-classify`.

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
