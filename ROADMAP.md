# Mashed RE Roadmap — v2 (2026-06-09)

**North-star goal:** a fully playable, standalone `mashed_re.exe` — every track, every
vehicle, every mode — booting from clean Mashed data files with zero dependence on
`MASHED.exe`, where every ported function is verbatim-verified against the original.

**v2 strategy decision (user-ratified 2026-06-09, evidence in
`re/analysis/AUDIT_2026-06-09.md`):** the project is now **standalone-first and
demand-driven**. The supply-driven coverage era is over — it completed its mission
(full inventory, first-party C1 = 0, 561-hook Frida harness, 99 arg_types) and its
yields measurably collapsed (C2→C3 batches: 57–80% → 12–21% → 8/48 ceiling). From here,
functions are reversed and ported **in the dependency order the standalone needs them**,
each port is verified with `diff-original` against the live original, and C3/C4
promotions fall out as a *byproduct* of slice work rather than being the goal. Progress
is measured in demonstrable capabilities, not coverage percentages.

Phases are gates: do not advance until the current phase's **Definition of Done** is
satisfied. Fuzzy exit criteria are not allowed.

## Definition of Done — three scopes

### F-DoD — function level (unchanged from v1)
A function is **DONE** only when ALL of these hold:
1. **RVA pinned** in `hooks.csv` against the version anchor SHA-256.
2. **Confidence ≥ C3** per `re/CONFIDENCE.md`. C4 means "verified by Frida diff with the
   hook actually installed on a canonical scenario."
3. **No `[UNCERTAIN]` markers** remain in the analysis note (resolved with evidence, or
   promoted to `UNCERTAINTIES.md` with a path-to-resolution).
4. **No stubs** — every callee resolves to a real reversed function or an
   explicitly-recorded passthrough (in `STUBS.md`).
5. **Frida diff clean** via `diff-original` on ≥ 1 canonical scenario.
6. **Hook registered** via `RH_ScopedInstall` and runtime-toggleable (dev target).
7. **Inline RVA comments** on every cross-reference.

### S-DoD — subsystem level (REVISED 2026-06-09 for demand-driven v2)
A subsystem is **DONE** when:
1. The standalone exe runs the subsystem's canonical scenario **natively** — no original
   code, no fallbacks (the standalone cannot call MASHED code by construction).
2. **Every function the standalone actually executes** in that subsystem satisfies F-DoD.
3. Functions in the subsystem that the standalone never executes are explicitly dispatched:
   each is either ported anyway (if cheap), or marked `deferred-not-needed` in `DEFERRED.md`
   with the slice that would need it. **No percentage target** — the v1 "≥90% C3+ / ≥50% C4"
   gate is retired; ladder percentages are diagnostic, not gates.
4. Shared structs documented field-by-field in `re/analysis/structs/` with RVA citations.
5. Custom asset formats have round-trip parser+writer tools in `re/tools/`.
6. The subsystem's `STUBS.md` section is empty.

### P-DoD — project level (unchanged in substance)
1. Every subsystem S-DONE.
2. A clean playthrough of every track + vehicle + mode on `mashed_re.exe` alone.
3. All trackers have zero rows or only `wontfix`/`deferred-not-needed` rows with rationale.
4. The dev `.asi` harness is dropped from the shipping build matrix.

## v1 phase ledger (closed 2026-06-09)

| v1 phase | Status |
|---|---|
| 0 Bootstrap | DONE 2026-05-02 |
| 1 Surface mapping | DONE 2026-05-20 (100% inventory, 83.3% C1+, subsystem map, skeleton call tree) |
| 2 Asset formats | Substantially done (piz round-trip, TXD decoder, gamesave/videocfg tools, RWS walker); remaining formats (track/vehicle geometry, .AI scripts) absorbed into Phase R3 |
| 3 Engine boot path | Superseded — boot subsystem at C2+ end-to-end; standalone boots natively |
| 4 Dev harness + skeleton | DONE (both targets build; install-observe canonical C4 harness validated 2026-06-05; the "blocked-on-runtime" text was stale — boot-to-menu fixed 2026-06-01) |
| 5 Subsystem sweeps | Retired as a phase shape. C1→C2 closed (first-party C1 = 0, 2026-06-04). C2→C3 batch lane continues only as opportunistic side-lane (see Operating model) |
| 6 Standalone v1 | Re-planned as R2–R8 below |

## Phases (v2)

### Phase R0 — Repair & re-baseline (DONE 2026-06-09)
**Goal:** the repo tells the truth before new work starts.
**Executed 2026-06-09** (commits `28badb07..` on `standalone/menu-state-machine`, merged
to `main` same day; repair scripts under `scripts/r0/`, CHANGELOG `r0-repair` entries,
backups under `log/backups/`). All criteria met; two honest exceptions recorded:
PROMOTION_QUEUE rows `c3-batch-af-s1`/`af-s4` stay Queued (their claimed promotions
never landed in hooks.csv — needs a re-classify drain), and the `## camera_follow`
appendix in DEFERRED.md was left in place (labeled, out of criterion scope).
**Exit criteria:**
- `standalone/menu-state-machine` (253 commits) merged to `main`; verify screenshots cited
  by commit messages committed; uncommitted scripts/probes triaged (commit or delete).
- Tracker repair (scripted, one transaction each): STUBS/UNCERTAINTIES rows re-filed from
  after "## Conventions" into Active/Resolved sections; 80 duplicate U-IDs renumbered;
  the two DEFERRED stores unified into root `DEFERRED.md` (re/DEFERRED.md is ~95% stale —
  drop rows whose RVAs are already C2+); hooks.csv duplicate header + 23 newline-spill
  rows repaired.
- Queue hygiene: PROMOTION_QUEUE "Queued" rows reconciled against CHANGELOG (move
  provably-merged rows to Merged); 70 one-row SCRIBE_QUEUE fragments back-annotated or
  archived to `re/archive/`.
- C4 ledger honest: the 24 untagged escapees tagged `C4-EVIDENCE-SUSPECT`; stale tags on
  the 22 already-demoted rows cleared (`re/analysis/C4_REVALIDATION.md` Populations A/B/C).
- CLAUDE.md stale claims fixed (patch count/skip_powerups status, Vec3-trio framing,
  "gameplay 324 + render 458 remain" belief).
- Root clutter cleared: batch/c3_batch txts archived to `log/batches/`, .pool_slot markers
  and stale `mashed_pool` locks removed (respect the documented pool4 leak-lock), build
  logs pruned.

### Phase R1 — C4 truth (DONE 2026-06-09)
**Goal:** zero suspect C4 rows; the C4 column means what the rubric says.
**Executed 2026-06-09, same day as R0** — the standing-lane estimate proved pessimistic:
subset-install batching (12–19 hooks per canonical run) drained all 101 in one session.
R1-A static audit demoted 16 (11 no-install-site, 5 install-disabled → C2); R1-B
reconfirmed 85/85 via six subset-install canonical boot-to-menu runs (every hook
0xE9-live + manifest-confirmed + 25s survival, 0 crashes — incl. the Vec3 trio).
**C4 = 106, zero suspect tags.** Evidence `log/install_observe_r1b_20260609.txt`;
audit trail `re/analysis/C4_REVALIDATION.md`.
**Exit criteria (met):** zero `C4-EVIDENCE-SUSPECT` tags; zero C4 rows whose only
canonical evidence dates 2026-05-15..24; every row reconfirmed or demoted with a
CHANGELOG line.

### Phase R2 — Menu complete (DONE 2026-06-10)
**Phase closed 2026-06-10** (branch `phase/r2-close-r3-open`): parity sweep vs the
original DONE (`re/frida/parity_shots.py` walked the original to GTS/Options/Sound —
runtime screen ids 1/8/19 exactly match the standalone's harvested tables, item lists
and Bonus-Features grey-out match; montages `verify/parity/parity_*.png`; ranked deltas
in `re/analysis/standalone_menu_sm/PARITY_2026-06-10.md`). Settings persistence DONE
(Sound screen: LEFT/RIGHT adjusts 3 volumes + Insults toggle, widgets render, persisted
to standalone-side `mashed_re_settings.bin`; restart-load proven). 34-screen pass DONE
(33/33 non-null tables build; 10 menu-reachable fresh-state, 23 classified
state/flow-gated — consistent with the original; full graph closure lands with R5/R6
game flows).
**Goal:** the standalone main menu is pixel-faithful and fully functional.
**Work items — ALL FIVE LANDED 2026-06-09** (branch `phase/r1-r2-menu-complete`;
details in the port spec's RESUME section):
- ✅ FGDC20 extended-charset glyph table (not a 256-LUT — ext_base/ext_count/u16 map
  @0x24/0x2c/0x30 per FUN_00554390; codepoints 0x80..0xFF resolve; commit `c3152314`).
- ✅ badges.txd named-sprite chrome (sfx.piz dictionary, 23 textures decoded by the
  existing Txd::Dictionary; Button cap drawn on the highlight bar; commit `67be20d6`).
- ✅ Frame-rate-scaled anim timing (verbatim FUN_00493480 clock + FUN_004325c0 tick;
  commit `e78a2435`).
- ✅ Save-driven `MenuGameState` (FUN_00404e80 span restore, DEADBEEF-gated; commit
  `07432ff7`).
- ✅ Animated logo overlay (verbatim FUN_00473ee0/4733b0/473220 — the wavy grid IS the
  menu's animated checkered-flag chrome; commit `61c97421`).
- (The 06-08 "window hang" was root-caused and fixed in `f61caf62` — explanation
  back-ported to the spec.)
**Exit criteria (phase close, still open):** side-by-side screenshot parity vs original
on the canonical screen set; all 34 screens reachable; settings screens actually mutate
persisted state. Logo animation: done (checkered chrome animates at the original's
2/3 rad/s).

### Phase R3 — Track & vehicle data foundation (DONE 2026-06-10)
**Goal:** every asset format the game world needs is parsed, documented, and dumpable.
**Opener LANDED 2026-06-10: the track world geometry is CRACKED.** `GRAPH*.BSP` is a
standard RW 3.6 world stream; `re/tools/track_dump.py` parses + validates **13/13**
tracks (sector sums == header totals, all indices in range, triangle order determined
empirically) and exports OBJ + wireframe PNG. Proof: `verify/r3/arctic_wireframe.png`
(the Arctic arena — island + bridge — clearly recognizable). Format doc with
byte-offset tables: `re/analysis/formats/track_world_bsp.md`.
**Remaining activities:** COLLISIONS/COLLIDE.BSP (collision world), AI*.BSP graph,
`.SPL` splines / `.MTS` material scripts / `.ANM` anims, material→TXD texture binding
(needed for textured R4 rendering), `VEHICLES/*.piz` DFF model parsing, `.AI` script
format (cross-ref MashedFileExtractor), format-flag bit semantics.
**Phase closed 2026-06-10 (same day):** collision worlds = the SAME RW world stream,
validated 13/13 (untextured materials = surface types); material→TXD binding cracked
(MATERIAL→TEXTURE→STRING; TXD naming + version variants handled; binding sweep 13/13);
vehicle DFF parsed (`re/tools/dff_dump.py`, ADVANTAGE0: 83 frames / 71 atomics /
15,358 tris validated → OBJ; doc `re/analysis/formats/vehicle_dff.md`).
**Exit criteria:** ✅ track geometry parsed+viewable; ✅ vehicle model parsed; ✅ format
docs (world+collision+binding, DFF); round-trip writers deferred (no rewriting use-case
yet — recorded in the format doc). Remaining loose ends (AI*.BSP, SPL/MTS/ANM,
multi-TXD lookup) move into R4/R5 demand-driven scope.

### Phase R4 — World render (OPENED 2026-06-10 — opener landed)
**Opener LANDED 2026-06-10:** the Arctic track renders TEXTURED in `mashed_re.exe`
with an auto-orbit fly-through camera (`MASHED_TRACK_VIEW=1`): `Track/TrackWorld` C++
parser (same validations as the Python tool) + `D3d9Render/TrackRenderer` spike
(per-material batches, baked prelight as vertex diffuse, PAL4/PAL8/32bpp textures,
hand-built matrices, depth buffer added to the device). Proof:
`verify/r4/arctic_fly_*.png` — island, dirt circuit and the suspension bridge
unmistakable. The spike deliberately prejudges nothing: the parsed world is
renderer-agnostic and the architecture gate below stays open.
**Opening gate — RATIFIED 2026-06-10 (re/analysis/RENDERER_GATE_BRIEF.md):** the D3D9
spike stays the DEV VIEWER; the shipping renderer is the **RW-subset verbatim port**,
demand-driven (vehicle lighting/material first); **librw is the fallback** only if the
RW core band proves impractical to port.
**Goal:** the standalone renders a full track in 3D with a controllable fly-through camera.
**Exit criteria:** track renders with textures; camera controllable; visual parity vs an
original-game screenshot of the same viewpoint (lighting deltas allowed and documented);
stable frame rate at 800×600.

### Phase R5 — Drivable car (OPENED 2026-06-10 — kinematic opener landed)
**Opener LANDED 2026-06-10 (branch `phase/r5-car-on-track`):** the Advantage car
DRIVES on Arctic inside `mashed_re.exe` — C++ DFF clump parser (`Track/DffModel`,
librw-cross-referenced, frame transforms baked to model space), vehicle TXD textures,
collision-world ground raycast (`COLLI*.BSP` triangle soup), kinematic drive model
(accel/drag/speed-scaled steering, ground snap, island-edge stop), chase camera, and a
visible-ground spawn scorer (Arctic's frozen bay is collision-only — its SEA.DFF visual
is an unrendered prop). `MASHED_CAR=1 MASHED_DRIVE_DEMO=1`; proof
`verify/r5/car_1_spawn.png` (the car on Arctic's lit main street) + 60-unit
ground-snapped drive telemetry. R4 also gained track-by-name loading, a free
WASD/mouse camera and multi-TXD material lookup (dump.piz 38/40 textured).
**The kinematic model is explicitly NOT the ported physics** — it is the scaffold the
real FUN-ported physics/handling will replace (the physics decision gate below).

**Goal:** a vehicle on a track under player control.
**Activities:** vehicle model load/render; **physics decision gate** (the original vendors
RenderWare Physics 3.7 + qhull-2002.1 — decide reimplement-the-used-subset vs vendor real
qhull + reconstruct the RW-Physics call surface); input (DirectInput path already C2/C3)
→ physics tick → camera follow (camera subsystem); track collision.
**Exit criteria:** drive a full lap with keyboard/pad; collision with track boundaries
works; camera follows faithfully; physics behavior diff-checked against original telemetry
(Frida trace of the original race vs standalone logs on matched inputs where feasible).

### Phase R6 — Race loop ★ flagship demo (CLOSED 2026-07-06; exit demo passed 2026-07-02)
**Landed 2026-06-10 (verbatim ports, re/analysis/race_camera/):** the shared race
camera (FUN_00446520 + Common/LED.piz per-gate angles — format cracked), the real
elimination rule (camera required-zoom saturation at 10.0, FUN_00410d10), real car
selection (liveries of one model, FUN_0040d110), and the points system + match rule
(FUN_0040eee0 / 0x00410510 — 4P awards −2/−1/+1/+2, match at score>11). Divergence
ledger #3–#6 CLOSED (re/analysis/DIVERGENCE_LEDGER_3D.md). Remaining in-phase:
handling/AI verbatim ports (needs the in-race input injector), the standings-drawer
presentation, the vehicle-lighting consumer (entry points pinned, ledger #9).
**Landed 2026-07-02 (WS-R6 final drain):** the .asi-side AI control chain is fully
C3 — control steps FUN_00416250 / FUN_00416a30 / FUN_00417da0, bank switcher
FUN_00417180, pre-tick FUN_004177b0 (Ai/AiControlStep.cpp + Ai/AiPreTick.cpp;
snapshot/restore A/B drivers, 0 raw + 0 confirmed mismatches across ~35k paired
calls; log/ai_ab_*.log). Exit criterion demonstrated end-to-end on mashed_re.exe
(automated MASHED_RACE_DEMO driver, 2026-07-02 15:31): boot → menu → Challenge
Select (default selection) → race vs 3 AI → 7 elimination rounds → match won by
car1 at t=62.7s (score>11 rule) → results + progression → back to menu → clean
exit. Evidence: verify/r6/round{1..7}_{go,result}.bmp + match_result.bmp/png +
verify/race1/00_results.bmp/png + mashed_re.log R6 telemetry. Literal-criterion
residue: an interactively-navigated track/car selection and a continuous screen
recording (the automated run parks on Challenge Select via MASHED_GOTO=6 and
captures per-stage frames instead) — filed as D-11060/D-11061 on phase close
2026-07-06 (re-pickup: M1 breadth pass / next unlocked-desktop verification
session). Note: the standalone races on the WS-C
faithful-lookahead AI (AiStandalone.cpp); the newly-C3 chain is the verbatim
.asi reference for its convergence.
**Goal:** complete a full race against AI in the standalone.
**Activities:** race state machine (countdown/laps/checkpoints/finish — gameplay subsystem's
C2 pool becomes the demand-driven port list); in-race HUD; minimal AI opponents (`.AI`
scripts from R3 + ai subsystem); Mashed's signature camera-pull elimination rule.
**Exit criteria:** boot → menu → select track/car → finish a race vs AI, end-to-end on
`mashed_re.exe` alone, screen-recorded.

### Phase R7 — Full game systems (ACTIVE since 2026-07-06 — scaffold→verbatim conversion; route: re/analysis/RE_MASTER_PLAN_2026-07.md)
**Scaffold landed 2026-06-15/16 (SCAFFOLD, not verbatim):** the full race loop,
results/progression, elimination+laps modes, particles, pickups (real
POWERUPS_GOLD placement), power-up effects, and **real audio** (both RWS formats
cracked — ambient/music/engine/menu+race SFX). See the **Completion plan**
section below for the session-by-session breakdown of converting these scaffolds
to verbatim + the remaining data/mode/verify work (WS-A..K).
**Goal:** everything else the menu promises actually works.
**Activities:** all party/battle modes; powerups; particles; audio (RWS playback — audio
subsystem is 510 C2 / 74 C3 already); save/unlock flows; remaining menu paths wired to real
subsystems; polish. Descoped 2026-07-11: video playback (deferred-not-needed, D-11062) and
multiplayer-local split-screen (OUT for v1.0, gate D3 → D-11063).
**M1 status:** the RE_MASTER_PLAN M1 breadth tail (9 items) CLOSED 2026-07-13; next per gate D5
is the M2 opener. **WS-PHYS-DRIVE-STABILIZE diagnosis DONE 2026-07-14** (root cause = the missing
system-2 two-body proxy loop; the fix is not a separable pre-B5 step but B5a..B5e itself — see
RE_MASTER_PLAN §7 item 10 + `INITD3D9_HANG_AND_REMEASURE_2026-07-14.md`). **Active next: B5a**
(system-2 call-surface plating) → B5b..B5e → A8 true-diff + steer-calib.
**Exit criteria:** every menu path functional; every mode playable; S-DoD for every
subsystem.

### Phase R8 — v1.0 ship
**Exit criteria:** P-DoD. Clean full playthrough; trackers zero-or-wontfix; dev harness
stripped from shipping build; packaging + README for clean-install users.

## Operating model — the Fable-5 era

How we work changed with the model upgrade; the machinery built for smaller models is
kept but demoted:

1. **Demand-driven RE.** Pick functions by what the current slice needs next (call-graph
   from the slice entry point), not by C-level lists. The 3,122-row first-party C2 pool is
   a *menu*, not a queue.
2. **Deep solo sessions by default.** One session takes a whole function cluster / port
   spec end-to-end (the menu-SM port proved multi-KB verbatim ports land in days). The
   6-way fanout + sweep machinery (`discover-c1-batch`, `promote-c3-batch`, sweeps) is
   reserved for genuinely mechanical, parallel work — do not run a batch whose predicted
   yield is under ~30%.
3. **Verification as byproduct.** Every port goes through `diff-original` against the live
   original via the dev `.asi`; `re-classify` records the resulting C3/C4. The dev-harness
   lane (MASHED.exe + Frida) stays alive for exactly this purpose until P-DoD.
4. **Harness capability work is first-class** — it is the only measured high-yield C3 lever
   (harness-ext 8/9, scenario-attach 8/8). Scenario-attach with live race state is also the
   verification vehicle for R5/R6 physics/gameplay ports. Extend the harness when a slice
   needs it, not speculatively.
5. **KPIs:** capability milestones shipped (per phase exits), verified ports per week,
   C4-suspect burn-down (R1), and screenshot/recording artifacts in `verify/`. Ladder
   percentages are reported but never gate.
6. **Rituals kept:** trackers mutate only via `re-classify`; ROADMAP.md updated at every
   phase boundary (the v1 ritual that lapsed — this rewrite is the correction); commit at
   every working state; stop-and-ask on architecture gates (R4 renderer, R5 physics).

## Sizing reality check (revised)

(Counts below are the 2026-06 snapshot; current numbers live in RE_MASTER_PLAN_2026-07.md §1 —
as of 2026-07-14: 5,895 rows, C3+ = 1,013.) First-party surface is ~3,649 functions. The
demand-driven model means we do **not** need every remaining C2 — only the executed subset,
ported in slice order. The expensive unknowns are concentrated, not spread: renderer (R4) and physics
(R5) are the two architecture cliffs; everything else is the proven port-verify-screenshot
loop. Horizon to P-DoD remains 12–24 months solo. Don't promise dates — promise phase exits.

---

# Completion plan — finishing the logic, session-by-session (2026-06-16)

## Current state (2026-06-16): the SCAFFOLD loop is complete end-to-end
A full playable loop runs in `mashed_re.exe`: boot → faithful menu → select
track/car → load any of 13 real tracks → **drive** (human control) → AI opponents
→ collision (ground) → race camera → HUD → pickups (real `POWERUPS_GOLD.LUA`
placement) → power-up effects → particles → **real audio** (ambient + music +
engine + menu/race SFX; both RWS formats cracked: 0x809 wave bank + 0x80d IMA
ADPCM) → elimination/laps modes → results screen → progression (persists).

**What that means for the plan:** breadth is done — every subsystem *exists*. The
remaining work is almost entirely **converting SCAFFOLD → VERBATIM** (item 1
gameplay + item 2 renderer) plus a finite tail of completable data/mode/verify
work (items 3–5). The classification of what is scaffold vs verbatim vs
data-verified is in `re/analysis/SESSION_VERIFICATION_AUDIT_2026-06-16.md`.

## How to run these as separate sessions
Each **WS-x.n** below is sized to fit one session (finishes before auto-compact)
and is self-contained. Per session: take a **worktree** bound to a **Ghidra pool
slot** (`worktree` + `ghidra-pool` skills; pre-assign the slot in the prompt —
[[feedback-pool-slot-in-prompts]]), do the RE+port, verify, `re-classify`, commit,
and `program_close` (lock hygiene). Coordinate via the `multi-session` skill.
**Verification gate:** every verbatim port lands a `diff-original` C4 (the
boot-original lane — boot AV is fixed); scaffolds that aren't 1:1 ports stay
data-verified. The scaffold stays live until its verbatim replacement is C4.

## Workstreams (the missing logic)

### WS-A — Vehicle physics (item 1; the biggest; mostly SEQUENTIAL within)
Replaces TrackRenderer::UpdateCar kinematic scaffold with the RW-Physics rigid-
body+wheel sim. Foundation: `re/analysis/vehicle_physics_cluster.md`.
- **A1** Map the full ~0xd04 vehicle struct (all fields) → `re/analysis/structs/`.
- **A2** Port/verify the RW math primitives the cluster uses (FUN_004c3df0
  transform, 004c4d20 matrix, 004c3ac0 length, 004c39b0 normalize) — extend
  Math/. (Parallel-safe; prereq for A5/A6.)
- **A3** Port the vehicle init/spawn chain (allocates+fills the struct from the
  DFF/handling data) — unmapped; find it from the spawn call site.
- **A4** Port FUN_00470670 (control input → drive/steer force) verbatim.
- **A5** Port FUN_0046ddb0 (per-wheel contact, drive/suspension/friction torque,
  ang-vel integrate) — the core; depends on A2 + WS-B contacts.
- **A6** Port FUN_00467650 + FUN_00468980 (the 2nd/3rd integration steps).
- **A7** Harvest every DAT_005c/005cea tuning constant the cluster reads.
- **A8** Wire the ported cluster into the standalone car; `diff-original` the
  per-frame velocity/pos vs the original on matched inputs (C4).

### WS-B — Collision / RW-Physics (item 1; prereq for WS-A handling)
The per-wheel + car↔car contacts feed WS-A. The original vendors RW-Physics 3.7 +
qhull-2002.1 ([[qhull-rwphysics-island]], 0x57c5b0..0x5a5820).
- **B1 RESOLVED 2026-07-06 — OPTION A (user-ratified).** Contact half was ratified
  Option B 2026-06-16 and is built (B2/B3 C2, self-tests PASS); the D1 spike
  (`re/analysis/D1_SPIKE_PROXY_BYPASS_2026-07-06.md`) proved the RW-Physics
  proxy-body world ("system 2": 0x0047eb30 driver + 0x55-band integrator + 4 qhull
  hull bodies) is LOAD-BEARING for rendered motion (2/2 terminal wedge + halved
  heading response when bypassed). Decision: **vendor qhull-2002.1 (public domain,
  load-time only) + reconstruct system 2's call surface** — new lane **B5** below.
  Brief: `re/analysis/COLLISION_GATE_BRIEF_D1_2026-07.md`.
  **B2** car↔world contact query. **B3** car↔car contacts.
  **B4** wire as the contact source for WS-A; diff vs original contact telemetry.
- **B5 (system-2 reconstruction, Option A):** B5a demand-closure + plating of what
  0x0047eb30/0x0047f840 actually reach in the 0x55 band (+ island caller closure,
  brief Open-Unknowns #3/#4); B5b vendor qhull-2002.1 + FUN_0057ca30 bridge + 4-body
  build chain (FUN_0047d3c0→FUN_004826d0→FUN_00481e00); **B5c DONE 2026-07-15** —
  per-tick integrator SUBSET ported (FUN_0057c210/0055deb0/0055ac00/0055b800/0055dff0/
  0055c000/0055e200/0047ea40 → Collision/RwpIntegrator.cpp), 4 build TUs wired into
  build.bat both targets, cone-table [UNCERTAIN] closed (_DAT_005cf240 = double 120.0,
  no writer), canonical-race acceptance clean (the full 12-stage FUN_0047e9c0 solver
  island is DEFERRED to a new lane); **B5d next** — coupling bridge 0x0047eb30 verbatim
  (drives the B5c subset; gives the C4 body-state diff); B5e wire into the standalone +
  re-run the drive-hold harness expecting wedge-free sustained drive, then A8 true diff.

### WS-C — AI drivers (item 1)
Replaces the gate-ribbon lane-follower scaffold. Real = **FUN_00418860 family**
(per-frame tick → per-vehicle decide/steer/throttle → synthetic input). [Corrected
2026-06-16: the earlier "FUN_0040e480 family" was a mis-citation — 0x0040e480 is
`CarSlotStateSet`, a frontend leaf the tick loop only pokes; root is FUN_00418860.]
- **C1** RE the AI controller cluster (per-driver decide/steer/throttle) — **DONE
  2026-06-16**, blueprint at `re/analysis/ai_controller.md` (cluster already C2-mapped;
  consolidated into one port-ready spec).
- **C2** Port verbatim. **C3** wire + diff-original on a canonical race.

### WS-D — Power-up effects (item 1)
Replaces the invented boost/shield/missile/mine/shock. Real = the **FUN_0045bba0
dispatcher + 9-entry type table @0x005f9998** family (+ the 12 real types in
POWERUPS_GOLD.LUA). [Corrected 2026-06-16: the old "FUN_00430670" was a mis-citation
— that RVA is a player-slot resolver.]
- **D1 DONE 2026-06-16** — full map in `re/analysis/structs/powerup_system.md`
  (dispatcher, slot struct @0x0088fbe0, type table + per-type effect-fn RVAs,
  lifecycle helpers, input bridge). Scaffold held-type name made data-faithful.
- **D2** Port per-type — GATED on (a) a Ghidra fn-split of 0x453f60–0x45be81 (the
  effect cluster is one merged fn; the table targets aren’t individually defined),
  (b) WS-A1 vehicle struct, (c) WS-B collision, (d) WS-E RW scene-graph. Start with
  low-dep area effects (OIL 0x457800 / FLASH 0x454db0); defer projectiles to WS-B.
- **D3** wire to the held-pickup + collision; diff.

### WS-E — Renderer: RW-subset verbatim port (item 2; large; partly PARALLEL)
Gate already RATIFIED (RENDERER_GATE_BRIEF.md: RW-subset port, librw fallback).
Replaces the D3D9 spike.
- **E1** RE + port the RW world render path (RpWorld sector render, atomic/clump).
- **E2** Material system + multi-TXD binding (verbatim) + the real draw order.
- **E3** **RpWorld lighting** (replaces the flat-shading approx) + vehicle
  lighting consumer (ledger #9). **E4** RW immediate-mode / 2D (HUD/menu) path.
- **E5** wire + screenshot parity vs original viewpoints.
(E1–E4 can split across sessions; E is independent of WS-A/B/C/D.)

### WS-F — Data formats (item 3; all PARALLEL, completable, no Ghidra)
Self-contained parsers like the audio/powerup wins; verify against asset bytes.
- **F1** `.SPL` splines (waves/water paths). **F2** `.ANM` anims (helicopter/
  cameraman flythrough). **F3** `.UVA` UV-anim dictionaries (sea/water scroll).
  **F4** `LAPDATA.LUA` lap lines (real checkpoints/lap counting). **F5** `.MTS`
  material scripts (if any binding still missing).

### WS-G — Modes & frontend completeness (item 4; mostly PARALLEL)
- **G1** RE the game-mode-id → rules table (DAT_0067e9fc switch) → replace the
  scaffold elim/laps env mapping with the real per-mode rules.
- **G2** Wire each real mode (Championship/Wreckin'/Gladiator/Time-Trial/Bonus).
- **G3** Cup PLACE-NAMES: Frida-capture runtime DAT_008a94f0 + DAT_0067ecbc from
  the booted original (cup structure already RE'd) → real names + membership.
- **G4** Remaining menu screens/leaves still scaffolded; full save/load + options
  (controls/video) screens. **G5** Full gamesave serialization (FUN_00404e80
  write side) → replace the sidecar progress store.

### WS-H — Verification debt / C4 lane (item 5; ONGOING, runs alongside)
- **H1** Stand up the boot-original `diff-original` lane for this era's verbatim
  pieces (RaceCamera 0x00446520, scoring trio, RW-math leaves) → first C4s.
- **H2..** As each WS-A/C/D/E port lands, diff-original it (C4) and move it
  scaffold→verbatim in the audit. Audit: SESSION_VERIFICATION_AUDIT_2026-06-16.md.

### WS-I — Multiplayer (later; R7 tail)
Split-screen render + 2nd input + the MP frontend screens (the 4-car race already
exists). **Netcode/online = out of scope for v1.0 (wontfix until a contributor).**

### WS-J — Audio remainder (small; PARALLEL)
Exact vehicle→character engine-bank map (6 char banks vs 12 vehicles), remaining
permdict FX wiring (skids/impacts on real collision events — needs WS-B), music
state transitions (menu vs race vs results).

## Dependency graph + suggested parallelization
```
WS-B (collision) ──┐
WS-A2 (RW math) ───┼─> WS-A (physics) ─> WS-A8 diff ─┐
                   │                                  ├─> race feel faithful
WS-C (AI) ─────────┘                                  │
WS-D (powerup effects) ───────────────────────────────┘
WS-E (renderer) ── independent, large, own track
WS-F (data) ── all parallel, independent, cheap
WS-G (modes/frontend) ── mostly parallel (G3 needs Frida)
WS-H (verify) ── continuous, gates everything
```
**Run in parallel now (independent, no shared cliffs):** WS-F (any), WS-G1/G2,
WS-E1, WS-B1 (the architecture gate), WS-A1/A2. **Serialize:** WS-A core (A3→A8)
after A2+B; WS-J impacts after WS-B. **Critical path to "faithful race":**
WS-B → WS-A → WS-A8, with WS-C/WS-D folded in. **Critical path to "faithful
render":** WS-E. These two are the long poles; everything else is the
proven parse/port/verify loop and can be filled by parallel sessions.

## Definition of "all the logic written"
Every WS-A..G item ported + `diff-original`-C4 (item 5), the scaffolds removed,
and a clean full playthrough (P-DoD §R8). Order phases, not dates.
