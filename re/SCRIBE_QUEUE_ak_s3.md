# SCRIBE_QUEUE fragment — batch_ak session 3 (ak_s3)

Author-only promote-c2 pass (bucket_vehicle_004922e0_0057c500 — high-address vehicle tail,
RenderWare-library-heavy). Bucket plates are the C2 deliverable; central finalize (ghidra-sweep)
writes hooks.csv / trackers and commits. **This bucket separates genuine vehicle game code (plate
C2) from vendored RenderWare / RenderWare-Physics-3.7 library code (library-skip, kept-C1).**

Pool slot: Mashed_pool2 (pre-assigned; opened read-only, program_close'd). No shared files touched
besides this fragment, the bucket dir, and `.pool_slot_ak_s3`. Binary anchor unchanged (read-only).
All 26 candidates confirmed `C1` in hooks.csv at session start (none already C2+). Every RVA was
live-decoded in Mashed_pool2 on 2026-06-02 (NO-GUESSING: classifications below cite the decomp).

## Queued (C1 -> C2)  — genuine vehicle game code (12 plates)

2026-06-02  ak_s3  bucket=re/analysis/bucket_vehicle_004922e0_0057c500  confidence=C1->C2  rvas=004922e0,00496900,004b3bf0,004b3e40,004b5190,004b5240,004b5320,004b5580,004b65a0,004b65b0,00546b10,00546e70

- **004922e0** — per-car hit/event trigger; two state guards then writes a 4-field "pending event"
  record into the per-player block (base DAT_007f1058, stride 0x4c). Pure game, no RW. (U-1858/59/60)
- **00496900** — `thunk_FUN_00497450`; 4-byte JMP to the player-slot-active predicate (idx<4,
  DAT_007e96fc stride 0x80). Target FUN_00497450 already carries a master `[C2 2026-06-02]` comment;
  this is the separate thunk row. (U-1509)
- **004b3bf0 / 004b3e40** — game-side load-clump helpers (file-stream mode 2 / memory-stream mode 3):
  open stream → find chunk 0x10 → **RpClumpStreamRead** → close. Thin wrappers over the RW stream API,
  called only by powerup/weapon/icon inits with game filenames. Plated `vehicle`; see boundary note.
- **004b5190** — sub-object property dispatch: reads inner RW ref at obj+0x18, runs a fixed 4-call op
  chain (FUN_00543d40/00543d70/0055deb0/00543df0). 13 game callers incl. DepthCharge/GatlingGun. (U-1447)
- **004b5240** — leaf bit-toggle on flags byte obj+2 (set/clear bit 2); DepthCharge/GatlingGun sub-model
  visibility. Pure game, no callees.
- **004b5320 / 004b5580** — game-side per-atomic apply wrappers: pack userdata + invoke
  **RpClumpForAllAtomics** with an in-body game callback (LAB_004b5300 / LAB_004b5560). LaserTower init.
  (U-1448 on 004b5580 arg packing.)
- **004b65a0** — `thunk_FUN_004b68e0`; getter `return DAT_007d3e4c`. (U-1451)
- **004b65b0** — `thunk_FUN_004b68f0`; case-insensitive asset-dir filename lookup (DAT_0090dac0, count
  DAT_008ad9a8, stride 0x80, return +0x78). Game VFS.
- **00546b10** — RwMatrix→quaternion (Shepperd; FastSqrt + 3 degenerate helpers). See boundary note.
- **00546e70** — quaternion SLERP prep (dot, hemisphere select, acos polynomial, 1/sin scaling →
  10-float buffer). All callers game-band (Replay::ReadFrame chain). Resolves S-1572.

## LIBRARY-SKIP (reclass-OUT to third-party-library; NOT plated C2 — sweep keeps C1 under the tag)

Per the prompt's RENDERWARE-LIBRARY RECLASS-OUT rule + memory `project_qhull_rwphysics_island`, the
following 14 are vendored RenderWare / RenderWare-Physics-3.7 primitives mislabeled `vehicle` only
because vehicle/powerup code calls them. Each was decoded and shows RW/Rp primitive vocabulary. Short
note-only plates authored; `library_skip` flag set in each plate's frontmatter.

library_skip=004c0b30:third-party-library[renderware],004c1040:third-party-library[renderware],004c39b0:third-party-library[renderware],004e69a0:third-party-library[renderware],004e6ab0:third-party-library[renderware],004e7e30:third-party-library[renderware],00559c40:third-party-library[RenderWare-Physics-3.7],0055ae70:third-party-library[RenderWare-Physics-3.7],0055b940:third-party-library[RenderWare-Physics-3.7],0055bab0:third-party-library[RenderWare-Physics-3.7],0055c4a0:third-party-library[RenderWare-Physics-3.7],0057c220:third-party-library[RenderWare-Physics-3.7],0057c300:third-party-library[RenderWare-Physics-3.7],0057c500:third-party-library[RenderWare-Physics-3.7]

### RenderWare core (6) — `third-party-library[renderware]`
- **004c0b30** — RW object alloc-by-type 0x3000e via plugin vtable `DAT_007d3ff8 + 0x118` + post-init.
- **004c1040** — RW frame reparent (`RwFrameAddChild`); frame links +4/+0x98/+0x9c/+0xa0 + dirty-frame
  list head `DAT_007d3ff8 + 0xbc`.
- **004c39b0** — `RwV3dNormalize` (master comment already names it); fast-inv-sqrt LUT `DAT_007d3ffc`.
- **004e69a0** — RW atomic clone; decomp resolves the allocator as the **named primitive `RpAtomicCreate()`**.
- **004e6ab0** — RW clump/hierarchy clone; clones atomics via 004e69a0, deregisters via RW vtable +0x11c.
- **004e7e30** — RW object ref-set (FUN_004c0740) + dirty bit (+3 |= 1).

### RenderWare Physics 3.7 (8) — `third-party-library[RenderWare-Physics-3.7]`
- **00559c40** — scene body register (free-slot bitfield scan + activate/sleep vtable dispatch, DAT_007dc8c0).
- **0055ae70** — scene body set shape flags (active-shape bitfield param_1[0x1a]).
- **0055b940** — body set mass + inertia tensor (FUN_0055bbf0 tensor; flag bit 1 of param_1[0x12]).
- **0055bab0** — body set damping (leaf; body+0x4c = 0.01f).
- **0055c4a0** — body set restitution (recursive over compound shapes; type-8 at +0x5c, children stride 0x60).
- **0057c220** — world-slot body assign (registry `DAT_007dc8d8`).
- **0057c300** — RWP37 body create (alloc 0x34, RW mem tag 0x30900, vtable 0x0062403c, type 0xb).
- **0057c500** — RWP37 world create (alloc 0x54, RW mem tag 0x30900, body list 0x10×0x60).

  All eight use the RW memory interface (`DAT_007d3ff8 + 0x108/+0x114/...`) and/or the `0x30900`
  RW-Physics alloc tag and the scene slot-bitfield broadphase — same signature flagged in batch_aj s4
  for the sibling RW-Physics allocators (0055dc70 / 00562520 / 0057c4b0).

## STOP-AND-ASK surfaced (boundary calls — did NOT block the bucket)

- **004b3bf0 / 004b3e40 (game-vs-RW-toolkit boundary)** — thin wrappers over the RW stream + clump API
  (open/find-chunk/**RpClumpStreamRead**/close). Best call = game-side asset-loader glue (0x004bxxxx
  powerups band, filename-driven, only game inits call them), NOT a vendored RW primitive — only their
  callees are RW. Plated `vehicle` with a bare `[UNCERTAIN]`. If the sweep judges them RW toolkit
  helpers, reclass-OUT to `third-party-library[renderware]`; the plates are bit-accurate either way.
- **00546b10 (game-vs-RW-library boundary)** — matrix→quaternion (Shepperd) is consumed by BOTH the
  game replay path (`FUN_004829d0`, Replay::ReadFrame chain) AND the RW-Physics band (`FUN_0055bbf0`,
  the inertia-tensor compute behind PhysicsBodySetMass 0x0055b940; also `FUN_0055b080`/`FUN_0055b800`).
  It uses NO Rw*/Rp* primitive itself (only the game's FastSqrt 0x004c3b30 + sibling degenerate helpers),
  so it reads as shared low-level math rather than a vendored RW primitive. Best call = `vehicle`
  (matches hooks.csv replay_record provenance); flagged `[UNCERTAIN]` in the plate. Sweep may refile as
  shared-math-lib if it prefers.

## Notes for the sweep

- **Subsystem confirmations / drift**: the 12 rvas_c2 are genuine game code (vehicle hit-event,
  race-state predicate, powerup model loaders + per-atomic apply wrappers + asset-dir lookup, replay
  matrix/quaternion math). The 14 library-skips are RW / RW-Physics primitives; keep them C1 under the
  third-party-library tag (do NOT promote to vehicle-C2).
- **Uncertainties**: NO new global U-IDs minted this session (no pre-assigned U-ID range; avoiding the
  known collision failure mode). New holes are inline `[UNCERTAIN]` and all data-/calling-convention-
  semantic (non-blocking for C2 of bit-identical leaves). Pre-existing IDs referenced where they apply:
  U-1858/1859/1860 (004922e0), U-1509 (00496900), U-1447 (004b5190), U-1448 (004b5580), U-1451 (004b65a0).
  Sweep to mint IDs for any inline `[UNCERTAIN]` it wants tracked (incl. the two boundary calls above).
- **Stub callees NOT deep-plated** (RW / RW-Physics library band, referenced per-call in plates):
  RpClumpStreamRead, RpClumpForAllAtomics, FUN_004cc230/004cc5e0/004cc160/004c0e50 (RW stream),
  FUN_00543d40/00543d70/00543df0/0055deb0 (004b5190 op-chain), FUN_004c0740/004c0b70/004c0ad0 (RW core),
  FUN_004e8e90/004e8ea0/004e6d80/004c0870/004e6f80/004d8090/004d8060 (RW clump/atomic internals),
  FUN_0055bbf0/00565570/00565200/00565260/00564c80/0055ded0/0057c550/0057c1b0/00564130/0055b860/0055dff0
  (RW-Physics internals), FUN_00546bf0/00546c50/00546cb0 (matrix→quat degenerate helpers, game-side).
- Files left UNTRACKED (bucket dir + this fragment + `.pool_slot_ak_s3`). No git commit by the worker
  beyond the prescribed scribe commit, no re-classify, no build, no Frida — per author-only mission.

## Pool drain accounting
- vehicle-tail bucket: 26 candidates → **12 C2 plates + 14 library-skips** = 26 resolved.
- C1->C2 promotable (vehicle): 12. RW reclass-OUT (kept-C1): 14 (6 renderware + 8 RenderWare-Physics-3.7).
