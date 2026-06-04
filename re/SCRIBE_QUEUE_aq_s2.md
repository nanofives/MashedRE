# SCRIBE_QUEUE fragment — batch_aq session 2 (aq_s2)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep + central re-classify) writes hooks.csv / trackers.

## Queued

2026-06-03  aq_s2  bucket=re/analysis/bucket_render_004dc750_004e0920  confidence=C1->C2  rvas=NONE  note=ALL-SKIP 26/26 vendored RenderWare D3D9 driver (rwd3d9 raster/image-conversion + dynamic-VB/IB module); library_skip=004dc750:renderware,004dc8a0:renderware,004dc8e0:renderware,004dc970:renderware,004dc9e0:renderware,004dcb80:renderware,004dcbb0:renderware,004dcd30:renderware,004dcd50:renderware,004dd0b0:renderware,004dd140:renderware,004dd150:renderware,004ddcb0:renderware,004ddd60:renderware,004ddfb0:renderware,004de1b0:renderware,004de390:renderware,004de800:renderware,004dee20:renderware,004dee40:renderware,004df120:renderware,004df750:renderware,004df870:renderware,004df8a0:renderware,004e08b0:renderware,004e0920:renderware; actual pool slot=Mashed_pool13 (pre-assigned Mashed_pool3 threw a REAL LockException — likely in-JVM poisoned by this session's own first malformed open missing program_name, the exact ao_s2/pool2 precedent; fallback pool11 ALSO LockException; pool13 opened read-only cleanly)

## Notes for the sweep / central re-classify

- **Count**: 26 candidates → 0 plated, 26 library_skip (kept C1, reclass-OUT to
  third-party-library[renderware]). 0 drift-skips (all 26 confirmed `render,C1` via
  anchored `^<rva>,` grep at session start). 0 needs_function_create (every RVA has a
  function object; all 26 decompiled cleanly on Mashed_pool13).
- **library_skip evidence (per-RVA, library-confirm rule)** — the whole bucket is the
  RenderWare D3D9 driver's raster/image-conversion + dynamic-vertex-buffer module:
  - **Dynamic-VB suballocator family** 004dc750 (free+coalesce on freelists
    DAT_007d70b8/DAT_007d70b0 via engine fn-table DAT_007d3ff8+0x118 calloc-hint /
    +0x11c free, memhint 0x30411), 004dcd50 (module init: 4× FUN_004cc7f0
    freelist-create flag 0x40411 → DAT_007d70b4/b8/bc/c4), 004dc8a0 (ring index
    DAT_007d70c8 advance mod 4), 004dc8e0 (4-slot invalidate + COM Release vtable+8
    walk of DAT_007d70c0 under caps flag DAT_00911fbc&0x10000), 004dc970 (post-Reset
    re-create via device DAT_007d4110 vtable+0x68 CreateVertexBuffer usage 0x208),
    004dc9e0 (ring (re)create 0x40000=256KB/slot via FUN_004dcaa0, 1-vs-4-slot per
    DAT_00911fbc&0x10000), 004dcb80 (list deactivate), 004dcbb0 (ring Lock allocator,
    vtable+0x2c Lock flags 0x2800 NOOVERWRITE / 0x1800 DISCARD), 004dcd30 (thin
    vtable+0x30 Unlock wrapper). All globals are RW driver-instance state
    (0x007d70xx/0x007d4xxx); no game-state access.
  - **Raster/image conversion family** 004dd150 (texture/cubemap mem-size calc via
    raster plugin offset DAT_00911ae4 + GetLevelCount/GetLevelDesc/LockRect/UnlockRect
    COM ladders), 004ddcb0 (raster size clamp to caps DAT_00911ff8/ffc + pow2 flags
    _DAT_00911fdc), 004ddd60 / 004ddfb0 (pixel pack/unpack format ladders 0x100=1555
    0x200=565 0x300=4444 0x400=LUM8 0x500=8888 0x600=888 0xa00=555 with RW error-set
    idiom FUN_004d7ff0(0x8000000d)+FUN_004d8480), 004de1b0 (raster→image convert with
    LITERAL RW driver error strings s_Conversion_from_compressed_raste_006184d8 /
    _non_palettised_r_00618490 / _to_4bit_i_00618458), 004de390 (raster→32bpp ladder
    keyed on D3DFMT 0x15/0x16/0x23 at raster+DAT_00911ae4+0x18), 004de800
    (D3DFMT→decode-thunk dispatch 0x15..0x51 → LAB_004deb20..LAB_004dee00),
    004dee20 (D3DFMT_L16 decode leaf), 004dee40 (alpha-histogram format-suggest
    using caps DAT_00911f94 + format-support query FUN_004d38b0), 004df120
    (image→raster convert; error string s_Conversion_to_compressed_rasters_00618510;
    palette LUT via encoder fn-ptr), 004df750 (D3DFMT→encoder fn-ptr dispatch),
    004df870 (X1R5G5B5 encode leaf), 004df8a0 (R5G6B5 encode leaf).
  - **2D/UI pipeline buffer pair** 004e0920 (CreateIndexBuffer device vtable+0x6c,
    0x1fffe indices, + 3 shared VBs via FUN_004cb8a0 into DAT_007d7110/7114/7118,
    descriptor flag from DAT_00911fbc bit 15) / 004e08b0 (matching teardown: COM
    Release DAT_007d710c + 3× FUN_004cba80 frees).
  - **Driver support leaves** 004dd0b0 (CPUID 0x80000000/0x80000001 extended-feature
    probe via EFLAGS ID-bit 0x200000 toggle; sibling of FUN_004dcf90/004dcff0/004dd050
    CPU-detect family, result stored at DAT_007d4594), 004dd140 (already named
    Direct3DCreate9 in Ghidra — IAT thunk to d3d9.dll!Direct3DCreate9, thunk=true,
    single indirect JMP; tagged renderware as part of the driver band — central
    re-classify may prefer a d3d9-import tag, flagging for the human).
- **Zero game-state reads/writes across all 26**: every global cited is RW
  engine-instance (DAT_007d3ff8 fn-table, DAT_007d4110 device, DAT_007d4594),
  RW driver module state (DAT_007d70xx/DAT_007d71xx), the rwd3d9 caps/plugin block
  (DAT_00911ae4/00911b00/00911f94/00911fbc/00911fdc/00911ff8/00911ffc — driver
  .data, not game globals), or RW static tables/strings (DAT_005d8bf5, 0x00618xxx).
- **Bucket dir** re/analysis/bucket_render_004dc750_004e0920/ intentionally NOT
  created — 0 plates (ap precedent: ALL-SKIP sessions ship only the fragment).
- **Pool note**: pre-assigned Mashed_pool3 threw
  ghidra.framework.store.LockException on open. First open attempt of this session
  was malformed (missing program_name) and errored — per the ao_s2/pool2 precedent
  that likely leaked an in-JVM lock; treat pool3 as suspect for future batches.
  Fallback Mashed_pool11 ALSO threw LockException (on-disk .lock written 21:46:20,
  possibly a sibling session's fallback). Session ran read-only on Mashed_pool13;
  program_close called at end.
- No hooks.csv / STUBS.md / UNCERTAINTIES.md / DEFERRED.md / re/SCRIBE_QUEUE.md
  writes. No master-project mutation. No U-/S- IDs minted (reserved U-8900..U-9199 /
  S-7000..S-7199 are for central re-classify; nothing needed — no plates).

## Drained

drained-by=sweep-20260603-2220; 0 plates, 0 bookmarks, 0 renames (ALL-SKIP: 26 library_skip, rvas=NONE)
