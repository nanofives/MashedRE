# SCRIBE_QUEUE fragment — batch_ar session 3 (ar_s3)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep + central re-classify) writes hooks.csv / trackers.

## Queued

2026-06-03  ar_s3  bucket=re/analysis/bucket_render_00538d60_005401d0  confidence=C1->C2  rvas=NONE  note=subsystem_observed=third-party-library[renderware] for ALL 50 (two vendored RW plugin modules: plugin-ID 0x11d collision toolkit + plugin-ID 0x120 material-effects toolkit; zero genuine game glue); library_skip=00538d60:renderware,00539900:renderware,00539ec0:renderware,0053a6c0:renderware,0053a7a0:renderware,0053b7b0:renderware,0053bdf0:renderware,0053c5d0:renderware,0053c6e0:renderware,0053ccc0:renderware,0053cd80:renderware,0053d040:renderware,0053d060:renderware,0053d090:renderware,0053d0b0:renderware,0053d200:renderware,0053d400:renderware,0053d460:renderware,0053d5a0:renderware,0053d690:renderware,0053d6c0:renderware,0053d820:renderware,0053d920:renderware,0053dba0:renderware,0053de50:renderware,0053dea0:renderware,0053e430:renderware,0053e5d0:renderware,0053e690:renderware,0053e6f0:renderware,0053e730:renderware,0053e760:renderware,0053e790:renderware,0053e7f0:renderware,0053e830:renderware,0053eaa0:renderware,0053eca0:renderware,0053f150:renderware,0053fa20:renderware,0053fb00:renderware,0053fb70:renderware,0053fba0:renderware,0053fbb0:renderware,0053fd30:renderware,0053fea0:renderware,0053fec0:renderware,00540080:renderware,00540100:renderware,00540160:renderware,005401d0:renderware; actual pool slot=Mashed_pool11

## Notes for the sweep / central re-classify

- **Count**: 50 candidates → 0 plated C1->C2, 50 library_skip (kept C1,
  reclass-OUT to third-party-library[renderware]). 0 drift-skips (all 50
  confirmed C1 via anchored `^<rva>,` grep at session start). NO bucket dir
  was created (ALL-SKIP — aq_s2/aq_s4 precedent).
- **All 50 decompiled fresh in Mashed_pool11 (session 7e2f45ca, read-only,
  2026-06-03); decisions are per-RVA from the decomp, not address-band.**
- **Module 1 — plugin ID 0x11d (collision toolkit), 28 RVAs
  00538d60..0053e7f0** (00538d60, 00539900, 00539ec0, 0053a6c0, 0053a7a0,
  0053b7b0, 0053bdf0, 0053c5d0, 0053c6e0, 0053ccc0, 0053cd80, 0053d040,
  0053d060, 0053d090, 0053d0b0, 0053d200, 0053d400, 0053d460, 0053d5a0,
  0053d690, 0053d6c0, 0053d820, 0053d920, 0053dba0, 0053de50, 0053dea0,
  0053e430, 0053e5d0, 0053e690, 0053e6f0, 0053e730, 0053e760, 0053e790,
  0053e7f0). Library-confirm evidence:
  - Registrar FUN_0053d0b0 registers plugin ID 0x11d via FUN_004c2d90 +
    **RpAtomicRegisterPlugin (FidDB-named in pool11)** + FUN_004f0910/
    FUN_004f0940 + FUN_004e8f50/FUN_004e8f80, storing plugin-offset globals
    DAT_007dc5a8/DAT_007dc5ac/DAT_007dc5b0/DAT_007dc5b4 (cited at
    0x0053d0b0 body).
  - Engine fn-table DAT_007d3ff8 +0x108/+0x10c allocs with memhints
    0x1011d (0x0053d460) / 0x3002c (0x0053d5a0) / 0 (0x0053dba0,
    0x0053e730, 0x0053e760).
  - RW error idiom FUN_004d7ff0 + FUN_004d8480 with module id 0x11d
    (0x0053d460, 0x0053d5a0, 0x0053d920) and code 0x80000013.
  - RW stream API FUN_004cc580/FUN_004cbe80 (write, 0x0053d6c0),
    FUN_004cc5e0/FUN_004cbd30/FUN_004cc790/FUN_004cc050 (read, 0x0053d820,
    0x0053d920) with stream version 0x37002 (inside the RW 3.5-3.7 window).
  - BVH traversal kernels (00538d60 ray-vs-world, 00539900 AABB-vs-world,
    00539ec0 sphere-vs-world, 0053a7a0/0053b7b0/0053bdf0 atomic variants,
    0053c6e0/0053cd80 morph-interpolated variants) call ONLY: FidDB
    RpAtomicGetWorldBoundingSphere (0053a6c0/0053c5d0 dispatchers, switch
    on bound-type enum [obj+0x18]), the vendored 0x00547xxx math band
    (FUN_00547bf0 tri-vs-AABB, FUN_00547450 tri-vs-sphere — ap_s1/s2
    library-cited), FastInvSqrt FUN_004c3b90 / FastSqrt FUN_004c3b30, RW
    frame/matrix FUN_004c0ed0/FUN_004c4dc0/FUN_004c3d90, and CRT qsort
    FUN_005c24e0. Float constants are .rdata (DAT_005d757c, _DAT_005cc320,
    _DAT_005e4574, _DAT_005cea1c, _DAT_005ce54c, _DAT_005cd03c,
    _DAT_005cd120, _DAT_005ceac0, _DAT_005cc32c, _DAT_005e4578).
  - SAH BVH builder chain 0053dba0→0053dea0 (recursive split) →0053e430/
    0053e5d0/0053e690/0053e6f0 (split-cost helpers) →0053e730/0053e760
    (node allocs) →0053de50/0053e790/0053e7f0 (free/measure) — pure
    toolkit, zero game globals.
  - Accessors 0053d040/0053d060/0053d090/0053d200 are plugin-offset
    one-liners on DAT_007dc5b0 (BVH slot set/release/test); release calls
    FUN_004522d0.
- **Module 2 — plugin ID 0x120 (material-effects toolkit), 22 RVAs
  0053e830..005401d0** (0053e830, 0053eaa0, 0053eca0, 0053f150, 0053fa20,
  0053fb00, 0053fb70, 0053fba0, 0053fbb0, 0053fd30, 0053fea0, 0053fec0,
  00540080, 00540100, 00540160, 005401d0). Library-confirm evidence:
  - Registrar FUN_0053eaa0 registers plugin ID 0x120 via FUN_004c2d90 +
    FUN_004e80c0/FUN_004e80f0 + **RpAtomicRegisterPlugin (FidDB-named)** +
    FUN_004e7da0 + FUN_004f0910/FUN_004f0940, storing DAT_007dc5dc/
    DAT_007dc5ec/DAT_007dc5f0 (cited at 0x0053eaa0 body).
  - Per-material effect record: freelist alloc via engine fn-table
    DAT_007d3ff8+0x118 (freelist DAT_007dc5e8) with memhint 0x30120
    (0x0053fbb0); 0x34-byte record = 2 x 0x18-byte texture-stage slots
    (type tags at +0x14: 1/2/4/5 switch in 0x0053eca0) + mode dword at
    +0x30 (getter 0x0053fea0).
  - Mode configurator 0053fbb0 (switch param_2 = 1..6 writing stage
    types + FUN_004d8560 state calls); stage setters/getters 0053f150,
    0053fd30, 00540080, 00540100, 00540160, 005401d0 all scan the 2-slot
    table for a tag and write texture ptr / value (RW texture release
    FUN_004c5a60; refcount inc at [tex+0x54]). No-match fallthrough
    writes near-null (uRam00000000 / _DAT_00000008) — unchecked-find
    idiom, noted mechanically.
  - Stream-read callbacks 0053fa20/0053fb00 (FUN_004cc790 4-byte read,
    lazy pipeline init via in-module FUN_00543b10/FUN_00543b20); direct
    lazy-init 0053fb70; flag getter 0053fba0.
  - Texture builders 0053e830 (dual-raster combine: FUN_004cdca0 raster
    create, FUN_004d52d0, FUN_004c77c0 texture create, FUN_004c5ae0 name
    set, 0x1e-char interleaved name concat) and 0053fec0 (texture-pair
    install with texture-dict lookup FUN_004c5ca0/FUN_004c5c00/
    FUN_004c5bc0, fallback combine FUN_00543b30) — RW API only.
  - Zero game globals in any of the 22 (only DAT_007dc5xx plugin band,
    DAT_007d3ff8 engine table, .rdata float _DAT_005cc320).
- **Per the library-confirm rule none were renamed; no master mutation;
  no bookmarks.** All MCP calls were read-only against pool11.
- **DEFERRED cross-refs for central re-classify**: 0053d0b0 carries
  D-0247 and 0053eaa0 carries D-0246 (rw_engine_init_cont1 plugin-registrar
  deferrals, plates at re/analysis/rw_engine_init_cont1/0053d0b0.md /
  0053eaa0.md). Both registrars are now library-confirmed renderware —
  central re-classify can close/annotate those D-rows with this evidence.
- **Existing C1 plates remain as historical notes**: 44 in
  re/analysis/bucket_0052df70/ (batch_z s3), 4 in
  re/analysis/bucket_004fcac3/ (batch_aa s3: 00540080/00540100/00540160/
  005401d0), 2 in re/analysis/rw_engine_init_cont1/. This session's decomp
  re-verified their mechanical content; no contradictions found.
- **Uncertainties**: NONE minted (author-only; no plates authored). No new
  bare [UNCERTAIN] markers were written anywhere.
- **Stubs**: NONE minted.
- **Pool slot**: pre-assigned Mashed_pool11 opened read-only FIRST TRY at
  the FLAT mashed_pool location with program_name="MASHED.exe" (session
  7e2f45ca); program_close called. Recorded in .pool_slot_ar_s3.
- No hooks.csv / STUBS.md / UNCERTAINTIES.md / DEFERRED.md /
  re/SCRIBE_QUEUE.md writes. No re-classify run. Reserved ranges
  U-9200..U-9499 / S-7200..S-7399 untouched (central re-classify mints).

## Drained

drained-by=sweep-20260604-0020; 0 plates, 0 bookmarks, 0 renames (ALL-SKIP: library_skip, rvas=NONE)
