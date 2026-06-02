# SCRIBE_QUEUE fragment — batch_aj session 4 (aj_s4)

Author-only promote-c2 pass (bucket_physics_smplfzx_00478cb0_0057c4b0). Bucket plates are the C2
deliverable; central finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

Pool slot: Mashed_pool3 (pre-assigned; opened read-only, program_close'd). No shared files touched
besides this fragment, the bucket dir, and `.pool_slot_aj_s4`.

## Queued (C1 -> C2)

2026-06-02  aj_s4  bucket=re/analysis/bucket_physics_smplfzx_00478cb0_0057c4b0  confidence=C1->C2  rvas=00478cb0,0047b9b0,0047ce40,0047cea0,0047d080,0047d100,0047f4c0,0047f840,0047f940,0047fc40,0047fe00,0047ff70,00480100,004850b0,004850e0,004852e0,004853b0,004853f0,00485460,004854e0,004858c0,00485a00,00485a70,00485b30,00485bd0,00485bf0,00485c20

- **27 plates authored**, all confirmed `C1` in hooks.csv at session start (none already C2+; the
  `[C2 2026-05-18]` strings in some inline Ghidra comments are stale plate annotations, NOT hooks.csv
  status — verified against hooks.csv rows directly).
- **physics (13)**: 00478cb0, 0047b9b0, 0047ce40, 0047cea0, 0047d080, 0047d100, 0047f4c0, 0047f840,
  0047f940, 0047fc40, 0047fe00, 0047ff70, 00480100.
- **smplfzx (14)**: 004850b0, 004850e0, 004852e0, 004853b0, 004853f0, 00485460, 004854e0, 004858c0,
  00485a00, 00485a70, 00485b30, 00485bd0, 00485bf0, 00485c20.

## LIBRARY-SKIP (STOP-AND-ASK candidates — vendored RenderWare Physics 3.7 / qhull; NOT plated as physics)

Per the prompt's STOP-AND-ASK rule and memory `project_qhull_rwphysics_island`, three of the listed
physics candidates show RW-Physics / qhull library shape and were **library-tagged + SKIPPED** (no C2
plate authored). Recommendation to the sweep: reclass-OUT to third-party-library (keep-C1 or retire),
NOT promote to physics-C2.

- **0055dc70** — `subsystem_observed=third-party-library[RenderWare-Physics-3.7]`. Allocates a 0x20-byte
  world object via the RenderWare memory interface `(**(code**)(DAT_007d3ff8 + 0x108))(0x20, 0x30900)`,
  then constructs it via `FUN_0055dca0` (which itself does `DAT_007d3ff8 + 0x108`/`+0x114` allocs with
  rwID tag `0x30900`). DAT_007d3ff8 = RW memory-function table (null in static image, set at runtime).
  Sole caller `FUN_0047f4c0` (game world ctor). HIGH confidence library.
- **00562520** — `subsystem_observed=third-party-library[RenderWare-Physics-3.7]`. Bit-matrix allocator:
  `uVar2 = (rows*cols + 0x1f) >> 5` words, `calloc((uVar2+3), 4, 0x30900)` via `DAT_007d3ff8 + 0x114`;
  header `[rows, cols, word_count]`. Same RW memory interface + `0x30900` tag. Sole caller
  `FUN_0047f4c0`. HIGH confidence library.
- **0057c4b0** — `subsystem_observed=third-party-library[qhull-2002.1]`. World-descriptor budget
  accumulator (`+4`++, `+8`+=p2, `+0x10/+0x14`+=p3, `+0x1c/+0x20`+=p3*5) that also writes the global
  `_DAT_00624044`. Evidence it is qhull/RW-Physics: (a) sits immediately below the qhull code-island
  start `FUN_0057c5b0` (island = 0x0057c5b0..0x005a5820 per memory); (b) `_DAT_00624044` lies inside
  the qhull data block — the very next global `0x0062406c "qhull s Pp"` (qhull option string) is read
  by qhull-island code at `0x0057ca47`/`0x0057ca56`; (c) provenance string at `0x005e5f58`:
  `"@@(#)$Id: //Physics/Rwp37Active/src/qhull/src/RwpQHullWrapper.c#1 $"` confirms the band is vendored
  RenderWare Physics 3.7 qhull. It is the descriptor-builder sibling of the two allocators above, all
  called only by `FUN_0047f4c0`. MEDIUM-HIGH confidence; residual [UNCERTAIN]: `_DAT_00624044` is
  write-only (single writer at 0x0057c4ef, zero readers), so its qhull membership is inferred from
  data-region locality + island-adjacency + sibling role rather than a direct qhull-code read. Sweep:
  please confirm before any irreversible retire.

  NOTE: `FUN_0047f4c0` itself (0x0047f4c0) IS a legit **game-side** physics function (it lives in the
  0x0047xxxx game physics band and merely drives the RW-Physics library); it was plated as physics-C2
  this session. Only the three RW-Physics/qhull-band callees above are library-skipped.

## Notes for the sweep

- **Subsystem confirmations / drift**:
  - The 13 physics rows are all genuine physics (physics world ctor/lazy-init, rigid-body creators for
    cylinder/box/sphere/cone, body slot lookup, activate/deactivate, joint/impulse apply, body
    post-init with AABB+broadphase). hooks.csv already carries the `d11007-reclass 2026-05-18:
    render->physics` note for most; plates keep `physics`. No further reclass needed.
  - **0047b9b0** is mechanically a Lua-script executor wrapper (runs COURSE.LUA via `(*param_3)()`
    with `&LAB_00440bc0`); hooks.csv flags `[subsystem-drift: input, vehicle]`. Plate keeps `physics`
    to match hooks.csv; true subsystem is script/track-init glue — sweep may re-file but it is not a
    physics-mechanics function.
  - **smplfzx** is the game's own internal label (verbatim from debug strings in 004852e0/004853b0:
    `"SmplFzx : Retiring %d"`, `"ERROR: SmplFzx. Retiring NULL object!!"`). NOT expanded/guessed.
    Mechanically the cluster is a per-vehicle pooled-object system: a free-stack (`DAT_006e71c8`,
    pop=00485bd0 / push=00485bf0) + a doubly-linked-list registry/manager (`DAT_006e71cc`,
    alloc=00485a70 / remove=00485b30 / lookup=004850b0 / getter=004853b0); object creation 004850e0
    builds an 8-vertex bounding cube (004854e0) with spring/damp params (004858c0) and a type-table at
    `DAT_006e70e8` stride 0x1c; retire path 004853f0->004852e0; 00485c20 is a 7-way vehicle
    bone/attachment getter. All 14 confirmed `smplfzx`.

- **Body-creator family note**: 0047f940 (cylinder) does NOT write the box-type flag `DAT_006c6eb8[i]=1`
  in its loop, whereas 0047fc40/fe00/ff70 (box/sphere/cone) do. 0047fc40 additionally calls the
  box-only `FUN_0055bab0` margin setter. Otherwise the registration loop is identical across all four.

- **Uncertainties**: NO new global U-IDs minted this session (no pre-assigned U-ID range in the prompt;
  avoiding the known 6-way collision failure mode). New holes are marked inline as `[UNCERTAIN]` in the
  plates and are all data-/calling-convention-semantic (non-blocking for C2 of bit-identical leaves).
  Pre-existing U-IDs referenced where they apply: U-1954 (0047f840), U-1955/1956/1957 (00480100),
  U-2498/2499 (0047f4c0), U-2500/2501 (0047d080), U-2502 (0047d100), U-2647/2648/2649 (0047b9b0),
  U-3706 (0047cea0), U-3692 (00562520), U-3693 (0057c4b0). Sweep to mint IDs for any inline
  `[UNCERTAIN]` it wants tracked.

- **DEFERRED touchpoints resolved by these plates** (per hooks.csv `clears` notes; leave the actual
  tracker mutation to the sweep): D-2630 (0047f840), D-2631/2632/2633/2634 (body creators),
  D-2635 (00480100), D-2636 + S-0907 (0047ce40), D-7840/7841/7842 (0047b9b0).

- **Stub callees NOT deep-plated** (RW-Physics/RW-graphics library band, referenced per-call in plates):
  FUN_0057c210/0057c220/0057c300/0057c420/0057c500/0057c550 (RW-Physics shape lifecycle),
  FUN_00559c40/00559ee0/0055ab30/0055ac00/0055ac50/0055ad20/0055ad30/0055ade0/0055af40/0055b650/
  0055b940/0055bac0/0055bab0/0055bd80/0055c490/0055c4a0/0055c4f0/0055c540/0055c810/0055dca0/0055ddd0/
  0055dec0/0055deb0/0055dff0/0055e440/0055fe10/0055fe30/0055fe40/00562600/00562fd0 (RW-Physics ops),
  FUN_004c0b30/004c0c20/004c0e50/004c15c0/004c1740/004c1800/004c1840/004c3d90/004c45f0/004c51a0/
  004c57a0 (RW matrix/transform), FUN_004e4380/004e69a0/004e7e30/004e5fc0 (RW clump/frame + vehicle
  sync), FUN_0041f1e0 (vehicle bone lookup), FUN_00451730, FUN_00485340 (existing validate),
  FUN_004987b0 (logger), FUN_004a332b (CRT abort), FUN_004b5240, FUN_00487280 (broadphase reg).

- Files left UNTRACKED (bucket dir + this fragment + `.pool_slot_aj_s4`). No git commit by the worker
  beyond the prescribed scribe commit, no re-classify, no build, no Frida — per author-only mission.

## Pool drain accounting
- physics pool: 16 candidates → **13 C2 plates + 3 library-skips** = 16 resolved → physics pool to 0.
- smplfzx pool: 14 candidates → **14 C2 plates** = 14 resolved → smplfzx pool to 0.
