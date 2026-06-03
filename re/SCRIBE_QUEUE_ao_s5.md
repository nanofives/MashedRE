# SCRIBE_QUEUE fragment — batch_ao session 5 (ao_s5)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers.

## Queued

2026-06-03  ao_s5  bucket=re/analysis/bucket_gameplay_0055bd80_0055e330  confidence=C1->C2  rvas=  note=subsystem_observed=third-party-library[RenderWare-Physics-3.7] for ALL 26 candidates; ZERO C1->C2 plates authored; library_skip=0055bd80:RenderWare-Physics-3.7,0055bde0:RenderWare-Physics-3.7,0055c000:RenderWare-Physics-3.7,0055c0f0:RenderWare-Physics-3.7,0055c230:RenderWare-Physics-3.7,0055c2d0:RenderWare-Physics-3.7,0055c380:RenderWare-Physics-3.7,0055c3e0:RenderWare-Physics-3.7,0055c490:RenderWare-Physics-3.7,0055c540:RenderWare-Physics-3.7,0055cd40:RenderWare-Physics-3.7,0055dca0:RenderWare-Physics-3.7,0055dd60:RenderWare-Physics-3.7,0055ddd0:RenderWare-Physics-3.7,0055de20:RenderWare-Physics-3.7,0055de60:RenderWare-Physics-3.7,0055deb0:RenderWare-Physics-3.7,0055ded0:RenderWare-Physics-3.7,0055df90:RenderWare-Physics-3.7,0055dff0:RenderWare-Physics-3.7,0055e050:RenderWare-Physics-3.7,0055e190:RenderWare-Physics-3.7,0055e200:RenderWare-Physics-3.7,0055e2d0:RenderWare-Physics-3.7,0055e300:RenderWare-Physics-3.7,0055e330:RenderWare-Physics-3.7; pool slot=Mashed_pool12 (pre-assigned, opened read-only cleanly)

## Notes for the sweep

- **Count**: 26 candidates examined end-to-end (decomp_function on every RVA in
  Mashed_pool12, read-only). 0 plated, 26 library_skip, 0 drift-skipped (all 26
  confirmed `gameplay,C1` in hooks.csv via anchored `^<rva>,` grep at session
  start), 0 needs_function_create (every RVA has a function object).
- **Library-confirm evidence** (rule applied per RVA; none reads/writes game
  state — every function operates exclusively on parameter-passed records; no
  application-code callees, no 0x006xxxxx/0x007xxxxx game globals other than the
  RW allocator table):
  - **RW allocator fn-table `DAT_007d3ff8`** (slots +0x108 alloc / +0x10c free /
    +0x114 calloc, RW memory-hint constant `0x30900` at every alloc call site):
    used by 0055dca0 (0x0055dcde, 0x0055dcf1, 0x0055dd0f), 0055dd60
    (0x0055dd71..0x0055ddb1, 4 frees), 0055ded0 (0x0055df03), 0055df90
    (0x0055dfb8, 0x0055dfd0), 0055e190 (0x0055e1a0).
  - **Shape-object virtual dispatch through `+0x5c` vtable** (no game state):
    0055bd80 (slot +0x10 at 0x0055bdc1, conditional body-relative matrix via
    RW-core FUN_004c4600), 0055bde0 (slot +0x28 at 0x0055bf0e, world<->local
    3x3 relativization of two points + result re-projection), 0055c000 (slot
    +0x14 at 0x0055c04f, support-point query w/ rotate-in/transform-out),
    0055c230 (slot +0x18 at 0x0055c280, then vertex-array re-transform via
    FUN_0055c0f0), 0055c2d0 (slot +0x20 at 0x0055c33f, interval projection with
    d-offset `local_10` added to both outputs), 0055c3e0 (slot +0x14 twice,
    0x0055c3eb + 0x0055c45a, extremal projection both directions; negation
    const `_DAT_005cc33c`).
  - **Shape-record ctors/setters/getters** (pure param writes): 0055c380
    (init: consts 0x3e4ccccd=0.2f x2 at 0x0055c3b3/0x0055c3ba,
    0x3f800000=1.0f x3, flag-or 0x20003 at 0x0055c3c4, vtable=param_2),
    0055c490 (`*(p1+0x48)=p2` at 0x0055c495), 0055c540 (box-shape ctor:
    FUN_0055c380 with vtable `&DAT_005e4f50` at 0x0055c545, `+0x4c=p2`),
    0055cd40 (per-component sign-select of box half-extents +0x40/+0x44/+0x48
    against `DAT_005d757c`, scale by `_DAT_005cc320`/`_DAT_005cc33c` — box
    support-direction function), 0055deb0 (getter `+4`), 0055e200
    (setter `*p1=p2`).
  - **Contact/pair-record lifecycle** (RW alloc + param-struct init only):
    0055dca0 (contact-cache ctor; callees FUN_00559b50/FUN_0055f450 same
    RW-Physics page), 0055dd60 (dtor; callees FUN_0055fdd0/FUN_00559ba0),
    0055ded0 (pair-record ctor, 0xc-stride array zero-fill), 0055df90
    (free-list walk at +0x2c, link via slot[4], then 2 frees), 0055e2d0
    (pop-from-pool FUN_00559c00 + init FUN_0055e330), 0055e300 (release:
    refcount-dec FUN_0055e190 + return-to-pool FUN_00559c20), 0055e330
    (pair-record init, 0x26 ints, +0x6c=0x3f800000=1.0f at 0x0055e394,
    refcount-inc of param_3), 0055e190 (refcount dec, free at zero).
  - **Generic slot-array utilities** (param-struct only): 0055ddd0 (insert into
    first free slot, cap at +0x10), 0055de20 (iterate w/ callback param_2,
    early-out on 0), 0055de60 (find+remove by value).
  - **Misc**: 0055dff0 (bitfield test `(*(p1+0x24)+0x60)[idx>>5] &
    1<<(idx&0x1f)` on pair-bucket index +0x20, then tail-call FUN_0055b800 —
    ao_s4-range sibling), 0055e050 (vtable slot +0x18 bbox query into
    FUN_0055ab30 — ao_s4-range broad-phase sibling).
  - **Neighbourhood corroboration**: batch_aj reclassed 0055dc70 ->
    RW-Physics; batch_ak reclassed 00559c40/0055ae70/0055b940/0055bab0/0055c4a0
    -> RW-Physics. All 26 here are the same family (existing batch_w C1 plates
    in re/analysis/bucket_00557fb0/ already label them physics-rigid-body /
    physics-collision / physics-contact with no game-state citations).
- **Central re-classify action**: reclass-OUT all 26 to
  third-party-library[RenderWare-Physics-3.7], KEEP C1 (library residue, not
  reimplementation targets). gameplay C1 drains by 26 with 0 C2 promotions from
  this bucket — the outcome the batch_ao header predicted for this slice.
- **No bucket dir created** (zero plates to author; manifest builder should see
  rvas= empty / plated 0 / missing_md 0 for this fragment).
- **No U-IDs / S-IDs minted**; no new uncertainties — every verdict is from
  literal decomp of the function body. No hooks.csv / tracker writes, no master
  Ghidra writes (read-only session, program_close at end).
