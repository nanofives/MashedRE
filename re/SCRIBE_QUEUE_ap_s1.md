# SCRIBE_QUEUE fragment — batch_ap session 1 (ap_s1)

Author-only promote-c2 pass (gameplay campaign FINAL batch 5/5, first slice
0x00562460-0x00565120, directly above the batch_ao all-skip RW-Physics page).
Outcome: **ALL-SKIP — zero C1->C2 plates authored.** Every one of the 28
candidates decodes as a vendored RenderWare-Physics-3.7 primitive
(library-confirm rule applied per RVA, not by address screen). No bucket dir
was created (nothing to plate). Central re-classify reclasses all 28 OUT to
third-party-library[RenderWare-Physics-3.7], kept C1.

## Queued

2026-06-03  ap_s1  bucket=re/analysis/bucket_gameplay_00562460_00565120  confidence=C1->C2  rvas=NONE  library_skip=00562460:RenderWare-Physics-3.7,00562500:RenderWare-Physics-3.7,00562560:RenderWare-Physics-3.7,00562580:RenderWare-Physics-3.7,005625e0:RenderWare-Physics-3.7,00562600:RenderWare-Physics-3.7,00562660:RenderWare-Physics-3.7,00562ed0:RenderWare-Physics-3.7,00562fd0:RenderWare-Physics-3.7,00563810:RenderWare-Physics-3.7,00563840:RenderWare-Physics-3.7,00563940:RenderWare-Physics-3.7,00563b00:RenderWare-Physics-3.7,00563c80:RenderWare-Physics-3.7,00563de0:RenderWare-Physics-3.7,00563df0:RenderWare-Physics-3.7,00563e70:RenderWare-Physics-3.7,00563f60:RenderWare-Physics-3.7,00564040:RenderWare-Physics-3.7,00564100:RenderWare-Physics-3.7,00564130:RenderWare-Physics-3.7,00564190:RenderWare-Physics-3.7,005641b0:RenderWare-Physics-3.7,00564310:RenderWare-Physics-3.7,005645b0:RenderWare-Physics-3.7,005646c0:RenderWare-Physics-3.7,00564c80:RenderWare-Physics-3.7,00565120:RenderWare-Physics-3.7  note=ALL 28 candidates are vendored RW-Physics world-collision-import / convex-hull / octree-broad-phase API (subsystem_observed=third-party-library[RenderWare-Physics-3.7] for every RVA); 0 plated; needs_function_create=NONE (all 28 had function objects and decompiled cleanly); actual pool slot=Mashed_pool1 (pre-assigned; opened read-only cleanly, no LockException; program_close issued)

## Notes for the sweep

- **Count**: 28 RVAs read end-to-end against live decomp (read-only,
  Mashed_pool1, session ae6fb12172e147ea95b6bb8a2e8365d9, image_base
  0x00400000). **0 plates authored — `rvas=NONE` is deliberate** (nothing for
  the sweep to plate from this session; the deliverable is the `library_skip`
  list for central re-classify).

- **POOL SLOT**: pre-assigned **Mashed_pool1** opened read-only on the FIRST
  attempt via `project_program_open_existing` (program_path /MASHED.exe, no
  LockException). `program_close` issued at end. Marker `.pool_slot_ap_s1`.

- **needs_function_create = NONE.** All 28 returned a function object and
  decompiled cleanly.

- **Drift check**: all 28 are their own first-field `gameplay,C1` rows in
  hooks.csv at session start (anchored `^<rva>,` grep, rows 4337-4364; all
  point at existing batch-y-s4 C1 plates in `re/analysis/bucket_00554010/`
  — "collision-bvh-octree + physics-math tail" — which the verdict below
  deepens, not restarts). None already >=C2; none drift-skipped.

- **LIBRARY-CONFIRM VERDICT — all 28 are one coherent RenderWare-Physics-3.7
  module: the world(static-geometry)-collision import + convex-hull support
  mapping + loose-octree broad-phase.** Evidence lines (same shape as ao_s4):

  1. **Named/confirmed-neighbour bracketing.** 00562520 (bitfield-grid
     allocator, sits INSIDE the 00562460..00562660 run) is already confirmed
     third-party-library via batch_aj (cited again by ao_s3 alongside
     0055dc70). 00564190 was pre-identified by ao_s3 as the RW-Physics
     farfield-arena destroy callee (00559ba0 "destroy via FUN_00564190"), and
     ao_s4's evidence line 3 lists 00564040/00564190/00564c80/00565120 among
     the exclusively-vendored-band callee set of the 26 confirmed RW-Physics
     body/scene functions.
  2. **Library allocator + host-callback table throughout.** Raw alloc
     `**(DAT_007d3ff8+0x108)` at 0x0056388e (00563840) and inside 00564130
     (0x0056413c); raw free `**(DAT_007d3ff8+0x10c)` at 0x00562566 (00562560),
     0x0056382e (00563810), 0x00564199 (00564190, forward-thunk shape).
     Allocator memhint tag **0x30900** at 0x00562608 (00562600 → FUN_00564130(
     0x60,0x10,0x30900)) and 0x00563890 (00563840) — identical to the tag in
     confirmed-library FUN_0055dc70 and ao_s3's 00559b50.
  3. **Callees/callers land in the vendored band or are RW core.**
     00562460 calls imported **RpWorldForAllWorldSectors** (twice: callbacks
     LAB_00562490 + FUN_00562500); ctors 00562600/00562fd0 call base-ctor
     FUN_0055c380 (RW-Physics band) with .rdata vtables DAT_005e52a8 /
     DAT_005e5338; 00563940 callees FUN_005663a0/FUN_00565c20 and 00563b00
     callees FUN_00573890/FUN_00565ab0/FUN_00546d10 are all 0x0054-0x0057
     vendored band; 005645b0's ONLY caller is FUN_005595d0 — ao_s3's confirmed
     RW-Physics farfield-arena carve; 00563940's only caller FUN_0057c670 is
     in the qhull-2002.1 island (0x0057c5b0..0x005a5820, FidDB-confirmed).
  4. **Zero game-state access.** No candidate reads/writes any Mashed game
     global. Data refs are exclusively: the library statics block
     DAT_007d3ff8 + profiling counters DAT_007dc8c8/DAT_007dc8cc (at
     0x00564731/0x00564a11 in 005646c0; same static block as physics-library
     sticky flag DAT_007dc8c0 per ao_s3), and the library .rdata float pool
     DAT_005cc318/DAT_005cc320/DAT_005cc32c/DAT_005cc33c/DAT_005d757c/
     DAT_005e5418. Game-page callers exist for the 00562460/00562580/005625e0
     entry points (FUN_0047eb30, FUN_0047f4c0 — 0x0047 pages) but they call
     this surface AS AN API, which does not make it game code (ao_s3 RtCharset
     precedent); those CALLERS are where any glue lives, and they are
     batch_ao-band rows already handled.

- **Per-RVA mechanical verdicts** (deepening the batch-y-s4 C1 plates in
  `re/analysis/bucket_00554010/`; all read from live decomp this session):

  | RVA | ~size | mechanical role (RW-Physics world-import / hull / octree) |
  |---|---|---|
  | 00562460 | 38b | world-collision import driver: RpWorldForAllWorldSectors(world, LAB_00562490, 0) then (world, FUN_00562500, 0); returns world |
  | 00562500 | 18b | RpWorldSectorCallBack thunk -> FUN_00562010(sector); returns sector |
  | 00562560 | 26b | destroy callback: raw free `**(DAT_007d3ff8+0x10c)(p1)`, return 1 |
  | 00562580 | 85b | build-parameter defaults: 17 fields incl. *p=0xffffffff, +4=3, caps 100/100/100, 1000/1000, +0x24=10, +0x3c=0x28, 400/400 (collision-BSP build descriptor; raw constants, no semantic claim) |
  | 005625e0 | 30b | accumulator swap: if old +0xc nonzero subtract from +0x18; store p2 at +0xc; add p2 to +0x18 |
  | 00562600 | 56b | volume ctor: FUN_00564130(0x60,0x10,0x30900) aligned alloc, base-ctor FUN_0055c380(&DAT_005e52a8), +0x40=p1 +0x44=p2(mode) +0x48=&LAB_00562ac0 |
  | 00562660 | 270b | bound-source dispatch on +0x44 mode 0..4: returns ptr +0x50/+0x48/+0x1c (after optional FUN_004e5fc0 refresh, flag bit2 at +0x4c)/(deref +0x5c)+4/zero-sphere 0x7f7fffff; modes 0-1 via FUN_00565cd0 transform; radius += +0x4c float |
  | 00562ed0 | 209b | triangle appender into fixed-cap array (stride 0x4c): copies 3 verts via param_3[7..9] ptrs, centroid/normal param_3[0..2], id param_3[6]; per-tri 4-bit material lookup via param_5[4] nibble table (FUN_00561ea0 on param_2), default 0xf |
  | 00562fd0 | 45b | 3-arg ctor variant: base-ctor FUN_0055c380(&DAT_005e5338), +0x48=0 +0x44=0 +0x40=p3 +0x4c=p2 |
  | 00563810 | 45b | refcount release: --(+0x84); on zero raw free `**(DAT_007d3ff8+0x10c)`; returns 1 (0 if null) |
  | 00563840 | 246b | hull slab alloc: packed 3-array layout (verts*0xc, edges*0xc, faces*0xc + ushort index arrays, 16/4/2-align rounding), raw alloc tag 0x30900, header counts at +0x1c/+0x20/+0x1e, refcount +0x84=1, flags +0x88=0 |
  | 00563940 | 445b | hull finalize phase 1: per-edge length -> inverse length (SQRT, guard DAT_005d757c, 1.0f=DAT_005cc320); AABB min/max accumulate (FUN_005663a0); center +0x44 = min+0.5*(max-min) (DAT_005cc32c=0.5f); max-extent half-size +0x50; per-vert FUN_00565c20; set flags bit0 |
  | 00563b00 | 373b | hull finalize phase 2: quat-rotate dir (FUN_00573890/FUN_00565ab0), pick dominant axis via 0x21312300 shift trick, two support queries FUN_00563c80 along ±axis (DAT_005cc33c=-1.0f scale), thickness +0x58 = sum; set flags bit1 |
  | 00563c80 | 345b | hill-climbing support-vertex search over hull adjacency (verts +0x4, edge table +0x14, vert-edge index +0x8/+0x18, 0xc-stride, early-out param_4) |
  | 00563de0 | 16b | pure size calc: (p1 + 2 + p2*2) * 8 (pair-list node pool bytes) |
  | 00563df0 | 114b | pair-list init in arena: bump-carve (p2+2+p3*2)*8 from +0x8; 8-byte nodes, identity heads 0..p2-1, free chain p2.., 0xffff tail sentinel |
  | 00563e70 | 239b | pair insert: pop 2 nodes off free head +0xc, cross-link (key,value) into both keys' doubly-linked rings (8-byte node: prev,next,partner,twin), 0xffff-guarded |
  | 00563f60 | 221b | bucket clear: unlink every node in key's ring + its twin, push back on free list +0xc |
  | 00564040 | 187b | pair-ring collect: walk key param_2's ring, gather partner ids not masked by bitmap param_4 (`1 << (id & 0x1f)` over dword table), optional bubble-sort ascending if param_5 |
  | 00564100 | 47b | memset clone (dword-wise + tail bytes) |
  | 00564130 | 81b | aligned alloc: raw alloc `**(DAT_007d3ff8+0x108)(size+align, tag)`, stores align-offset byte at result-1 (free side recovers base) |
  | 00564190 | 27b | aligned-free forward thunk -> `**(DAT_007d3ff8+0x10c)` (jumptable-warning indirect jmp; the ao_s3-cited farfield-arena destroy callee) |
  | 005641b0 | 351b | AABB-containment test with hysteresis: blend factors DAT_005e5418/DAT_005cc318 on 8-float loose AABBs, 6-way compare packed to ==7 |
  | 00564310 | 663b | loose-octree octant select: _rand() tiebreak, child AABB out (blend DAT_005e5418/DAT_005cc318), returns 3-bit octant or 0xffff |
  | 005645b0 | 267b | octree arena init: bump-carve 0xc01c slab; root AABB at +0x7fe0; 0x198-entry node free chain (+0x9830 stride 0x14, terminator 0x3ff), 0x3fe-entry leaf-object free chain (+0x8820 stride 4, sentinel 0xffff->0x3ff), 8 root child slots = 0x83ff, optional ushort table zero (+0xb810), pair-list ptr +0xc010, slop float +0xc014 |
  | 005646c0 | 1465b | octree overlap query (iterative, 8-float AABB stack ~0x2a00 bytes): per-node object-ring walk with strict AABB overlap test + optional exclusion-matrix bitmap (+0xc018, dims at +4, bits at +0xc); hits -> FUN_00563e70(pairlist, param_3, id); profiling counters DAT_007dc8c8 (tests) / DAT_007dc8cc (hits); octant pushdown via 0xaa/0x55/0xcc/0x33/0xf0/0x0f masks |
  | 00564c80 | 1179b | octree insert: inflate AABB by slop +0xc014, query existing (FUN_005646c0), descend via FUN_00564310, link object node (free heads +0x981a/+0xc00e), straddle flag bit 0x80 + per-node population in bits 10..14 (+0x400 steps, split at >0xc00 = pop 3), node split redistributes ring via FUN_00564310/FUN_00565120/FUN_005651b0/FUN_00565160, locator code +0x8020 = octant|node<<6 |
  | 00565120 | 60b | octant-table child-link setter: entry (+0x9820 + (octant + node*10)*2) = keep-high-6-bits | param_4 (0x3ff value mask) |

- **For central re-classify**: reclass-OUT all 28 to
  `third-party-library[RenderWare-Physics-3.7]`, kept C1 (library residue, not
  reimplementation targets — same treatment as batch_ak s3 / ao_s4 rows). Do
  NOT rename on master (none renamed; read-only session). One tag flag for the
  central pass: 00562460/00562500 sit on the RpWorld API boundary
  (RpWorldForAllWorldSectors driver + sector callback) — central re-classify
  may prefer plain `renderware` for those two; either way vendored, kept C1
  (same flag shape as ao_s3's 005592f0).
- **U-IDs / S-IDs**: none minted (author-only; nothing plated, so no inline
  [UNCERTAIN] markers created; reserved range U-8900..9199 untouched).
- **gameplay C1 still drains by 28** via the reclass-OUT (324→296).
- Files in the atomic commit: this fragment only. No hooks.csv writes, no
  re-classify, no master-Ghidra mutation (read-only MCP throughout).
