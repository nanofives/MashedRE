# SCRIBE_QUEUE fragment — batch_ap session 2 (ap_s2)

Author-only promote-c2 pass (gameplay campaign batch 5/5 — FINAL, second slice
0x00565160-0x00566c10, RenderWare-Physics-3.7 region). Outcome: **ALL-SKIP —
zero C1->C2 plates authored.** Every one of the 28 candidates decodes as a
vendored RenderWare-Physics-3.7 primitive (library-confirm rule, applied
aggressively per the batch_ap header). No bucket dir was created (nothing to
plate). Central re-classify reclasses all 28 OUT to
third-party-library[RenderWare-Physics-3.7], kept C1.

## Queued

2026-06-03  ap_s2  bucket=re/analysis/bucket_gameplay_00565160_00566c10  confidence=C1->C2  rvas=NONE  library_skip=00565160:RenderWare-Physics-3.7,005651b0:RenderWare-Physics-3.7,00565200:RenderWare-Physics-3.7,00565260:RenderWare-Physics-3.7,00565550:RenderWare-Physics-3.7,00565570:RenderWare-Physics-3.7,00565590:RenderWare-Physics-3.7,005655a0:RenderWare-Physics-3.7,00565780:RenderWare-Physics-3.7,005659f0:RenderWare-Physics-3.7,00565ab0:RenderWare-Physics-3.7,00565bc0:RenderWare-Physics-3.7,00565c20:RenderWare-Physics-3.7,00565cd0:RenderWare-Physics-3.7,00565d50:RenderWare-Physics-3.7,00565ef0:RenderWare-Physics-3.7,00565fa0:RenderWare-Physics-3.7,00566050:RenderWare-Physics-3.7,00566200:RenderWare-Physics-3.7,005663a0:RenderWare-Physics-3.7,00566420:RenderWare-Physics-3.7,005665a0:RenderWare-Physics-3.7,005667c0:RenderWare-Physics-3.7,00566830:RenderWare-Physics-3.7,005668e0:RenderWare-Physics-3.7,00566aa0:RenderWare-Physics-3.7,00566b40:RenderWare-Physics-3.7,00566c10:RenderWare-Physics-3.7  note=ALL 28 candidates are vendored RW-Physics broadphase-octree/rigid-body-math/lifecycle primitives (subsystem_observed=third-party-library[RenderWare-Physics-3.7] for every RVA); 0 plated; needs_function_create=NONE (all 28 had function objects and decompiled cleanly); actual pool slot=Mashed_pool3 (pre-assigned; opened read-only cleanly on first attempt, no LockException; program_close issued)

## Notes for the sweep

- **Count**: 28 RVAs read end-to-end against live decomp (read-only,
  Mashed_pool3, session 0a7205f07f924319b64df00b843b9435, image_base
  0x00400000). **0 plates authored — `rvas=NONE` is deliberate** (the manifest
  builder's RVAS_RE will not parse it, so the row is skipped at manifest-build
  time; that is correct, there is nothing for the sweep to plate from this
  session). The deliverable of this session is the `library_skip` list for
  central re-classify.

- **POOL SLOT**: pre-assigned **Mashed_pool3** opened read-only on the FIRST
  attempt via `project_program_open_existing` with explicit
  `program_name="MASHED.exe"` (no LockException; the ao_s2 stale-lock
  tolerance held). `program_close` issued at end. Recorded in
  `.pool_slot_ap_s2`.

- **needs_function_create = NONE.** All 28 returned a function object and
  decompiled cleanly.

- **Drift check**: all 28 are their OWN first-field `gameplay,C1` rows in
  hooks.csv at session start (verified with anchored `^<rva>,` grep; they point
  at existing batch-y-s4/batch-y-s5/batch-z-s2 C1 plates in
  `re/analysis/bucket_00554010/`, `re/analysis/bucket_00565cd0/`, and
  `re/analysis/bucket_00565d50/`, which were read first and deepened — the
  library verdict below is the deepening).

- **LIBRARY-CONFIRM VERDICT — all 28 are one coherent RenderWare-Physics-3.7
  family**: the tail of the broadphase-octree node machinery (0x00565160..
  0x00565590), the rigid-body math kernel (inertia tensor / Jacobi eigen /
  bounding volumes, 0x005655a0..0x00566200), polynomial root-finding
  (0x00566420/0x005665a0), vector utilities (0x005663a0..0x00566830,
  0x00566b40/0x00566c10), and ref-counted lifecycle (0x005668e0/0x00566aa0).
  Four independent lines of evidence:

  1. **Named/confirmed-neighbour bracketing.** 0x005668e0 calls
     FUN_00564190 — the RW-Physics arena-free already cited by ao_s3 as the
     farfield-arena destroy callee (ap_s1 candidate band) — and
     FUN_0055bad0, reclassed-OUT to third-party-library[RenderWare-Physics-3.7]
     by the batch_ao s4 drain. The octree functions here are the direct
     continuation of the batch_ao all-skip pages and complete the same node
     object that FUN_00564c80 (insert/split) and FUN_005646c0 (overlap query)
     operate on per the batch-y C1 plates.
  2. **Shared struct family + constants.** The octree object offsets seen
     live this session (+0x79b*0x14 leaf records, +0x8020 location tags,
     +0x8820 chain successors, +0x9820 octant table, +0x981a record free
     head, +0xc00e node free head, +0xc010 pair-manager ptr, +0xc014 loose
     margin, +0xc018 filter ptr, +0xb810 group table) are the identical
     field set used by FUN_00564c80/FUN_005646c0 (insert/query, same band).
     All float constants resolve into the library's own .rdata pool:
     0x005cc320 (1.0f numerator), 0x005cc32c (0.5f), 0x005cc33c,
     0x005d757c (0.0 pivot), 0x005ceabc (epsilon ptr), 0x005dae94 (length
     threshold), 0x005e5470 (convergence tolerance) — the same pool cited
     across batch_ao/an RW-Physics verdicts.
  3. **Callee/caller set is exclusively vendored-band.** Callees across the
     28: FUN_00563f60 (pair-manager remove, ap_s1 band), FUN_005659f0 /
     FUN_00565780 / FUN_00566420 / FUN_00566830 / FUN_005667c0 (intra-slice),
     FUN_00546b10 (matrix->quat, 0x546xxx math library), FUN_004c3ac0
     (Vec3Magnitude, RW core), FUN_0055bad0 / FUN_00564190 (RW-Physics,
     reclassed-OUT/cited), and the DAT_007d3ff8+0x10c allocator vtable. The
     callers noted in the existing plates (FUN_00559ee0, FUN_0055abb0,
     FUN_0055ae50, FUN_0055ae70, FUN_0055af40, FUN_0055b4a0, FUN_00562660,
     FUN_00563940, FUN_005670a0, FUN_005739d0, FUN_005742c0) all sit inside
     the 0x0055x-0x0057x RW-Physics band. Zero application-page calls either
     direction.
  4. **Zero game-state access.** No candidate reads or writes any Mashed game
     global (no 0x006xxxxx-0x009xxxxx DAT). The only global touched anywhere
     in the 28 is DAT_007d3ff8 — the RW-Physics allocator fn-table (+0x10c
     free slot, at 0x005668e0), which the batch_ap header lists as an
     explicit library marker. By the decision rule (glue must read/write game
     state) none qualify as game/engine glue.

- **Per-RVA mechanical verdicts** (deepening the batch-y/z C1 plates; all
  read from live decomp this session):

  | RVA | size | mechanical role (RW-Physics) |
  |---|---|---|
  | 00565160 | 67b | octree leaf-chain push-front: splice param_3 as new head of per-node leaf record (+0x79b, 0x14 stride), old head into chain-successor table +0x8820 (masks 0x3ff/0xfffffc00) |
  | 005651b0 | 70b | octant primitive-count increment with 0x7c00 saturation guard, 16-bit table +0x9820 (10 entries/node), +0x400 step |
  | 00565200 | 94b | per-prim loose AABB read: tight AABB at prim*0x20 expanded by margin float +0xc014 into param_3 (min -margin at [0..2], max +margin at [4..6]) |
  | 00565260 | 747b | octree primitive remove: pair-table purge FUN_00563f60(+0xc010), unlink from root-chain ((node*5+0x2607)*4) or octant-chain (+0x9820) via +0x8820 successors, free-list push +0x981a, saturated-count (0x7c00) recompute walk, bottom-up empty-parent collapse (0x83ff marker) into node free-chain +0xc00e |
  | 00565550 | 22b | getter: per-prim group ushort +0xb810 |
  | 00565570 | 28b | setter: per-prim group ushort +0xb810 |
  | 00565590 | 15b | setter: filter-callback ptr +0xc018 |
  | 005655a0 | 468b | world inverse-inertia tensor: R*diag(I)*R^T from negated quaternion, in 7-float [Ix,Iy,Iz,qx,qy,qz,qw], symmetric 3x3 out with [3]/[6]/[7] symmetry copies |
  | 00565780 | 620b | Jacobi eigenvalue iteration for symmetric 3x3: identity init (0x3f800000), off-diagonal-squared convergence vs param_4^2, Givens sweeps via FUN_005659f0, returns remaining iteration count |
  | 005659f0 | 179b | Givens column-pair rotation on 3x3 row-major matrix (rows +0/+0xc/+0x18), c/s caller-supplied |
  | 00565ab0 | 272b | symmetric-matrix -> principal axes: Jacobi (0x19 iters, 0x358637bd threshold) then FUN_00546b10 matrix->quaternion; out [eig0,eig1,eig2,qx,qy,qz,qw]; 0 on non-convergence |
  | 00565bc0 | 88b | argmax(|x|,|y|,|z|) component index 0..2 |
  | 00565c20 | 174b | bounding-sphere expand to enclose point: distance via FUN_004c3ac0 (Vec3Magnitude, RW core), new center/radius = ((d-r)/2d shift, (d+r)*0.5) |
  | 00565cd0 | 118b | AABB -> center ((min+max)*0.5 at [0..2]) + half-diagonal length at [3] |
  | 00565d50 | 415b | triangle metric: 3 edge lengths, Heron semi-perimeter (sum*0.5, epsilon clamp vs PTR_DAT_005ceabc), area/s at out[3], edge-length-weighted vertex blend at out[0..2], returns max edge length |
  | 00565ef0 | 171b | AABB union: componentwise max into [0..2], min into [4..6] |
  | 00565fa0 | 167b | two-point swept AABB with margin param_4: per-axis max+margin [0..2] / min-margin [4..6] |
  | 00566050 | 425b | transformed AABB: |R|*halfExtents + 3x4-matrix transform of center (cols at 0/4/8, translation at 0xc..0xe) -> world max [0..2] / min [4..6] |
  | 00566200 | 411b | inverse-transformed AABB: subtract translation then transposed-rotate center, |R|^T*halfExtents -> local max/min |
  | 005663a0 | 128b | AABB extend to include point (per-axis min/max clamp) |
  | 00566420 | 382b | Newton-bisection polynomial root refiner on sign-change bracket [p1,p2]: 0x17 iteration cap, Horner eval of poly (param_5 degree) + derivative, Newton step with bisection fallback, convergence vs _DAT_005e5470 |
  | 005665a0 | 542b | polynomial real-root isolation: strips trailing zero coefficients, builds derivative cascade in-place ((i+1)*c[i+1]), per-level Horner sign-change bracketing -> FUN_00566420; returns root count in [param_2,param_3] |
  | 005667c0 | 109b | vec3 normalize with (0,0,1) fallback under epsilon PTR_DAT_005ceabc; returns length (inv*len2) |
  | 00566830 | 163b | perpendicular-vector builder: packed LUT 0x21312300 selects dominant |axis|, zeros it, swap/negate the other two components |
  | 005668e0 | 195b | ref-counted destroy: --(*(p1+0x40)+0x70); on 0: FUN_0055bad0 per 0x60-stride element, FUN_00564190 arena free, 5x allocator-vtable free via DAT_007d3ff8+0x10c (+0x14/+0x18/+0x24/+0x20/+0x28), free self, clear p1+0x40 |
  | 00566aa0 | 145b | min bounding metric over 0x60-stride elements: vtable[+0x24] result + 2*elem[+0x4c], 1e10 sentinel, cached at obj+0x7c |
  | 00566b40 | 198b | normalize-with-fallback-direction: len2 > _DAT_005dae94 -> normalize+return length; else perpendicular-of-param_3 (FUN_00566830 + FUN_005667c0) when param_4==0, or scaled param_3 renormalized |
  | 00566c10 | 141b | single-axis plane separating test with TOI output: signed distance comp[param_4]*p5-p6, intersect -> {0,1}/return 1, else stores distance + slope, 0xffffffff on no-impact |

- **For central re-classify**: reclass-OUT all 28 to
  `third-party-library[RenderWare-Physics-3.7]`, kept C1 (library residue, not
  reimplementation targets — same treatment as the batch_ao s4/s5/s6 all-skip
  rows). Do NOT rename on master (none were renamed; read-only session). No
  U-IDs / S-IDs minted (author-only; nothing plated, so no inline [UNCERTAIN]
  markers were created either). gameplay C1 still drains by 28 via the
  reclass-OUT.

## Drained

drained-by=sweep-20260603-2132; 0 plates, 0 bookmarks, 0 renames (ALL-SKIP: 28 library_skip, rvas=NONE)
