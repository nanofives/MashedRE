# SCRIBE_QUEUE — batch_aj session 5 (author-only promote-c2)

Queued for the ghidra-sweep / central finalize. Do NOT hand-merge into hooks.csv; the sweep drains
this fragment. This session DID `git commit` its bucket dir + this fragment (per the batch_aj s5
workflow), but made **no** mutation to hooks.csv / STUBS.md / UNCERTAINTIES.md — those are the sweep's.

Pool note: pre-assigned **Mashed_pool4** was POISONED by a leaked in-JVM project lock
(`program_list_open=0` yet `LockException` on open — the documented "MCP leaks project lock on failed
open" mode; my first `program_open` used `path="MASHED.exe"` which locked the project then errored).
Fell back to **Mashed_pool10** (.rep 2026-06-02 00:51, freshest; all 27 fns defined). Closed cleanly.

## Queued

2026-06-02  aj_s5  bucket=re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140  rvas=0x0044d5e0,0x004540c0,0x0045b9d0,0x0045baa0,0x0045bac0,0x0045be90,0x0045bed0,0x0045bf30,0x0045bfa0,0x0045c010,0x00441490,0x004414b0,0x00441b30,0x00442410,0x00442420,0x00471ac0,0x00471cf0,0x00471ec0,0x0047c160,0x004b4010,0x004b4050,0x004b4080,0x004b40c0,0x004b40f0,0x004b4120,0x004b4130,0x004b4140

## Promotion target

All 27 RVAs: C1 → **C2** (author-only mechanical plate; one `.md` per RVA in the bucket dir).
All 27 were `C1` in hooks.csv at session start; none already C2+. Drift-skips: NONE.
No `function_create` was needed — every RVA already has a defined function boundary in master.

## STOP-AND-ASK resolution (particle cluster 0x004b40xx)

The guard was: "if particle 0x004b40xx decodes as CRT residue (band ends 0x004b3fff), flag
library_suspect and do NOT promote." **It does NOT decode as CRT residue.** The whole cluster
0x004b3f20..0x004b4140 makes named **RenderWare 3.x RpWorld** calls:
- `FUN_004b4010` → `RpClumpForAllLights`
- `FUN_004b3f90` / `FUN_004b3fc0` (sibling, below bucket) → `RpClumpForAllAtomics`
- `FUN_004e89a0` (0x004e89a0) is the generic `{+0x20:ptr,+0x24:count}` pointer-array forall iterator.
These are first-party Mashed wrappers (with Mashed-authored callbacks at LAB_004b3f20 / LAB_004b4070)
around the RenderWare retained-mode scene graph. **NOT library_suspect, NOT CRT, NOT particle emitters.**
→ Promote all 8; the bucket hypothesis ("particle = emitter field setters") is disconfirmed.

## subsystem_observed (confirmations + RECLASSIFICATIONS)

**powerups (10) — CONFIRMED, all bit-clean C2:**
- Type-table at `0x005f9998`, stride `0x40`, count `DAT_005f9bd8`. Per-phase fn-ptr dispatch slots
  `+0x24` (0x0045b9d0), `+0x28` (0x0045be90), `+0x2c` (0x0045bed0), `+0x30` (0x0045bf30); record
  `+0x00`=type id (lookup 0x0045baa0), `+0x04`=arm virtual, `+0x10`=deactivate virtual.
- Per-player slot array at `0x0088fbe0`, stride `0xb4`, 4 slots; `slot+0xa8`=active-type entry ptr,
  `slot+0xac`=arm-result/countdown. Activate 0x0045bfa0 / pickup-wrapper 0x0045c010 (←FUN_0045bba0);
  deactivate 0x0045bac0; round-cleanup 0x0045bed0; teardown-all 0x0045bf30 (+clears parallel
  `0x0068d4d0` array).
- DepthCharge weapon: path-safety 0x0044d5e0 (gate `DAT_00684b34`; proximity `FUN_004c18c0`;
  obstacle array `0x006848e8` stride `0x48` × 8; `FUN_00547230` segment-intersect; 0.2f=0x3e4ccccd);
  per-frame teardown 0x004540c0 (`RpClumpDestroy` pools at `0x00688260`/`0x00688058`).

**particle (8) — RECLASSIFY `particle -> render`** (RenderWare RpClump scene-graph traversal; see
STOP-AND-ASK above). 0x004b4010 (forall-lights), 0x004b4050/0x004b40f0/0x004b4120/0x004b4080
(forall-atomics / +0x18-linked element forall + deref), 0x004b40c0/0x004b4130/0x004b4140
(copy the `{+0x20,+0x24}` element-pointer array). Promote C2 but central finalize should retag the
subsystem column `particle -> render` for all 8.

**camera (9) — CONFIRMED; 4 carry an existing master-Ghidra `[C2]` plate (hooks.csv DRIFT):**
- 0x004414b0 — camera tilt/zoom from keyboard (`DAT_007f108b/c`) + gamepad axes (`DAT_008989c0`);
  ←FUN_0040d470.
- 0x00441b30 — camera-path entity init from config block; ←FUN_00442600 (CameraPath::InitForTrack);
  uses `FUN_004a2c48` (x87 float→int64 round-half-even, already C2).
- 0x00442410 / 0x00442420 — element-ptr (`&0x008964fc`) and float (`&0x00896570`) getters into the
  `0x00896460` per-slot record array, stride `0xd8` (5 slots; ←FUN_00448dc0, FUN_0044cb00).
- 0x00441490 — 2-resource destructor for global `DAT_006831c0` (no callers; **subsystem_confidence
  low** — keeps hooks.csv `camera` tag, but this leaf alone does not prove camera; flagged for finalize).
- **DRIFT (promote C1→C2; master Ghidra already plated them `[C2 2026-05-18]`):** 0x00471ac0
  (cam-anim event manager), 0x00471cf0 (cam-anim state reset), 0x00471ec0 (cam-anim trigger checker),
  0x0047c160 (camera-path node containment loop). Cam-anim manager array `0x006905c8`/`0x0069064c`,
  stride `0x23` dwords; event-handler table `0x00690ab0` stride `0x21`, id base 2000. These were
  render→camera reclassified 2026-05-18 (d11007) and are already C2-quality in master.

## Uncertainties

No NEW formal U-rows minted by this author-only session. All newly-observed uncertainties are
**data-semantic / struct-field-meaning** only (raw offsets reported with citations) and are
non-blocking for C2 of a mechanically-exact leaf per [[feedback_data_semantic_uncertainty_nonblocking]].
Reserved band **U-7500..U-7599** is available if central finalize chooses to formalize any.
Pre-existing U-rows referenced by the camera plates (carry forward, do NOT re-mint):
U-0556 (00471ac0/D-10356), U-0557 (00471cf0/D-10357), U-0567/U-0568/U-0569 (00471ec0),
U-0570/U-0571/U-0572 (0047c160).
One worth surfacing: **0x0045bfa0** calls `(*(entry+4))(slot)` with no null-check on the
PowerupTypeLookup result — faithful to original (caller is gated on a valid pickup), reported not fixed.

## Stubs

No NEW STUBS.md rows minted. All unresolved callees are defined functions outside the candidate set
(noted as `[STUB]`-style references in each plate for traceability, not as STUBS.md rows). Reserved
band **S-6100..S-6199** available if finalize formalizes any. Pre-existing stub rows referenced by the
camera plates: S-0551..S-0554 (00471ec0 helpers), S-0555 (0047c160 helpers).
Notable callee identifications made this session (for the sweep's cross-ref):
- `FUN_004e89a0` (0x004e89a0) = generic `{+0x20:ptr,+0x24:count}` pointer-array forall iterator.
- `FUN_004a2c48` (0x004a2c48) = x87 `float10`→int64 round-half-to-even helper (already C2).
- `RpClumpForAllLights` / `RpClumpForAllAtomics` / `RpClumpDestroy` = vendored RenderWare exports.
