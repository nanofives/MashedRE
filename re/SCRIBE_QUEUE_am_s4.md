# SCRIBE_QUEUE fragment — batch_am session 4 (am_s4)

Author-only promote-c2 pass (gameplay campaign batch 2/~5, slice 0x00422440 + the
0x00449-0x0044e cluster). Bucket plates are the C2 deliverable; central finalize
(ghidra-sweep + re-classify) writes hooks.csv / trackers and commits.

## Queued

2026-06-03  am_s4  bucket=re/analysis/bucket_gameplay_00422440_0044e070  confidence=C1->C2  rvas=00422440,00449ba0,0044aaa0,0044ab20,0044b000,0044bbc0,0044bc30,0044bd50,0044be70,0044bfa0,0044c370,0044c490,0044c4f0,0044c5f0,0044c740,0044c8b0,0044c920,0044caa0,0044cb00,0044d6e0,0044df80,0044dfe0,0044dff0,0044e020,0044e050,0044e070

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in `bucket_gameplay_00422440_0044e070/`. None
  drift-skipped. All read end-to-end against live decomp in Mashed_pool7 (read-only).
  Plate set == queued rva set (verified, exact 26/26).

- **POOL SLOT**: pre-assigned **Mashed_pool7** opened read-only via
  `project_program_open_existing` (session `ffb170dd4ddf4ebaa89965999ed1399f`, image_base
  0x00400000). The on-disk `Mashed_pool7.lock` is the persistent clone-creation lock
  (Thu May 28 13:28, same mtime as the .gpr) — NOT an active hold; open succeeded
  read-only with no leak. `program_close` issued at end. Recorded in `.pool_slot_am_s4`.

- **needs_function_create = NONE.** All 26 returned a non-null `function_at` and
  decompiled cleanly.

- **library_skip = NONE.** Zero vendored-library hits. The whole slice is genuine
  game/engine glue (RenderWare transform/emitter/Im2D helpers + audio dispatcher
  accessors), none decoded as a CRT/D3DX9/qhull/RW-primitive named family.

- **Subsystem findings — the "gameplay" hypothesis reclassifies heavily** (as batch_am
  predicted). I did NOT edit hooks.csv (author-only). subsystem_observed recorded per
  plate; recommend the central re-classify reconcile as below:

  - **render / effects (20)** — one coherent effect-entity subsystem spanning
    0x0044a-0x0044d, all RenderWare transform / emitter / Im2D / frame-hierarchy:
    - **smoke particle effect** `&DAT_00683288`: 00449ba0 (thunk→FUN_00449b30, builds
      "smoke" textured effect).
    - **emitter array `&DAT_0068432c`/`&DAT_00684320`** (10-dword stride): 0044aaa0
      (init/transform pass), 0044ab20 (per-frame update w/ RW world-bitmap visibility
      gate via FUN_0055dec0/FUN_0055e300), 0044b000 (emitter slot getter +0x30).
    - **effect-entity update + spawn/physics/billboard-draw**: 0044bbc0 (spawn descriptor
      → FUN_00484cf0 queue, cb &LAB_0044ba30), 0044bc30 (damped-spring physics integrate,
      dt), 0044bd50 (4 dark-quads Im2D draw, color 0x60101010), 0044be70 (sibling, white
      quads 0x60ffffff), 0044bfa0 (main per-frame update; sin/cos oscillators, calls
      0044bbc0+0044bc30, builds 2 normalized bases, camera scalar via FUN_004260b0),
      0044c370 (effect getter `*(DAT_00896000+4)+0x40`), 0044c490 (spawn descriptor
      sibling, cb &LAB_0044c380).
    - **3-slot 0x58-byte bouncing-object array `&DAT_00896120`** (3×0x58=0x108): 0044c4f0
      (slot init, particle attach FUN_004b3f90/FUN_0047fad0), 0044c5f0 (per-slot tick,
      damped reflecting 1-D bounce), 0044c740 (all-pairs 1-D collision resolve, leaf),
      0044c8b0 (init driver, RwFrameAddChild), 0044c920 (tick driver: all-pairs + per-slot).
    - **4-entry handle array `&DAT_00896230`**: 0044caa0 (init, RW frame clones), 0044cb00
      (per-frame index-driven rotate/translate).
    - **misc**: 0044d6e0 (DAT_00684b34 setter; caller FUN_00480340 in the 0x48 effect band).
    - **0044df80** (80-item show/hide batch toggle): marked **render** because the body
      itself calls the 0x0055xxxx RW-visibility ops FUN_00557fb0/FUN_00558100/FUN_00558140.
      Its caller FUN_00410510 is gameplay race-state — see the cross-cutting-array caveat.

  - **audio (5)** — accessors over the `&DAT_00890080` in-world-item array (80 records,
    stride 0xf8) whose **sole caller is FUN_00461650**, the already-[C2] "in-world item
    proximity SFX" audio dispatcher: 0044dfe0 (item count = 0x50/80, the loop bound),
    0044dff0 (item world-position getter for listener-distance), 0044e020 (per-item bool,
    field +0x30 < -0.015), 0044e050 (per-item float +0x28 → voice pitch factor _DAT_0069048c),
    0044e070 (per-item table lookup into &DAT_00894de0 ×1/6 → voice pitch factor _DAT_0068f630).
    0044e050 & 0044e070 outputs are multiplied and written as a 3D-voice pitch param via
    FUN_005a6dc0 — concrete audio evidence.

  - **gameplay (1, kept)** — 00422440: pure cosine-ease lerp math leaf
    (`lerp(a,b,(1-cos(pi*t))*0.5)`). Subsystem-neutral; no subsystem-specific evidence
    (sole caller FUN_00422470 not read). Left at gameplay (batch hypothesis); central
    re-classify may prefer **util** (general math helper) — flagged.

  - **CAVEAT (genuinely cross-cutting — central re-classify to decide):** the
    `&DAT_00890080` 80-record array is a shared **in-world placed-item** array. Its items
    are RW-renderable (visibility toggled by 0044df80 → render) AND audio-emitting
    (position/pitch consumed by the FUN_00461650 audio dispatcher). I split the accessor
    group by their actual sole-caller evidence (0044df80→render via its own callees;
    dfe0/dff0/e020/e050/e070→audio via FUN_00461650). If the project prefers a single
    "world/gameplay" label for this whole item-array family, that is a defensible
    alternative — the mechanical reads above support either; the sweep owns the call.

- **Live constants read (Mashed_pool7)** — decoded IEEE-754 LE, now in the plates:
  0x005cd310=pi(0x40490fdb), 0x005cc320=1.0, 0x005cc32c=0.5, 0x005cc33c=-1.0,
  0x005cc348=1.5, 0x005cc558=0.001, 0x005cc8f4=0.166667(1/6), 0x005cc9b4=0.99,
  0x005cd0b4=0.875, 0x005cd50c=-0.5, 0x005cc94c=4294967296.0(2^32),
  0x005ce18c=0.02, 0x005ce200=2.0, 0x005ce26c=3.5, 0x005ce270=0.018549,
  0x005ce2d8=-0.015. **Anomaly**: 0x005ce278 reads raw **0x60000000** (≈3.69e19 if a
  float) yet the decompiler types its use as a float10 sin/cos amplitude written into an
  int field param_1[8] (FUN_0044bfa0) — cited raw + flagged [UNCERTAIN] (likely
  int-typed / packed datum; control flow is explicit regardless). Immediates in code:
  0x3f59999a=0.85, 0x406147ae=3.5201, 0x4060a3d7=3.5100, 0x3dcccccd=0.1,
  0x3f7ff972≈0.99998, 0x42a00000=80.0, 0x42c80000=100.0, packed colors 0x60101010 /
  0x60ffffff (alpha 0x60). 0x005d757c=0.0 (sign-test sentinel).

- **Uncertainties carried** (all data-semantic / struct-layout / helper-semantic —
  NON-BLOCKING for C2 of these control-flow-complete reads; marked **bare `[UNCERTAIN]`**
  in the plates, NOT minted — central re-classify owns U-numbering from the reserved
  **U-8000..U-8299** range): effect-entity struct field maps (0044bc30, 0044bfa0,
  0044c4f0, 0044c5f0); spawn-descriptor field semantics + callback bodies LAB_0044ba30 /
  LAB_0044c380 (0044bbc0, 0044c490); render-state ids 1/0xc and color channel order
  (0044bd50, 0044be70); FUN_0055dec0 struct fields +0x14/+0x6c (0044ab20); 0x60000000
  anomaly (0044bfa0); in-world-item per-record field meanings +0x28/+0x30/+0xc/+0x10 and
  the &DAT_00894de0 table contents (0044e020/e050/e070); DAT_00684b34 / DAT_005f96d4 /
  DAT_006146fc data semantics. **needs-attention** (function_callers == 0, indirect/vtable
  entry not located): 00449ba0, 0044aaa0, 0044ab20, 0044bd50, 0044be70, 0044bfa0,
  0044c8b0, 0044c920, 0044caa0, 0044cb00 — these are the vtable-dispatched effect
  init/update/draw drivers (expected for a per-frame effect subsystem).

- **Stubs noted** (NOT minted — sweep owns S-numbering from reserved **S-6400..S-6599**).
  Recurring RenderWare helpers across this cluster: transform/matrix band FUN_004c0ed0,
  FUN_004c1340, FUN_004c1480, FUN_004c1520, FUN_004c15c0, FUN_004c39b0(normalize),
  FUN_004c3d90, FUN_004c51a0; emitter handles FUN_0047d080, FUN_0047d100, FUN_0047d150,
  FUN_0047d180; spawn queue FUN_00484cf0; particle setup FUN_004b3f90, FUN_0047fad0,
  FUN_0047cdc0, FUN_004728f0; Im2D submit FUN_004cd070/FUN_004cd2d0/FUN_004cd140 + render
  device vtable DAT_007d3ff8(+0x20); frame hierarchy FUN_004e6ab0, FUN_004e4800,
  RwFrameAddChild; world/visibility FUN_0055dec0, FUN_0055e300, FUN_00557fb0,
  FUN_00558100, FUN_00558140; transform-build FUN_00545d30; texture-by-name FUN_0040bb30;
  index helpers FUN_00442410/FUN_00442420; camera FUN_004260b0; effect finalize
  FUN_00449880; alloc/zero FUN_004b6520; texture/material setters FUN_004770c0/
  FUN_00476ae0/FUN_00476c10. Audio voice band (from caller FUN_00461650, already C2):
  FUN_005a66d0/FUN_005a6dc0/FUN_005a6d60, FUN_0045e0f0.

- **No new U-IDs / S-IDs / arg_types minted. No Frida, no build, no re-classify, no
  hooks.csv write, no master-Ghidra mutation.** Per author-only mission: only the bucket
  dir, this fragment, and `.pool_slot_am_s4` were created/modified.
