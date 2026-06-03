# SCRIBE_QUEUE fragment — batch_al session 5 (al_s5, author-only promote-c2)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (scripted ghidra-sweep + re-classify) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  al_s5  bucket=re/analysis/bucket_gameplay_00418e70_0041a8d0  confidence=C1->C2  rvas=00418e70,00418f30,00418f40,00418f50,00419090,004190f0,00419100,00419480,004194f0,00419530,004196f0,004197e0,00419a00,00419fd0,00419ff0,0041a060,0041a180,0041a400,0041a4a0,0041a500,0041a550,0041a5b0,0041a5d0,0041a840,0041a8b0,0041a8d0

## Session metadata

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. None drift-skipped (all
  were C1/gameplay in hooks.csv at session start; none already C2+).
- **Pool slot**: pre-assigned **Mashed_pool5 POISONED** — first `program_open` failed
  with a LockException and **leaked an in-JVM lock** (pool5.lock appeared on disk while
  the shared server held 0 sessions, so it could not be released; see memory
  feedback_mcp_leaked_project_lock). Fell back to **Mashed_pool8** (verified-free; not
  pool4, not pool3=al_s4, not pool6=al_s6). Opened read-only OK, `program_close` clean.
  `.pool_slot_al_s5` records ACTUAL slot = Mashed_pool8.
- **Confidence target**: C2 (author-only — no re-classify/hooks.csv edits; central
  finalize promotes C1→C2).
- **library_skip**: NONE. All 26 are genuine game code (addresses 0x00418e70..0x0041a8d0,
  below the CRT / D3DX9-PSGP / qhull bands). No RW/CRT primitive decoded verbatim.
- **needs_function_create**: NONE. All 26 have Ghidra function objects; full decomp
  available for each.

## Subsystem confirmation (gameplay hypothesis held for all 26)

All 26 **confirmed `gameplay`** — no reclassification. The bucket is one cohesive
weapons-effects + ghost-vehicle subsystem (HUD-overlay effects rendered through RW
camera/matrix/sprite-batch primitives), split into two clusters:

- **Weapons-effects (0x00418e70..0x0041a1d6)** — driven by master init **FUN_0041a060**
  (loads exocet.dff / airstrike / target / shockwave / crosshair / smoke assets; builds
  4 launchers + 4 missile clones + 2 target reticules), per-frame **FUN_0041a180** (caller
  FUN_004189a0), render **FUN_00419530** (caller FUN_004189c0), reset **FUN_004194f0**,
  teardown **FUN_004196f0**. Includes the 64-slot smoke-puff pool (FUN_00418e70 draw /
  FUN_00418f50 spawn / FUN_00419ff0 init / FUN_00418f30/f40 batch wrappers), the
  3-state target-reticule SM (FUN_004197e0), the target visual update (FUN_00419100),
  the 6-handle target render (FUN_00419480), the target constructor (FUN_00419090), and
  the **4-state missile-in-flight SM FUN_00419a00** (homing + explosion FX).
- **Ghost-vehicle replay slots (0x0041a400..0x0041a956)** — 196-byte (0xc4-stride) records
  based at &DAT_0063c630. 2-slot init **FUN_0041a8d0** (caller FUN_0040d110, track-load
  chain) loads "ghost.dff" per slot; constructor FUN_0041a400; setters FUN_0041a4a0
  (transform), FUN_0041a500 (vec3 tint-scale), FUN_0041a550 (4-dword secondary tint),
  FUN_0041a5b0 (+0xbc dword), FUN_0041a8b0 (+0xc0 enabled); render FUN_0041a5d0; destroy
  FUN_0041a840 (caller FUN_0041a960 = Ghost::Init). Setter callers are FUN_0041a9d0 /
  FUN_0041aac0 / FUN_0041ac60 / FUN_0041ad00 / FUN_0041a9b0 (the Ghost::* glue, outside
  this bucket's address range).

## Resolved-name notes for the sweep (master Ghidra already carries these names)

- **FUN_004e6e00 = RpClumpDestroy** (used by FUN_004196f0 teardown). Sharpens the C1
  "RpAtomicDestroy" guess.
- **FUN_004e6100 = RpAtomicGetWorldBoundingSphere** (used by FUN_00419a00 case 1). This
  **corrects** the prior C1 plate (bucket_00412130/0x00419a00.md), which misread it as
  "particle color" — it returns the atomic's {x,y,z,radius} sphere, copied to the trail
  particle spawn at FUN_00484cf0.
- **C3-leaf reuse** in FUN_00419a00: FUN_004c3ac0 = Vec3Magnitude (landed C3 hook
  0x004c3ac0), FUN_004c39b0 = Vec3Normalize, FUN_004b4510 = RandomFloat (util C2).

## Uncertainties carried (bare [UNCERTAIN] in plates — NOT minted; central re-classify mints from U-7700..U-7999)

- 0x00419090 — `local_18[6]` is read uninitialized (FUN_004b3fc0 output buffer); `0x40`
  arg to FUN_004b52c0 is a texture-filter-mode candidate, unverified vs RW catalog.
- 0x00419100 — target sub-record offsets +0x50/+0x58/+0x60 and state codes 1/2/4 classifier
  semantics not resolvable here.
- 0x00419480 — vtable slot +0x48 (render vs destroy) depends on the concrete handle vtable.
- 0x004194f0 — Ghidra "callee-returns-pointer + compare-to-end" loop shape (behavior
  unaffected).
- 0x00419530 — 0.6f/0.45f (&local_88) passed to FUN_004c1c80 (fog/clip params), role unknown.
- 0x004197e0 — per-channel byte tables &DAT_007f150b / &DAT_007f104b (stride 0x4c) identity;
  -NAN no-lock sentinel.
- 0x00419a00 — FUN_00418de0 (case-4 final destructor) not in bucket.
- 0x00419ff0 — `0x895` (2197) install arg meaning.
- 0x0041a400 — `0x40` texture-filter-mode arg to FUN_004b52f0 (same as 0x00419090).
- 0x0041a5d0 — exact mapping of the 8 ghost sub-mesh handles vs the 4-blend tint chain.
- 0x0041a840 — non-sequential destroy order implies clump-tree dependency ordering.

**Resolved this pass (drop from C1):** 0x00418e70 age predicate `(a<b) != (a==b)` ≡ `age <=
_DAT_005cc950`; 0x0041a4a0 slot +0x0c is the RwMatrix flags word (0x3c-byte identity matrix).

## Flags

- **No new S-IDs** minted. Stub callees listed per-plate (`## Stubs encountered`) are
  outside the bucket; leave stub resolution to the central sweep.
- Files left as the session commit (bucket dir + this fragment + `.pool_slot_al_s5`).
  No hooks.csv / STUBS / UNCERTAINTIES / DEFERRED / shared SCRIBE_QUEUE.md edits, no
  re-classify, no build, no Frida — per author-only mission.
- Binary anchor: opened original/MASHED.exe via Mashed_pool8 clone (image_base 00400000,
  PE32). Did not re-hash; clone is of the pinned original.
