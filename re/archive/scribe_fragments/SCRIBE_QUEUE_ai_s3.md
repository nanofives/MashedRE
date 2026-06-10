# SCRIBE_QUEUE fragment — batch_ai session 3 (ai_s3)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  ai_s3  bucket=re/analysis/bucket_audio_005bf4d0_005c9770  confidence=C1->C2  rvas=005bf4d0,005bf660,005bf670,005bf6d0,005bf710,005bf7d0,005bf8a0,005bf8f0,005bfa20,005bfa40,005bfb20,005bfb90,005bfcc0,005bfd10,005bfe60,005bfed0,005bff40,005c7070,005c7330,005c7500,005c7570,005c75b0,005c75c0,005c7990,005c7a90,005c7ee0,005c7f80,005c7fb0,005c7fe0,005c8120,005c88b0,005c8910,005c8980,005c8eb0,005c90a0,005c9120,005c91f0,005c9220,005c9380,005c93a0,005c9490,005c94c0,005c95b0,005c9680,005c96d0,005c96e0,005c9730,005c9740,005c9770

## Notes for the sweep

- **Count**: 49 RVAs, 49 plates authored in the bucket dir. None drift-skipped
  (all 49 were `audio,C1` in hooks.csv at session start; none already C2+).
- **Subsystem confirmation**: all 49 **CONFIRMED `audio`** — no reclassifications.
  The bucket splits into two coherent audio sub-modules:
  - **0x005bf4d0..0x005bff40 (17 RVAs) — COM-based DirectShow/DirectSound audio
    backend.** Two distinct COM object classes share this band: object-A (ctor
    [[0x005bf710]], dual vtables `PTR_LAB_00635a80`/`PTR_LAB_00635a38`, size 0x164)
    and object-B (ctor [[0x005bfd10]], vtables `PTR_LAB_00635af8`/`PTR_LAB_00635ab0`,
    size 0x178). Standard IUnknown plumbing present: AddRef `InterlockedIncrement`
    (005bf670), QueryInterface returning `E_NOINTERFACE` 0x80004002 (005bf6d0).
    DirectShow domain is pinned by literal HRESULTs: `VFW_E_NO_ALLOCATOR`
    0x80040209 (005bfa20), 0x8004020a / 0x8004020b / 0x80040227 (VFW_E_ range),
    and `REFERENCE_TIME` /10000000 conversion (005bfb20). 005bff40 is the
    DirectSound source-caps init (matches the prior batch_aa note exactly).
  - **0x005c7070..0x005c9770 (32 RVAs) — software audio mixer.** Voice dequeue/
    reset under a `WaitForSingleObject`/`ReleaseSemaphore` lock on the shared
    mixer context (`*(this+0x9c)`); pan/volume gain with a table-driven sqrt
    transform (005c7330); sample-rate/pitch fixed-point (005c7500/005c9740);
    SIMD-variant mix-routine setup twins (005c7a90 / 005c8120); a voice-allocation
    **bit-array allocator** (005c9380/005c93a0/005c9490/005c94c0/005c95b0); two
    lazy singleton registries (005c9680/005c96e0 + shutdowns 005c96d0/005c9730);
    and the **saturating int32→int16 PCM output pack** 005c9770 (clamp
    [-0x800000,0x7fff00], `>>8`, saturate 0x8000/0x7fff) committed per-channel by
    005c9740. This is the "16-bit PCM saturated mix" backend referenced in memory.
- **batch_t MULTI tag does NOT apply here.** The hooks.csv `batch_t s6
  MULTI[audio-dll-mixer,rw-v3d-vector,msvc-seh-funclets,crt-fiddb]` note's
  `rw-v3d-vector` component refers to its 3 V3dTransform renames at
  0x005cb000/0x005cb07f/0x005cb0ef — **outside this bucket's range**. None of the
  32 0x005c7xxx-0x005c9xxx RVAs in this bucket are RW vector math, SEH funclets,
  or CRT; all are the software mixer. Keep `audio` for all 49.
- **Structural finding for the sweep**: `DAT_00617ff8` is a **byte log2/bit-length
  lookup table** (proven by [[0x005c93a0]] using it to convert a bit count to a
  bit-length index with per-octet biases -1/+7/+0xf/+0x17). The same table in
  005c7a90/005c8120 therefore computes `floor(log2)` of a capability value, not a
  routine-table index. Worth a one-line plate on the master if not already named.
- **Vendored/out-of-band callees referenced but NOT deep-plated** (pool & CRT &
  shared audio-singleton helpers): FUN_005aea00, FUN_005ae800, FUN_005ae920,
  FUN_005aea00, FUN_005aa560, FUN_005aba20, FUN_005ad420/005ad540/005ad570/005ad5f0,
  FUN_005aab00, FUN_005aaa00, FUN_005ab040, FUN_005aee20, FUN_005a9e10, FUN_004522d0
  (CRT free), FUN_005b0f10, FUN_005adf30 (IID-equal). Tagged per-call as
  audio-lib/CRT callees; left to the sweep / sibling sessions.
- **New uncertainties filed this session** (range U-7200..U-7219, all
  data-semantic / calling-convention, **NON-BLOCKING** for C2 of bit-identical
  leaves):
  - U-7200/U-7201/U-7203/U-7206 — identities of the GUID/IID constants at
    0x005d0b1c / 0x005d1124 / 0x005d0b3c and their COM-interface vtable contracts.
  - U-7202 — meaning of constructor mode value `param_4 == 2` (005bf710).
  - U-7204 — 005bfb20's two out-pointers appear as unbound `puStack_4`/
    `unaff_retaddr` (original `__thiscall` register operands).
  - U-7205 — 005bfe60 create-failure path returns 0x8004020a without a visible
    LeaveCriticalSection (possible SEH/__finally not recovered by the decompiler).
  - U-7207..U-7219 — data-semantic field/constant meanings in the mixer (pan-law
    scalars, flag bits, fixed-point formats, registered-table contents, unrolled
    loop sample accounting). All arithmetic/control-flow is determinate.
- **No new S-IDs minted.** 005c7990 references the pre-existing **S-3190**
  (`FUN_005aa560` global dispatcher) already filed in hooks.csv; do not duplicate.
- **Pool-slot note**: pre-assigned `Mashed_pool2` had a live/leaked lock at
  session start (stamped 00:14:49; server `program_list_open`=0). First fallback
  `Mashed_pool4` was a STALE clone (the 0x005bf000/0x005c7000 region was undefined
  `DataDB` — functions not yet defined). Final working slot = **`Mashed_pool10`**
  (.rep Jun 1 23:47; all 49 functions defined). Recorded in `.pool_slot_ai_s3`.
  program_close issued at session end.
- Files left UNTRACKED (bucket dir + this fragment + .pool_slot_ai_s3). No git
  commit, no re-classify, no build, no Frida — per author-only mission.
