# SCRIBE_QUEUE fragment — batch_ar session 6 (ar_s6)

Author-only promote-c2 pass (render campaign 2/2 FINAL, sixth/last slice — the
scattered tail 0x00551840-0x005cb42b). Outcome: **ALL-SKIP — zero C1->C2
plates authored.** Every one of the 50 candidates decodes as a vendored
RenderWare 3.x library primitive (library-confirm rule applied per RVA from
live decomp, not by address screen): the 0x00551840-0x00553ef0 run is the
Rt2d 2D-graphics toolkit (anim-channel + vector-path halves), the
0x00558-0x0055b + 0x0057bxxx-0x0057c5xx bands are the RenderWare-Physics-3.7
module that abuts the qhull island, and 005cb404/005cb42b are RW-core
residue next to the batch_ai V3dTransform trio. No bucket dir was created
(nothing to plate). Central re-classify reclasses all 50 OUT to
third-party-library[<tag>], kept C1 — render C1 closes by 50 from this
session's share.

## Queued

2026-06-03  ar_s6  bucket=re/analysis/bucket_render_00551840_005cb42b  confidence=C1->C2  rvas=NONE  library_skip=00551840:renderware,00551a50:renderware,00551ad0:renderware,00551c40:renderware,00551ca0:renderware,00551ce0:renderware,00551cf0:renderware,00551d40:renderware,00551fd0:renderware,00551fe0:renderware,00552020:renderware,00552230:renderware,005522e0:renderware,005524a0:renderware,00552720:renderware,00552890:renderware,00552920:renderware,00552fa0:renderware,00553030:renderware,005530f0:renderware,005531f0:renderware,005533d0:renderware,00553590:renderware,00553620:renderware,005538a0:renderware,00553c50:renderware,00553cf0:renderware,00553e00:renderware,00553e80:renderware,00553ef0:renderware,005578a0:renderware,005584c0:renderware,00558df0:renderware,0055ac00:RenderWare-Physics-3.7,0055ac50:RenderWare-Physics-3.7,0055ad30:RenderWare-Physics-3.7,0055b650:RenderWare-Physics-3.7,00561ee0:renderware,0057bf30:RenderWare-Physics-3.7,0057c0d0:RenderWare-Physics-3.7,0057c1b0:RenderWare-Physics-3.7,0057c210:RenderWare-Physics-3.7,0057c270:RenderWare-Physics-3.7,0057c2b0:RenderWare-Physics-3.7,0057c370:RenderWare-Physics-3.7,0057c420:RenderWare-Physics-3.7,0057c440:RenderWare-Physics-3.7,0057c550:RenderWare-Physics-3.7,005cb404:renderware,005cb42b:renderware  note=ALL 50 candidates are vendored library code (30x Rt2d toolkit 00551840-00553ef0 incl. the watch-(a) cluster 00553c50-00553ef0 = Rt2d path bbox/node primitives, the library substrate of the 0x00554010 font-vector module — ao_s2 module-vendor-doubt resolves toward VENDOR; 14x RenderWare-Physics-3.7 incl. the full 0057bf30-0057c550 fringe abutting the qhull island — NO qh_*-shapes seen, zero qhull-2002.1 tags; 6x generic RW plugin/stream/core); subsystem_observed=third-party-library[<tag>] for every RVA; 0 plated; needs_function_create=NONE (all 50 had function objects and decompiled cleanly); actual pool slot=Mashed_pool15 (pre-assigned; opened read-only cleanly at the FLAT mashed_pool location with program_name=MASHED.exe on first attempt, no LockException; program_close issued)

## Notes for the sweep

- **Count**: 50 RVAs read end-to-end against live decomp (read-only,
  Mashed_pool15, session a21d16a8fb1c48e4bf4845ace9c8200c, image_base
  0x00400000). **0 plates authored — `rvas=NONE` is deliberate** (the
  deliverable is the `library_skip` list for central re-classify).

- **POOL SLOT**: pre-assigned **Mashed_pool15** opened read-only on the FIRST
  attempt (project_program_open_existing, flat `mashed_pool\` location,
  project_name Mashed_pool15, program_name MASHED.exe — no LockException).
  `program_close` issued at end. Marker `.pool_slot_ar_s6`.

- **needs_function_create = NONE.** All 50 returned a function object and
  decompiled cleanly (incl. the odd-aligned 005cb404/005cb42b).

- **Drift check**: all 50 are their own first-field `render,C1` rows in
  hooks.csv at session start (anchored `^<rva>,` grep, all 50 verified in one
  pass). None already >= C2; none drift-skipped.

- **WATCH RESULTS (the four mission watches)**:
  - **(a) 00553c50..00553ef0 (below the 0x00554010 hud font-vector module)**:
    LIBRARY-CONFIRMED, not plated as hud. They are the Rt2d vector-path
    bbox/prune/free/alloc primitives (CriterionTK memhints 0x30190/0x301a1,
    engine fn-table allocs into the Rt2d node pool DAT_00912a24, global
    scratch path DAT_00912a28, zero game state). Since Rt2dFont is built on
    Rt2dPath, this strengthens the VENDOR side of ao_s2's module-vendor-doubt
    for 0x00554010 itself (not adjudicated here — 0x00554010 is not in this
    candidate set).
  - **(b) RW-Physics-band neighbours**: 0055ac00/0055ac50/0055ad30/0055b650
    CONFIRMED physics-shaped (see verdicts) -> tagged RenderWare-Physics-3.7.
    005584c0/00558df0/00561ee0 sit in the same band but their bodies show only
    generic RW machinery (scratch-buffer init with vendor-0x401 memhint
    0x40180; descriptor-driven container stream reader; plugin registration
    pair with 005578a0) — tagged generic `renderware` rather than overclaiming
    the physics tag; central re-classify may merge tags freely (same
    third-party-library outcome).
  - **(c) qhull fringe 0057bf30..0057c550**: NO qh_*-shapes (no qhull globals,
    no qh_ naming, no convex-hull machinery). All 10 are one coherent
    physics-module tail (plugin 0x901 registration + rigid-body pose/refresh +
    intersection predicates) ending 0x60 below the qhull island 0x0057c5b0 —
    consistent with batch_ak's RW-Physics reclass of 0057c220/300/500.
    **Zero qhull-2002.1 tags minted.**
  - **(d) 005cb404/005cb42b**: 005cb404 is the RW FPU control-word shim (saves
    CW to _DAT_00915000/_DAT_00915004, sets (cw & 0xfffffcff) | 0x3f = single
    precision, all exceptions masked); 005cb42b is an empty no-op (ret).
    Page-neighbours of the batch_ai-confirmed V3dTransform renderware trio
    005cb000/07f/0ef -> renderware.

- **LIBRARY-CONFIRM VERDICT — three coherent vendored modules:**

  **(A) Rt2d 2D-graphics toolkit, 30 RVAs (00551840..00553ef0).** Two halves:
  an anim-channel evaluator family and a vector-path family. Evidence lines:
  1. CriterionTK memhints throughout: keyframe/name allocs hint **0x30135**
     (00551ad0/00551c40), path-segment appends hint **0x30190**
     (00553030/005530f0/005531f0/005533d0/00553620/005538a0), path-node
     allocs hint **0x301a1** (00553030/00553620/00553ef0) — all via the
     engine-instance fn-table `**(DAT_007d3ff8+0x118)` / freed via `+0x11c`
     / `+0x10c`, exactly the header's RW-idiom list.
  2. RW stream API + 3.7 version anchor: 00551840 writes a chunk header via
     FUN_004cc580(stream, 1, len, **0x37002**, 10) and payloads via
     FUN_004cc770/FUN_004cbe80; 00551c40 reads via FUN_004cbd30/FUN_004cc790.
  3. 2D-affine math: 00552720 extracts the 6-float 2D transform from a 4x4
     RwMatrix picking [0],[1],[4],[5],[0xc],[0xd]; 00552230 recovers rotation
     via fpatan(-m[4], m[0]) and rebuilds with FUN_004c51a0/FUN_004c4d20
     against .rdata consts DAT_005e488c/DAT_005e4898/_DAT_005e48a8.
  4. Module statics only — the Rt2d device page DAT_00912xxx (channel array
     DAT_009124f8; device fields DAT_00912b08/0c/10/58/be0/be4/be8/bec/bf0/
     c0c; pools DAT_00912a24, scratch path DAT_00912a28; keyframe pool
     DAT_007dc770) and type descriptors DAT_00623e24/DAT_00623e54. **Zero
     Mashed game globals** (no 0x0068xxxx+ entity pools, no application-page
     data) in any of the 30.
  5. Leaf math via the C3-verified RW-core pair FUN_004c3b30 (FastSqrt) /
     FUN_004c3b90 (FastInvSqrt) for path tangents/arc-lengths, and RW matrix
     helpers FUN_004c45f0/FUN_004c51a0/FUN_004c4d20/FUN_004c52f0/FUN_004c3d90/
     FUN_004c3dc0.

  **(B) RenderWare-Physics-3.7 module, 14 RVAs (0055ac00..0055b650 +
  0057bf30..0057c550) plus 4 generic-RW band-mates (005578a0/005584c0/
  00558df0/00561ee0).** Evidence lines:
  1. Rigid-body state idiom: 0055b650 accumulates force (+0x30, stride 0x20)
     and torque += cross(f, p - body_pos) with body pos at +0x30 of a 0x40-
     stride state record under `*(*(h)+0x10)`; 0057c440 copies the same
     0x40-stride pose matrix and re-bases it via FUN_004c51a0(m, -refpos, 1)
     + FUN_004c45f0.
  2. Activation manager: 0055ac00/0055ac50/0055ad30 set/clear per-object id
     bits (`ushort` id at obj+0x20 / obj[8]w) in bitmask tables at mgr+0x5c/
     +0x60/+0x64 with vtable activate/deactivate callbacks (*obj)[+0x10]/
     [+0x14], active-count mgr+0x4c capped by mgr+0x48, one-shot overflow
     flag DAT_007dc8c0.
  3. Plugin registrations: 0057c270 -> **RpAtomicRegisterPlugin**(4, 0x901,
     ...) storing plugin offset in DAT_007dc8d8 (read back by getter
     0057c210) and installing destructor PTR_LAB_00624058 = FUN_0057c2b0;
     005578a0 -> RpAtomicRegisterPlugin(4, 0x112, &LAB_00561f40, ...) +
     engine plugin FUN_004c2d90(0, 0x112, ...) + FUN_004e7da0(0x112, ...);
     00561ee0 (one-shot gate DAT_00623fac) -> FUN_004f0910(4, 0x902,
     &LAB_00561f40, FUN_004d7ff0, FUN_004d7ff0) + FUN_004f0940(0x902, ...) —
     shares callback &LAB_00561f40 with 005578a0 (same module).
  4. Geometry predicates: 0057bf30 (segment-vs-surface nearest point via
     FUN_005667c0 normalize + FUN_00566ea0/FUN_00566d50 intersect, sentinel
     return _DAT_005cc574) and 0057c0d0 (same-side test: dot of two cross
     products vs DAT_005d757c) call exclusively into the 0x00566xxx physics
     math band.
  5. RW error idiom: 00558df0 raises FUN_004d7ff0(0x80000013, 8) +
     FUN_004d8480 on OOM; allocs hint `desc[+4]|0x30000`; 005584c0 allocs
     0x1c000/0x3000 scratch buffers with vendor-0x401 hint 0x40180 into
     DAT_007dc8b8/bc.
  6. Band continuity: destructors 0057c370/0057c550 free via fn-table +0x10c
     and call into 0x0055bxxx/0x00562xxx/0x00564xxx (FUN_0055df90/
     FUN_0055b930/FUN_00564190); 0057c1b0 wraps FUN_0055c380(obj,
     &DAT_005e5e50); batch_ak already reclassed interleaved 0057c220/300/500
     to RW-Physics. Zero game-state access anywhere.

  **(C) RW core residue, 2 RVAs (005cb404/005cb42b)** — FPU CW shim + no-op,
  V3dTransform-trio page (see watch (d)).

- **Per-RVA mechanical verdicts** (all read from live decomp this session):

  | RVA | ~size | mechanical role (vendored module) |
  |---|---|---|
  | 00551840 | 166b | Rt2dAnim channel serializer: presence bitmask over 8 track slots at DAT_009124f8+8+off; chunk hdr FUN_004cc580(s,1,len,0x37002,10); mask + per-track 0x20B writes |
  | 00551a50 | 120b | per 2 outputs: scan 8 tracks for kf-table[+0x20+out*4]==out; lazily FUN_004c57a0 create / FUN_004c5770 destroy output object |
  | 00551ad0 | 284b | clip ctor: FUN_0052da20(DAT_00623e24,2,0,1.0f); name record alloc hint 0x30135, strncpy<=0x20; seeds 2 identity keyframes via FUN_005522e0 |
  | 00551c40 | 93b | keyframe stream read: alloc 0x30135; FUN_004cbd30(s,kf,0x20)+FUN_004cc790(s,kf+0x20,0x20); refcount *(kf+0x40)=1 |
  | 00551ca0 | 52b | keyframe stream write: FUN_004cbe80(s,kf,0x20)+FUN_004cc770(s,kf+0x20,0x20) |
  | 00551ce0 | 5b | returns 0x40 — keyframe record size |
  | 00551cf0 | 55b | keyframe refcount-- at *(track+0x14)+0x40; 0 -> free into DAT_007dc770 pool (+0x11c) + FUN_00562560(track) |
  | 00551d40 | 648b | channel evaluator: reset 2 output transforms (identity, flags|0x20003); per 8 tracks x keyframes dispatch on interpolator fn-ptr (&LAB_005524c0 direct-6f / &LAB_00552050 angle via FUN_004c45f0+FUN_004c51a0(&DAT_005e488c,2)+FUN_004c4d20(*_DAT_005cd100)+FUN_004c51a0(&DAT_005e4898,2) / indirect); apply FUN_004c52f0; tail FUN_005402d0 |
  | 00551fd0 | 8b | getter *(track+0x14) — keyframe-table ptr |
  | 00551fe0 | 55b | per 8 track slots: FUN_0052e0e0(track, arg) |
  | 00552020 | 39b | any-track-nonnull predicate over 8 slots |
  | 00552230 | 174b | RwMatrix(16f) -> 2D transform[6]: angle=fpatan(-m[4],m[0]); rebuild via FUN_004c51a0/FUN_004c4d20, consts DAT_005e488c/DAT_005e4898/_DAT_005e48a8 |
  | 005522e0 | 92b | keyframe init dispatch by type descriptor: DAT_00623e24 -> FUN_00552720 extract; DAT_00623e54 -> FUN_00552230 convert; stores time pair |
  | 005524a0 | 32b | serialized clip size = FUN_00551ce0() + *(clip+4)*0x20 + 4 |
  | 00552720 | 43b | copy 2D affine: 6 floats from 4x4 matrix picks [0],[1],[4],[5],[0xc],[0xd] |
  | 00552890 | 141b | lazy-cached flatness threshold^2: (DAT_00912b08/_DAT_00912b10)*_DAT_00912be4*DAT_00912be0 thru FUN_004c3dc0(&DAT_00912b58); cache DAT_00912bec/_DAT_00912be8 |
  | 00552920 | 315b | viewport-rect path: device extents DAT_00912b08/0c x DAT_00912be0 (skip if *(DAT_00912c0c+0x14)==2) x _DAT_005cd50c/_DAT_005cc32c; FUN_004c3d90 transform 4 pts; reset/moveto/3x lineto/close |
  | 00552fa0 | 129b | path reset: free segment array (fn-table +0x10c / FUN_005c4d50), free node into DAT_00912a24 (+0x11c), FUN_00553e80 chain-free, FUN_005c4ba0 |
  | 00553030 | 192b | moveto: count==0 -> append type-0 seg (FUN_005c4bb0 hint 0x30190); else alloc sub-path node (0x301a1) + FUN_00553f40 + self-recurse |
  | 005530f0 | 244b | lineto: type-1 seg; tangent=delta*FastInvSqrt(len^2) (FUN_004c3b90), arclen+=FastSqrt (FUN_004c3b30); miter-average prev tangent (_DAT_005cc320) |
  | 005531f0 | 468b | cubic ctrl-poly: 3 consecutive type-2 segs with same tangent/arclen math; clears flatten flag +0xc |
  | 005533d0 | 440b | close-path: last!=first (DAT_005d757c cmp) -> closing type-1 seg; miter-blend first/last tangents; closed flag [2]=1 |
  | 00553590 | 134b | reset of the GLOBAL scratch path DAT_00912a28 (same body as 00552fa0) |
  | 00553620 | 625b | copy/flatten path: type-2 -> FUN_005538a0 subdivision (depth DAT_00912bf0); type-1 re-emit; type-0 copy 6 dwords; close if src closed |
  | 005538a0 | 935b | recursive cubic-Bezier De Casteljau subdivision: flatness vs _DAT_00912be8; emit type-1 or split at midpoints (x _DAT_005cc32c) and recurse 2x |
  | 00553c50 | 157b | single-path AABB: walk segs stride 6 dwords, min/max into bbox[4]; flattened-view via FUN_005c4d30/FUN_0055deb0 when [1]==-1 |
  | 00553cf0 | 262b | path-tree AABB: per sub-path flatten into DAT_00912a28 (inline reset + FUN_00553620) then FUN_00553c50; bbox -> (x,y,w,h) |
  | 00553e00 | 115b | recursive prune: drop 1-segment sub-path nodes, free into DAT_00912a24 |
  | 00553e80 | 100b | recursive chain free of sub-path list |
  | 00553ef0 | 71b | alloc fresh head node from DAT_00912a24 (hint 0x301a1) + FUN_00553f40 |
  | 005578a0 | 120b | module attach: FUN_004c2d90(0,0x112,..) engine plugin -> _DAT_00913270; RpAtomicRegisterPlugin(4,0x112,&LAB_00561f40,&LAB_00557d90,&LAB_00557cb0) -> DAT_00913274; FUN_004e7da0(0x112,..) stream cbs |
  | 005584c0 | 133b | scratch-buffer init: alloc 0x1c000 + 0x3000 via +0x108 hint 0x40180 -> DAT_007dc8b8/bc; init DAT_007dc8ac/b0/b4; rollback on 2nd-alloc fail |
  | 00558df0 | 579b | descriptor-driven container stream reader: FUN_004cc5e0 find-chunk + version gate desc+0xc; head alloc hint desc[+4]\|0x30000 + FUN_005c4ad0 array; per-item read-cb desc+0x28 + attach desc+0x18/0x1c; OOM error 0x80000013 (FUN_004d7ff0+FUN_004d8480); rollback w/ slist swap-remove |
  | 0055ac00 | 77b | set/clear id-bit (ushort obj+0x20) in bitmask table *(mgr+0x5c) |
  | 0055ac50 | 197b | id-bit clear on tables +0x64/+0x60; on clear: if active AND pair obj[9] active -> vtable deactivate (*obj)[+0x14], active-count *(mgr+0x4c)-- |
  | 0055ad30 | 160b | activate: skip if pair already active; count<cap(+0x48) -> vtable (*obj)[+0x10], set bit in +0x60, DAT_007dc8c0=0, count++; else one-shot flag DAT_007dc8c0=1 |
  | 0055b650 | 250b | rigid-body force/torque accumulate: force[3] += f (state+0x30 stride 0x20); torque += cross(f, p - body_pos), pos at +0x30 of 0x40-stride record under *(*(h)+0x10) |
  | 00561ee0 | 87b | one-shot plugin attach (gate DAT_00623fac==-1): FUN_004f0910(4,0x902,&LAB_00561f40,FUN_004d7ff0,FUN_004d7ff0) + FUN_004f0940(0x902, 3 cbs); shares &LAB_00561f40 with 005578a0 |
  | 0057bf30 | 407b | segment-vs-surface nearest point: FUN_005667c0 normalize + FUN_00566ea0 intersect; clamp to segment ends; re-query FUN_00566d50; returns param t or sentinel _DAT_005cc574 |
  | 0057c0d0 | 220b | same-side predicate: dot of two cross products vs DAT_005d757c -> 1/0 |
  | 0057c1b0 | 22b | thin wrapper: FUN_0055c380(obj, &DAT_005e5e50) |
  | 0057c210 | 13b | plugin-data getter: *(DAT_007dc8d8 + offset) |
  | 0057c270 | 50b | plugin attach: DAT_007dc8d8 = RpAtomicRegisterPlugin(4,0x901,&LAB_0057c2e0,FUN_004d7ff0,FUN_004d7ff0); PTR_LAB_00624058 = FUN_0057c2b0 |
  | 0057c2b0 | 36b | refresh: FUN_0057c440(h); *(h+0x30) -> FUN_004c0e50(*(*(h+0x30)+4)) |
  | 0057c370 | 42b | destroy: FUN_0055df90(h); FUN_0057c550(*(h+4)); fn-table +0x10c free |
  | 0057c420 | 29b | thin wrapper: FUN_0055bd80(st[0], st[2], 1, arg) |
  | 0057c440 | 102b | pose copy: copy 16 dwords from body state (**(st)+0x10)+idx*0x40; FUN_004c51a0(m, -refpos, 1) + FUN_004c45f0(m) |
  | 0057c550 | 61b | refcount-- at +0x50; 0 -> FUN_0055b930(h) + FUN_00564190(*(h+0x4c)) + fn-table free |
  | 005cb404 | 39b | FPU CW shim: save CW -> _DAT_00915000/_DAT_00915004; set (cw & 0xfffffcff) \| 0x3f (single precision, exceptions masked) |
  | 005cb42b | 6b | empty no-op (ret) |

- **For central re-classify**: reclass-OUT all 50 to
  `third-party-library[renderware]` (36) / `third-party-library[RenderWare-Physics-3.7]`
  (14), kept C1 (library residue, not reimplementation targets — same
  treatment as aq's 153). Do NOT rename on master (none renamed; read-only
  session). Tag-merge between the two RW tags is at the sweep's discretion.
- **U-IDs / S-IDs**: none minted (nothing plated, no inline [UNCERTAIN]
  markers created; reserved range U-9200..U-9499 untouched by this session).
- **render C1 drains by 50** via the reclass-OUT — with the other five ar
  sessions this CLOSES the render campaign.
- Files in the atomic commit: this fragment only. No hooks.csv writes, no
  re-classify, no master-Ghidra mutation (read-only MCP throughout).

## Drained

drained-by=sweep-20260604-0020; 0 plates, 0 bookmarks, 0 renames (ALL-SKIP: library_skip, rvas=NONE)
