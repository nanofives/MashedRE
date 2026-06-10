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

### Phase R3 — Track & vehicle data foundation (IN PROGRESS — opened 2026-06-10)
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
**Exit criteria:** ✅ one full track's geometry parsed and exported to a viewable form;
vehicle model parsed (open); `re/analysis/formats/<name>.md` per format (world done,
others open); round-trip tests where the format is rewritable (open).

### Phase R4 — World render
**Opening gate — renderer architecture decision (stop-and-ask):** evaluate with short
spikes, then the user decides between (a) vendoring **librw** (the re3 path), (b) porting
the minimal RW 3.x subset ourselves (extending the `RwIm2DBridge` fake-device approach to
3D), (c) a custom D3D9 renderer consuming RW-format data directly.
**Goal:** the standalone renders a full track in 3D with a controllable fly-through camera.
**Exit criteria:** track renders with textures; camera controllable; visual parity vs an
original-game screenshot of the same viewpoint (lighting deltas allowed and documented);
stable frame rate at 800×600.

### Phase R5 — Drivable car
**Goal:** a vehicle on a track under player control.
**Activities:** vehicle model load/render; **physics decision gate** (the original vendors
RenderWare Physics 3.7 + qhull-2002.1 — decide reimplement-the-used-subset vs vendor real
qhull + reconstruct the RW-Physics call surface); input (DirectInput path already C2/C3)
→ physics tick → camera follow (camera subsystem); track collision.
**Exit criteria:** drive a full lap with keyboard/pad; collision with track boundaries
works; camera follows faithfully; physics behavior diff-checked against original telemetry
(Frida trace of the original race vs standalone logs on matched inputs where feasible).

### Phase R6 — Race loop ★ flagship demo
**Goal:** complete a full race against AI in the standalone.
**Activities:** race state machine (countdown/laps/checkpoints/finish — gameplay subsystem's
C2 pool becomes the demand-driven port list); in-race HUD; minimal AI opponents (`.AI`
scripts from R3 + ai subsystem); Mashed's signature camera-pull elimination rule.
**Exit criteria:** boot → menu → select track/car → finish a race vs AI, end-to-end on
`mashed_re.exe` alone, screen-recorded.

### Phase R7 — Full game systems
**Goal:** everything else the menu promises actually works.
**Activities:** all party/battle modes; powerups; particles; audio (RWS playback — audio
subsystem is 510 C2 / 74 C3 already); save/unlock flows; video playback; multiplayer-local
(split-screen); remaining menu paths wired to real subsystems; polish.
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

First-party surface is 3,649 functions; 527 are C3+ today (14.4%). The demand-driven model
means we do **not** need all 3,122 remaining C2s — only the executed subset, ported in
slice order. The expensive unknowns are concentrated, not spread: renderer (R4) and physics
(R5) are the two architecture cliffs; everything else is the proven port-verify-screenshot
loop. Horizon to P-DoD remains 12–24 months solo. Don't promise dates — promise phase exits.
