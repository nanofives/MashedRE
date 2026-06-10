# SCRIBE_QUEUE fragment — batch_ao session 4 (ao_s4)

Author-only promote-c2 pass (gameplay campaign batch 4/~5, fourth slice
0x0055a9a0-0x0055bd70, RenderWare-Physics-3.7 region). Outcome: **ALL-SKIP —
zero C1->C2 plates authored.** Every one of the 26 candidates decodes as a
vendored RenderWare-Physics-3.7 primitive (library-confirm rule, applied
aggressively per the batch_ao header). No bucket dir was created (nothing to
plate). Central re-classify reclasses all 26 OUT to
third-party-library[RenderWare-Physics-3.7], kept C1.

## Queued

2026-06-03  ao_s4  bucket=re/analysis/bucket_gameplay_0055a9a0_0055bd70  confidence=C1->C2  rvas=NONE  library_skip=0055a9a0:RenderWare-Physics-3.7,0055ab30:RenderWare-Physics-3.7,0055abb0:RenderWare-Physics-3.7,0055ad20:RenderWare-Physics-3.7,0055ade0:RenderWare-Physics-3.7,0055ae50:RenderWare-Physics-3.7,0055af40:RenderWare-Physics-3.7,0055af60:RenderWare-Physics-3.7,0055af70:RenderWare-Physics-3.7,0055af80:RenderWare-Physics-3.7,0055afc0:RenderWare-Physics-3.7,0055b030:RenderWare-Physics-3.7,0055b060:RenderWare-Physics-3.7,0055b080:RenderWare-Physics-3.7,0055b4a0:RenderWare-Physics-3.7,0055b750:RenderWare-Physics-3.7,0055b800:RenderWare-Physics-3.7,0055b860:RenderWare-Physics-3.7,0055b930:RenderWare-Physics-3.7,0055ba90:RenderWare-Physics-3.7,0055bac0:RenderWare-Physics-3.7,0055bad0:RenderWare-Physics-3.7,0055bae0:RenderWare-Physics-3.7,0055bb70:RenderWare-Physics-3.7,0055bbf0:RenderWare-Physics-3.7,0055bd70:RenderWare-Physics-3.7  note=ALL 26 candidates are vendored RW-Physics rigid-body/scene API (subsystem_observed=third-party-library[RenderWare-Physics-3.7] for every RVA); 0 plated; needs_function_create=NONE (all 26 had function objects and decompiled cleanly); actual pool slot=Mashed_pool10 (pre-assigned; opened read-only cleanly, no LockException; program_close issued)

## Notes for the sweep

- **Count**: 26 RVAs read end-to-end against live decomp (read-only,
  Mashed_pool10, session 6c4e66b49dd348fa94327fca6069d562, image_base
  0x00400000). **0 plates authored — `rvas=NONE` is deliberate** (the manifest
  builder's RVAS_RE will not parse it, so the row is skipped at manifest-build
  time; that is correct, there is nothing for the sweep to plate from this
  session). The deliverable of this session is the `library_skip` list for
  central re-classify.

- **POOL SLOT**: pre-assigned **Mashed_pool10** opened read-only on the FIRST
  attempt via `project_program_open_existing` with explicit
  `program_name="MASHED.exe"` (no missing-program_name leak, no LockException;
  the an_s6 stale-lock tolerance held). `program_close` issued at end. Recorded
  in `.pool_slot_ao_s4`.

- **needs_function_create = NONE.** All 26 returned a function object and
  decompiled cleanly.

- **Drift check**: all 26 are their OWN first-field `gameplay,C1` rows in
  hooks.csv at session start (verified with anchored `^<rva>,` grep; all point
  at existing batch_w-s5 C1 plates in `re/analysis/bucket_00557fb0/`, which
  were read first and deepened — the library verdict below is the deepening).

- **LIBRARY-CONFIRM VERDICT — all 26 are one coherent RenderWare-Physics-3.7
  rigid-body/scene API family.** Four independent lines of evidence:

  1. **Named-neighbour bracketing.** hooks.csv already carries FidDB/manual
     names for the interleaved neighbours, all reclassed-OUT to
     third-party-library[RenderWare-Physics-3.7] by batch_ak s3
     (sweep-20260603-0116): `0055ae70 PhysicsSceneBodySetShapeFlags`,
     `0055b940 PhysicsBodySetMass` (sits BETWEEN candidates 0055b930 and
     0055ba90), `0055bab0 PhysicsBodySetDamping` (sits between 0055ba90 and
     0055bac0/bad0/bae0), `00559c40 PhysicsSceneBodyRegister`,
     `0055c4a0 PhysicsBodySetRestitution`. The 26 candidates are the unnamed
     remainder of the same contiguous API surface.
  2. **Shared struct family + constants.** The owner-vtable-at-+0x5c dispatch
     shape appears in 0055ba90/bad0/bae0/bb70/bbf0/bd70 (virtual calls
     `(**(code**)(*(int*)(p1+0x5c)+4/8/0xc/0x10))`) — same record as the named
     PhysicsBody setters. The damping default `0x3c23d70a` (0.01f) written by
     0055b080 (`param_1[5]/[6]`) and 0055b860 (`param_1[0xd]/[0xe]`) is the
     exact constant PhysicsBodySetDamping (0055bab0) is called with per its
     hooks.csv note. Scene slot-bitmap ops in 0055a9a0/ab30/abb0/ade0
     (`1 << (idx & 0x1f)` over word table at scene+0x60/+0x70/p1[0x1a]) match
     PhysicsSceneBodyRegister/SetShapeFlags.
  3. **Callee set is exclusively vendored-band.** Every callee across the 26
     lands in 0x00546b10 (quat-from-matrix), 0x004c4600/0x004c51a0/0x004c52f0
     (RenderWare core matrix ops), or 0x00559bc0/00559be0/00564040/00564190/
     00564c80/00565200/00565260/00565550/00565590/005655a0 (same RW-Physics
     band). Zero calls into application pages.
  4. **Zero game-state access.** No candidate reads or writes any Mashed
     game global (no 0x006xxxxx-0x009xxxxx DAT). The only data refs are float
     constants in the 0x005cc320/0x005cd0c8/0x005cc558/0x005d757c .rdata pool
     (1.0f / epsilon / scale), i.e. the library's own constant pool. By the
     batch_ao decision rule (glue must read/write game state) none qualify as
     game/engine glue.

- **Per-RVA mechanical verdicts** (deepening the batch_w C1 plates in
  `re/analysis/bucket_00557fb0/`; all read from live decomp this session):

  | RVA | size | mechanical role (RW-Physics) |
  |---|---|---|
  | 0055a9a0 | 397b | broad-phase pair-candidate generator: clears scene visited-bitmap (+0x70), per input shape queries spatial index FUN_00564040(scene+0x54,...), 2-bit collision-filter test on scene bitmaps +0x5c/+0x64, emits 0x14-byte pair records (bodyA,0x8000,bodyB,0x8000,0) with cap early-exit |
  | 0055ab30 | 117b | scene-shape refresh: if shape bit set in bitmap p1[0x1a] -> FUN_00565260(scene,idx); if param_3 -> FUN_00564c80(scene,idx,param_3) + set bit |
  | 0055abb0 | 73b | conditional shape query: bit test in p1[0x1a] -> FUN_00565200(scene,idx,param_3), returns param_3 or 0 |
  | 0055ad20 | 12b | setter `*(p1+0xc)=p2` |
  | 0055ade0 | 102b | scene-body remove: bitmap test at owner+0x60, virtual destroy vtable[+0x14], clear bit, `--*(p1+0x4c)` |
  | 0055ae50 | 27b | thunk -> FUN_00565550 |
  | 0055af40 | 25b | thunk -> FUN_00565590(*p1,p2) |
  | 0055af60 | 8b | getter `*(p1+0x2c)` |
  | 0055af70 | 8b | getter `*(p1+0x3c)` |
  | 0055af80 | 59b | ForAll-style iterator over ptr table (base +0x50, count +0x44) with callback early-exit |
  | 0055afc0 | 98b | ForAll-style iterator over inline records (base +0x34, stride 0x9c, count +0x40), gated by ownership + bitmap +0x6c |
  | 0055b030 | 46b | body create: pop free record FUN_00559bc0(scene) + init FUN_0055b080 |
  | 0055b060 | 22b | body destroy: push FUN_00559be0(*p1,p1), return 1 |
  | 0055b080 | 1046b | body initialize from descriptor: scene SoA arrays via *(*p1+0x10) (+0x18 flags, +0x20/+0x28 inertia + 1.0f/SQRT inverse at stride 0x30, +0x50 velocity at 0x10), damping defaults 0x3c23d70a, identity-or-copy 0x40-stride matrix, calls FUN_0055b4a0 / FUN_00546b10 / FUN_004c51a0 |
  | 0055b4a0 | 425b | inertia-tensor pair: epsilon guard DAT_005d757c, FUN_005655a0 builds 3x3 from triple and from 1.0f/SQRT inverse triple, writes both 0x30-stride matrices (+0x20/+0x28) |
  | 0055b750 | 172b | point velocity: v + omega x r from body matrix (0x40-stride, pos +0x30) and angular-vel array (0x20-stride) |
  | 0055b800 | 92b | body set-transform: FUN_004c52f0 (matrix copy), FUN_004c51a0 (update), FUN_00546b10 (quat sync) |
  | 0055b860 | 202b | shape-record ctor: owner ptr, identity 0x3f800000 fields, damping 0x3c23d70a x2, zeroed links |
  | 0055b930 | 14b | thunk -> FUN_0055bad0 |
  | 0055ba90 | 27b | destroy: virtual vtable[+4] on owner at +0x5c, then FUN_00564190(p1), return 1 |
  | 0055bac0 | 13b | u16 setter `*(p1+0x5a)=p2` |
  | 0055bad0 | 13b | tail-call vtable[+4] via owner +0x5c |
  | 0055bae0 | 132b | virtual vtable[+8] (get point ptr), then in-place 4x3 matrix transform of the vec3 |
  | 0055bb70 | 114b | wrapper: flag tests (owner+0x40 bit1, self+0xc bit 0x20000); optional FUN_004c4600 (matrix concat into local) then FUN_0055bae0 |
  | 0055bbf0 | 378b | virtual vtable[+0xc] (support/extent query), FUN_00546b10 quat, rotates ray/plane params (p2+0xc..0x18) into body space, transforms out point by 4x3 matrix |
  | 0055bd70 | 14b | tail-call vtable[+0x10] via owner +0x5c |

- **For central re-classify**: reclass-OUT all 26 to
  `third-party-library[RenderWare-Physics-3.7]`, kept C1 (library residue, not
  reimplementation targets — same treatment as batch_ak s3's
  PhysicsBody*/PhysicsScene* rows). Do NOT rename on master (none were renamed;
  read-only session). No U-IDs / S-IDs minted (author-only; nothing plated, so
  no inline [UNCERTAIN] markers were created either). gameplay C1 still drains
  by 26 via the reclass-OUT.

## Drained

drained-by=sweep-20260603-2210; 0 plates (all-skip fragment; library_skip list consumed by central re-classify)
