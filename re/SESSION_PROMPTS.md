# Session prompts — finishing the Mashed port (paste one per session)

Generated 2026-06-16 from ROADMAP.md "Completion plan" (WS-A..K). Each block is a
self-contained Claude Code session prompt. Run independent ones in parallel; honor
the dependency graph in ROADMAP.md. Pool slots are pre-assigned per workstream
([[feedback-pool-slot-in-prompts]]); pool0/pool1 are locked — use the assigned slot.

---

## COMMON PREAMBLE (the per-session prompts assume this; it's also in CLAUDE.md)
> You are working on **Mashed RE** — a greenfield standalone port of *Mashed: Fully
> Loaded* into `mashed_re.exe` (re3/TD5RE shape). Discipline: **NO-GUESSING** — cite
> the exact RVA for every constant/offset; mark `[UNCERTAIN]` with the missing
> evidence; never invent intent. Version anchor: `original/MASHED.exe` SHA-256
> `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` — abort if it
> differs. Build with `mashedmod\build.bat` (run from repo root, PowerShell). Open
> Ghidra **read-only** via the `ghidra-pool` skill at the **pre-assigned slot**;
> always `program_close` when done (lock hygiene). Verify ports with the
> `diff-original` skill (C4); record results via `re-classify`. Commit as **nanofives**
> at every working state, ending the message with `Co-Authored-By: Claude Opus 4.8
> (1M context) <noreply@anthropic.com>`. Coordinate with the `multi-session` skill.
> The standalone's in-race code is `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.*`,
> `Race/*`, `Audio/*`, `exe_main.cpp`. The current state + scaffold-vs-verbatim
> classification: `re/analysis/SESSION_VERIFICATION_AUDIT_2026-06-16.md`.

---

# WS-A — Vehicle physics (verbatim). Pool: Mashed_pool2. Read: re/analysis/vehicle_physics_cluster.md

### WS-A1 — map the vehicle struct
> Map the full ~0xd04-byte vehicle struct used by the handling cluster. Open
> Mashed_pool2 read-only; from FUN_00470670 + FUN_0046ddb0 (and the spawn/init site),
> enumerate every field (velocity +0x9b0, forward +0x9d4, maxspeed +0x9e4, control
> slots +0x1a8/+0x26c, wheel blocks at +0x5b stride 0x31, history rings, +0x278
> contact count, etc.). Produce `re/analysis/structs/vehicle.md` (field table with RVA
> citations) + a C++ struct in `mashedmod/src/mashed_re/Vehicle/VehicleStruct.h`.
> Deliverable: the struct compiles; every field cites where it's read/written. Commit.

### WS-A2 — RW math primitives (prereq for A5/A6; parallel-safe)
> Port/verify the RenderWare math the physics cluster calls: FUN_004c3df0
> (RwV3dTransformPoints), FUN_004c4d20 (matrix build), FUN_004c3ac0 (RwV3dLength),
> FUN_004c39b0 (RwV3dNormalize). Some exist in `mashedmod/src/mashed_re/Math/` — extend
> to cover these. Verify each with `diff-original` (leaf functions, easy C4). Commit +
> re-classify each to C4.

### WS-A3 — vehicle init/spawn chain
> Find + port the vehicle init/spawn that allocates and fills the WS-A1 struct from the
> DFF/handling data (mass, wheel layout, accel/grip/suspension). Locate it from the call
> site that precedes FUN_00470670 in a race. Port verbatim into Vehicle/. Deliverable:
> a spawned struct matches the original's field values for one car (diff-original or a
> field-dump compare). Commit. (Depends: WS-A1.)

### WS-A4 — control input integrator FUN_00470670
> Port FUN_00470670 verbatim (input byte[] -> drive/steer force into the control slots).
> File `Vehicle/VehicleControl.cpp`, RVA-commented. Harvest its DAT_005c constants.
> Verify diff-original on matched input. Commit + re-classify. (Depends: A1.)

### WS-A5 — per-wheel sim FUN_0046ddb0 (the core)
> Port FUN_0046ddb0 verbatim — per-wheel RW contact query, drive/suspension/friction
> torque, angular-velocity integrate, the per-wheel loop (stride 0x31). Depends on A2
> (RW math) + WS-B (contacts). Verify diff-original of per-frame velocity/ang-vel vs the
> original on matched inputs. Commit + re-classify.

### WS-A6 — integration steps FUN_00467650 + FUN_00468980
> Port the 2nd/3rd integration callees verbatim (RVA-cited). Verify diff-original.
> Commit + re-classify. (Depends: A1, A2.)

### WS-A7 — handling constants harvest
> Harvest every DAT_005c / DAT_005cea tuning float the cluster reads (memory_read at
> each, cite RVA) into `re/analysis/structs/handling_constants.md` + a C++ consts header.
> Cross-check vs HANDLING_V2 harvested rates. Commit.

### WS-A8 — wire + diff the physics
> Replace TrackRenderer::UpdateCar's kinematic scaffold with the ported cluster
> (A3..A7). Verify the standalone car's per-frame velocity/pos matches the original on a
> matched-input canonical race (diff-original C4). Remove the scaffold. Update the
> verification audit (scaffold->verbatim). Commit. (Depends: A3-A7, B4.)

---

# WS-B — Collision / RW-Physics. Pool: Mashed_pool3. Read: [[qhull-rwphysics-island]] (0x57c5b0..0x5a5820)

### WS-B1 — architecture gate (STOP-AND-ASK)
> Decide: vendor real qhull-2002.1 + reconstruct the RW-Physics 3.7 call surface, vs
> port only the used contact-query subset. Read the qhull/RW-Physics island notes;
> enumerate the contact functions the vehicle sim calls (DAT_008815a4.. arrays in
> FUN_0046ddb0). Write `re/analysis/COLLISION_GATE_BRIEF.md` with the recommendation and
> STOP-AND-ASK the user before porting. Commit the brief.

### WS-B2 — car↔world contact query
> Port the per-wheel car↔world contact query (ground + walls) that fills the contact
> arrays FUN_0046ddb0 reads. Verify contact points vs an original Frida trace on a known
> spot. Commit + re-classify. (Depends: B1 decision.)

### WS-B3 — car↔car contacts
> Port the car↔car contact/resolution. Verify vs original on a staged collision. Commit.

### WS-B4 — wire as WS-A's contact source
> Wire B2/B3 as the contact source for the ported vehicle sim (replaces the ground-
> raycast scaffold). Diff vs original contact telemetry. Commit. (Depends: B2/B3, A5.)

---

# WS-C — AI drivers. Pool: Mashed_pool4

### WS-C1 — RE the AI controller cluster
> Open Mashed_pool4 read-only. RE the AI driver cluster (FUN_0040e480 family + callees):
> per-opponent decide/steer/throttle/target. Document call graph + struct in
> `re/analysis/ai_controller.md`. Commit the doc.

### WS-C2 — port the AI verbatim
> Port the AI cluster verbatim into `mashedmod/src/mashed_re/Ai/` (RVA-cited). Commit +
> re-classify. (Depends: C1.)

### WS-C3 — wire + diff
> Replace TrackRenderer's gate-ribbon scaffold AI with the port. Diff-original on a
> canonical race (AI car positions over N frames on matched seed). Remove scaffold.
> Commit. (Depends: C2, ideally A8.)

---

# WS-D — Power-up effects. Pool: Mashed_pool5

### WS-D1 — RE the power-up dispatch + per-type effects
> Open Mashed_pool5 read-only. RE FUN_00430670 (power-up dispatch) + the per-type effect
> functions for the 12 real types (MISSILE/MINE/OIL/SHOTGUN/FLASH/R_FLAME/MORTAR/GUN/
> DRUM/DETONATOR/P_MINE/BLANK — ids in POWERUPS_GOLD.LUA). Doc:
> `re/analysis/powerups.md`. Commit.

### WS-D2 — port per-type effects
> Port the dispatch + each effect verbatim into `mashedmod/src/mashed_re/Powerup/`.
> Commit + re-classify. (Depends: D1.)

### WS-D3 — wire + diff
> Replace the scaffold boost/shield/missile/mine/shock with the ported effects, driven
> by the held pickup (PickupField) + collision events. Diff-original. Remove scaffold.
> Commit. (Depends: D2, B4 for hit detection.)

---

# WS-E — RW-subset renderer (verbatim). Pool: Mashed_pool6. Read: re/analysis/RENDERER_GATE_BRIEF.md

### WS-E1 — RW world render path
> RE + port the RW world render (RpWorld sector render + atomic/clump render) that draws
> GRAPH*.BSP. Replace the D3D9 spike's world draw in TrackRenderer. Doc the pipeline in
> `re/analysis/formats/rw_render_pipeline.md`. Screenshot-parity a track vs the original.
> Commit. (Independent of WS-A/B/C/D.)

### WS-E2 — material system + multi-TXD + draw order
> Port the RW material system + multi-TXD binding + the real draw order (opaque/alpha
> passes) verbatim. Parity vs original. Commit. (Depends: E1.)

### WS-E3 — RpWorld lighting + vehicle lighting (ledger #9)
> Replace the flat-shading approximation (ApplyCarLighting) with the real RpWorld
> lighting + the vehicle-lighting consumer (DIVERGENCE_LEDGER_3D #9). Parity vs original
> car/track lighting. Commit. (Depends: E1.)

### WS-E4 — RW immediate-mode / 2D (HUD + menu)
> Port the RW Im2D path the HUD/menu use (the current bridge is a scaffold). Parity vs
> original menu/HUD draw. Commit. (Depends: E1.)

### WS-E5 — wire + parity sweep
> Wire E1-E4 as the shipping renderer (spike becomes dev-only). Screenshot-parity sweep
> over track + menu + race viewpoints vs original. Commit.

---

# WS-F — Data formats (parallel, completable, no Ghidra). Pool: none

### WS-F1 — .SPL splines
> Crack the `.SPL` spline format (Arctic WAVE.SPL etc.). Write `re/tools/spl_dump.py`
> (parse + dump) + a C++ parser; wire wave/water spline geometry into TrackRenderer if
> used. Verify parse vs bytes (like the RWS audio crack). Commit.

### WS-F2 — .ANM animations
> Crack the `.ANM` format (H_Anim_*.anm, KTC1.anm — the helicopter/cameraman flythrough).
> `re/tools/anm_dump.py` + C++ parser; play the copter anim on the track. Verify. Commit.

### WS-F3 — .UVA UV-anim dictionaries
> Crack the `.UVA` format (Sea.uva — UV scroll for water/sea). Parser + wire the UV
> animation into the material system. Verify. Commit.

### WS-F4 — LAPDATA.LUA lap lines
> Parse `LAPDATA.LUA` (Lap_Line definitions) for the real lap/checkpoint lines; replace
> the gate-0-crossing lap heuristic with the real lap-line crossing test. Verify lap
> counts vs the LUA. Commit.

### WS-F5 — .MTS material scripts (if any binding still missing)
> Audit MTS material-script coverage; crack/parse any remaining `.MTS` not yet bound.
> Verify textured render. Commit. (Skip if R3 already covers it.)

---

# WS-G — Modes & frontend completeness. Pool: Mashed_pool7

### WS-G1 — game-mode-id -> rules table
> Open Mashed_pool7 read-only. RE the game-mode rules: how DAT_0067e9fc (GetRaceSubMode
> 0x0042f6a0) selects race rules (laps vs elimination vs battle...). Replace the scaffold
> MASHED_RACE_MODE env mapping in RaceSession/GameFlow with the real per-mode rules. Doc:
> `re/analysis/game_modes.md`. Commit.

### WS-G2 — wire each real mode
> Wire each real mode (Championship/Wreckin'/Gladiator/Time-Trial/Bonus) to its rules +
> the frontend selection. Verify each launches with the right rules. Commit. (Depends: G1.)

### WS-G3 — cup place-names (Frida capture)
> The cup track names (DAT_008a94f0) are RUNTIME-populated (cup structure already RE'd —
> DAT_0067ecbc membership, FUN_0040b6c0 name-msgid getter). Use Frida on the booted
> original to capture DAT_008a94f0 + DAT_0067ecbc at the Challenge Select screen; map
> track-idx -> name msgid -> string. Wire real place-names + cup membership into
> Race/GameFlow. Verify vs the original screen. Commit. (Needs the booted original; mind
> the GPU-wedge — see CLAUDE.md compat notes.)

### WS-G4 — remaining menu screens + options
> Wire the still-scaffolded menu leaves + full save/load management + controls/video
> options screens to real state. Parity vs original. Commit.

### WS-G5 — full gamesave serialization
> Port the real gamesave write side (FUN_00404e80 read side is ported; find + port the
> writer). Replace the sidecar `mashed_re_progress.bin` with the real gamesave.bin format
> (write to a standalone copy, NEVER original/gamesave.bin). Verify round-trip. Commit.

---

# WS-H — Verification / C4 lane (ongoing, runs alongside). Pool: dev .asi + Frida

### WS-H1 — boot-original diff-original lane
> Stand up the boot-original `diff-original` lane for this era's verbatim pieces:
> RaceCamera (0x00446520), the scoring trio (FUN_0040eee0/FUN_0040b290/0x00410510), the
> RW-math leaves. Boot the original (boot AV is fixed), run run_diff.py per hook, record
> C4 via re-classify. Update SESSION_VERIFICATION_AUDIT. Commit.

### WS-H2+ — C4 each port as it lands
> As each WS-A/B/C/D/E verbatim port lands, diff-original it (C4) and move it
> scaffold->verbatim in re/analysis/SESSION_VERIFICATION_AUDIT_2026-06-16.md. (Run inside
> each WS session, or as a follow-up sweep.)

---

# WS-J — Audio remainder (small, parallel). Pool: none mostly

### WS-J1 — vehicle->character engine-bank map + collision FX + music transitions
> RE the vehicle->character mapping (6 char banks bluejay/gold/melon/pink/red/shadow vs
> 12 vehicles) so the engine voice uses the selected car's real bank; wire permdict
> collision FX (skids/impacts) to real collision events (needs WS-B); add music state
> transitions (menu/race/results). Verify (audio plays per event). Commit.

---

# WS-I — Multiplayer (LATER / R7 tail). Pool: as needed
> Split-screen render + 2nd input + the MP frontend screens (the 4-car race exists).
> **Online/netcode = wontfix for v1.0** (record in DEFERRED.md). Only start after the
> single-player ports (WS-A..G) are verbatim.

---

## Suggested first parallel wave (independent, no shared cliffs)
WS-F1/F2/F4 (data, cheap) · WS-G1 (mode rules) · WS-E1 (renderer opener) ·
WS-B1 (collision gate — stop-and-ask) · WS-A1 + WS-A2 (struct + RW math) · WS-H1 (C4 lane).
Long poles to staff continuously: WS-A (physics) and WS-E (renderer).
